#include "global.h"
#include <filesystem>
namespace fs = std::filesystem;
/**
 * @brief 
 * SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING
<aggregate(attribute)> <bin_op> <attribute_value> RETURN
<aggregate_func(attribute)>
T1
<-
GROUP
BY
Dnumber
FROM
DEPARTMENT
HAVING
MIN
Mge_ssn
>
6
RETURN
COUNT
Dnumber
 */
bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY " +to_string(tokenizedQuery.size()));
    if (tokenizedQuery.size() != 15 ||tokenizedQuery[3]!="BY"|| tokenizedQuery[5]!="FROM" || tokenizedQuery[7]!="HAVING" ||tokenizedQuery[12]!="RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupbyResultRelation=tokenizedQuery[0];
    parsedQuery.groupbyAttribute=tokenizedQuery[4];
    parsedQuery.groupbyRelation=tokenizedQuery[6];
    parsedQuery.aggregateFunction=tokenizedQuery[8];
    parsedQuery.aggregateAttribute=tokenizedQuery[9];
    string binaryOperator = tokenizedQuery[10];
    if (binaryOperator == "<")
        parsedQuery.groupbyBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupbyBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupbyBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupbyBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupbyBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupbyBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.aggregateValue=stoll(tokenizedQuery[11]);
    parsedQuery.returnFunction=tokenizedQuery[13];
    parsedQuery.returnAttribute = tokenizedQuery[14];
    logger.log("groupby attribute "+parsedQuery.groupbyAttribute+" aggregate Attribute "+parsedQuery.aggregateAttribute+" return attribute "+parsedQuery.returnAttribute);
    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");

    if (tableCatalogue.isTable(parsedQuery.groupbyResultRelation))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupbyRelation))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupbyAttribute, parsedQuery.groupbyRelation) || !tableCatalogue.isColumnFromTable(parsedQuery.aggregateAttribute, parsedQuery.groupbyRelation) || !tableCatalogue.isColumnFromTable(parsedQuery.returnAttribute, parsedQuery.groupbyRelation))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}
void writeTempPage(string relationName,int blockNumber,vector<vector<int>>data,int rowCount,int columnCount)
{
    logger.log(" temp writePage " + to_string(rowCount)+" "+ to_string(columnCount));
    string pageName = "../data/temp/" + relationName + "_Page" + to_string(blockNumber);
    ofstream fout(pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << data[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
    logger.log("temp write page successfull");
}
Table* prepareTempTable(){
    logger.log("Preparing temp file");
    string newTableName = "temp_groupby"+parsedQuery.groupbyRelation;
    Table* table=tableCatalogue.getTable(parsedQuery.groupbyRelation);
    Table *newtable = new Table(newTableName);
    newtable->rowCount = table->rowCount;
    newtable->rowsPerBlockCount=table->rowsPerBlockCount;
    newtable->blockCount=table->blockCount;
    newtable->columns.push_back(parsedQuery.groupbyAttribute);
    newtable->columns.push_back(parsedQuery.aggregateAttribute);
    newtable->columns.push_back(parsedQuery.returnAttribute);
    newtable->columnCount = newtable->columns.size();
    newtable->maxRowsPerBlock = table->maxRowsPerBlock;
    newtable->tableName = newTableName;
    tableCatalogue.insertTable(newtable);
    return newtable;
}
void addDataNewTable(Table* newtable,Table* table){
int startBlock=0;
    int endBlock= newtable->blockCount;
    vector<vector<int>>data;
    int grpColumn=table->getColumnIndex(parsedQuery.groupbyAttribute);
    int aggColumn=table->getColumnIndex(parsedQuery.aggregateAttribute);
    int returnColumn=table->getColumnIndex(parsedQuery.returnAttribute);
    for(int i=startBlock;i<endBlock;i++){
        Cursor cursor(parsedQuery.groupbyRelation,i);
        int rows=newtable->rowsPerBlockCount[i];
        logger.log("no of rows to push "+to_string(rows));
        for(int j=0;j<rows;j++){
            vector<int>values=cursor.getNext();
            vector<int>refinedValues;
            refinedValues.push_back(values[grpColumn]);
            refinedValues.push_back(values[aggColumn]);
            refinedValues.push_back(values[returnColumn]);
            data.push_back(refinedValues);
        }
        writeTempPage(newtable->tableName,i,data,rows,newtable->columnCount);
        data.clear();
    }
    //add sorting required parameters
    parsedQuery.sortRelationName=newtable->tableName;
    parsedQuery.sortColumns_order.push_back(make_pair(0,0));
}
class AggregateValues
{
    public:
    long long int minValue;
    long long int maxValue;
    long long int sumValue;
    long long int countValue;
    long long int avgValue;
    AggregateValues()
    {
        minValue=LLONG_MAX;
        maxValue=LLONG_MIN;
        sumValue=0;
        countValue=0;
        avgValue=0;
    }
    void updateAggregateValues(long long int value)
    {
        if(minValue>value)
            minValue=value;
        if(maxValue<value)
            maxValue=value;
        sumValue+=value;
        countValue+=1;
        avgValue=sumValue/countValue;
    }
    void clear()
    {
        minValue=LLONG_MAX;
        maxValue=LLONG_MIN;
        sumValue=0;
        countValue=0;
        avgValue=0;
    }
};
int evaluateBinOpt(long long int value1, long long int value2, BinaryOperator binaryOperator)
{
    logger.log("binary operator "+to_string(binaryOperator)+" "+to_string(value1)+ " "+to_string(value2));
    switch (binaryOperator)
    {
    case LESS_THAN:
         if (value1 < value2) return 0; //check for other matching tuples using L and K
         if (value1 >= value2) return -1; //increment j
    case GREATER_THAN:
        if (value1 > value2) return 0; //check for other matching tuples using L and K
        if (value1 <= value2) return 1; //increment i
    case LEQ:
        if (value1 <= value2) return 0; //check for other matching tuples using L and K
        if (value1 > value2) return -1; //increment j
    case GEQ:
        if (value1 >= value2) return 0; //check for other matching tuples using L and K
        if (value1 < value2) return 1; //increment i
    case 4:
        if (value1 == value2) return 0; //check for other matching tuples using L and K
        if (value1 < value2) return 1; //increment i
        return -1; //increment j

    default:
        return 1;
    }
}
bool checkHavingCondition(AggregateValues* havingAttribute)
{
    string AggregateFunction=parsedQuery.aggregateFunction;
    long long int aggregateValue=parsedQuery.aggregateValue;
    int flag;
    if(AggregateFunction=="MIN"){
        flag=evaluateBinOpt(havingAttribute->minValue,aggregateValue,parsedQuery.groupbyBinaryOperator);
    }
    else if(AggregateFunction=="MAX"){
        flag=evaluateBinOpt(havingAttribute->maxValue,aggregateValue,parsedQuery.groupbyBinaryOperator);
    }
    else if(AggregateFunction=="SUM"){
        flag=evaluateBinOpt(havingAttribute->sumValue,aggregateValue,parsedQuery.groupbyBinaryOperator);
    }
    else if(AggregateFunction=="COUNT")
    {
        flag=evaluateBinOpt(havingAttribute->countValue,aggregateValue,parsedQuery.groupbyBinaryOperator);
    }
    else if(AggregateFunction=="AVG")
    {
        flag=evaluateBinOpt(havingAttribute->avgValue,aggregateValue,parsedQuery.groupbyBinaryOperator);
    }
    if(flag==0)
        return true;
    return false;
    
}
int getReturnAttribute(AggregateValues* returnAttribute)
{
    string returnFunction=parsedQuery.returnFunction;
    if(returnFunction=="MIN"){
        return returnAttribute->minValue;
    }
    else if(returnFunction=="MAX"){
        return returnAttribute->maxValue;
    }
    else if(returnFunction=="SUM"){
        return returnAttribute->sumValue;
    }
    else if(returnFunction=="COUNT")
    {
        return returnAttribute->countValue;
    }
    else if(returnFunction=="AVG")
    {
        return returnAttribute->minValue;
    }
    return 1;
}
void deleteTemporary(string tableName){
    logger.log("Table::deleteTemporary");
    logger.log(tableName);
    Table *table = tableCatalogue.getTable(tableName);
    tableCatalogue.deleteTable(tableName);

    std::string folderPath = "../data/temp/";                  // Replace with the actual folder path
    std::string tempsubstring = tableName + "_Page"; // Substring to find

    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            std::string filename = entry.path().filename().string();

            // Check if the oldFileName contains the oldSubstring
            size_t found = filename.find(tempsubstring);
            if (found != std::string::npos)
            {
                // Rename the file
                if(fs::remove(filename)==0)
                logger.log("Deleted: "+filename);
                else
                logger.log("Deletion failed!!");
            }
            else
            {
                logger.log("Not found " + entry.path().filename().string());
            }
        }
    }
    return;
}
void executeGROUPBY()
{
    logger.log("executeGROUPBY");
    //copy data to resultant Relation first
    //Prepare the metadata for new result table
    Table* table=tableCatalogue.getTable(parsedQuery.groupbyRelation);
    Table* newTable=prepareTempTable();
    //Add data to new result table
    addDataNewTable(newTable,table);
    executeSORT();
    logger.log("after execute sort");
    // return ;
    AggregateValues* returnAttribute=new AggregateValues();
    AggregateValues* havingAttribute=new AggregateValues();
    Cursor cursor=newTable->getCursor();
    logger.log("got cursor");
    vector<int>row=cursor.getNext();
    bool flag=false;
    int currGroupingAttribute=0;
    vector<string>columns={parsedQuery.groupbyAttribute,parsedQuery.returnFunction+parsedQuery.returnAttribute};
    logger.log("columns in resultant table "+columns[0]+" "+columns[1]);
    Table* resultantTable=new Table(parsedQuery.groupbyResultRelation,columns); 
    // = new Table(parsedQuery.joinResultRelationName, joinColumns);
    logger.log("group by begins");
    while(!row.empty())
    {
        int groupingAttribute=row[0];
        logger.log("row values "+to_string(row[0])+" "+to_string(row[1])+" "+to_string(row[2]));
        if(flag==false)
        {
            currGroupingAttribute=groupingAttribute;
            flag=true;
        }
        if(currGroupingAttribute!=groupingAttribute)
        {
            int valueToBeReturned=getReturnAttribute(returnAttribute);
            logger.log("return condition checker"+to_string(currGroupingAttribute)+" "+to_string(valueToBeReturned)+" "+to_string(havingAttribute->avgValue));
            if(checkHavingCondition(havingAttribute))
            {
                // int valueToBeReturned=getReturnAttribute(returnAttribute);
                vector<int>tempRow={currGroupingAttribute,valueToBeReturned};
                logger.log("row in the output file "+to_string(currGroupingAttribute)+" "+to_string(valueToBeReturned));
                resultantTable->writeRow<int>(tempRow);
            }
            havingAttribute->clear();
            returnAttribute->clear();
            currGroupingAttribute=groupingAttribute;
        }
        
        returnAttribute->updateAggregateValues(row[2]);
        havingAttribute->updateAggregateValues(row[1]);
            
       row=cursor.getNext();    
    }
    int valueToBeReturned=getReturnAttribute(returnAttribute);
    logger.log("return condition checker"+to_string(currGroupingAttribute)+" "+to_string(valueToBeReturned));
    if(checkHavingCondition(havingAttribute))
    {
        
        vector<int>tempRow={currGroupingAttribute,valueToBeReturned};
        resultantTable->writeRow<int>(tempRow);
        logger.log("row in the output file "+to_string(currGroupingAttribute)+" "+to_string(valueToBeReturned));
    }
    logger.log("blockify the file begins");
    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        resultantTable->unload();
        delete resultantTable;
    }
    //delete the temporary file
    tableCatalogue.deleteTable(newTable->tableName);
    return;
}