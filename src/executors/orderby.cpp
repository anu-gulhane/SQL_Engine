#include "global.h"
/**
 * @brief 
 * SYNTAX: <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
 */
bool syntacticParseORDERBY()
{
    logger.log("syntacticParseORDERBY" +to_string(tokenizedQuery.size()));
    if (tokenizedQuery.size() != 8 || (tokenizedQuery[5] != "ASC" && tokenizedQuery[5]!="DESC") || tokenizedQuery[6]!="ON" )
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderbyResultRelationName=tokenizedQuery[0];
    parsedQuery.orderbyColumnName = tokenizedQuery[4];
    parsedQuery.orderbyStartegy = tokenizedQuery[5];
    parsedQuery.orderbyRelationName = tokenizedQuery[7];
    
    return true;
}

bool semanticParseORDERBY()
{
    logger.log("semanticParseORDERBY");

    if (tableCatalogue.isTable(parsedQuery.orderbyResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.orderbyRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.orderbyColumnName, parsedQuery.orderbyRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.orderbyRelationName);
    vector<string>columns= table->columns; 
    for(int i=0;i<columns.size();i++){   
        if(columns[i]==parsedQuery.orderbyColumnName){
            if(parsedQuery.orderbyStartegy=="ASC")
            parsedQuery.sortColumns_order.push_back(make_pair(i,0));
            else
            parsedQuery.sortColumns_order.push_back(make_pair(i,1));
            break;
      }
}
    return true;
}
void writeNewPage(string relationName,int blockNumber,vector<vector<int>>data,int rowCount,int columnCount)
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
Table* prepareResultTable(){
string newTableName = parsedQuery.orderbyResultRelationName;
    Table* table=tableCatalogue.getTable(parsedQuery.orderbyRelationName);
    Table *newtable = new Table(newTableName);
    newtable->rowCount = table->rowCount;
    newtable->rowsPerBlockCount=table->rowsPerBlockCount;
    newtable->blockCount=table->blockCount;
    newtable->columns=table->columns;
    newtable->columnCount = table->columnCount;
    newtable->maxRowsPerBlock = table->maxRowsPerBlock;
    newtable->tableName = newTableName;
    tableCatalogue.insertTable(newtable);
    return newtable;
}
void executeORDERBY()
{
    logger.log("executeORDERBY");
    //copy data to resultant Relation first
    //Prepare the metadata for new result table
    Table* newtable=prepareResultTable();
    //Add data to new result table
    int startBlock=0;
    int endBlock= newtable->blockCount;
    vector<vector<int>>data;
    for(int i=startBlock;i<endBlock;i++){
        Cursor cursor(parsedQuery.orderbyRelationName,i);
        int rows=newtable->rowsPerBlockCount[i];
        logger.log("no of rows to push "+to_string(rows));
        for(int j=0;j<rows;j++){
            data.push_back(cursor.getNext());
        }
        writeNewPage(newtable->tableName,i,data,rows,newtable->columnCount);
        data.clear();
    }
    parsedQuery.sortRelationName=newtable->tableName;
    executeSORT();
    return;
}