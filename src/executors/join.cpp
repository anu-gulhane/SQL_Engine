#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

int evaluateBinOpt(int value1, int value2, BinaryOperator binaryOperator)
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
 
void sortJOINRelation(string relationName,string columnname){
   parsedQuery.sortRelationName=relationName;
   parsedQuery.orderbyColumnName=columnname;
    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    vector<string>columns= table->columns;
    for(int i=0;i<columns.size();i++){   
        if(columns[i]==parsedQuery.orderbyColumnName){
            parsedQuery.sortColumns_order.push_back(make_pair(i,0));
            break;
      }
}
executeSORT();
parsedQuery.sortColumns_order.clear();
parsedQuery.orderbyColumnName="";
parsedQuery.sortRelationName="";
}
void copyPageData(string relationName,int blockNumber,vector<vector<int>>data,int rowCount,int columnCount)
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
Table* copyTable(string relationName)
{
    string newRelationName="temp_"+relationName;
    // string newTableName = parsedQuery.orderbyResultRelationName;
    Table* table=tableCatalogue.getTable(relationName);
    Table *newtable = new Table(newRelationName);
    newtable->rowCount = table->rowCount;
    newtable->rowsPerBlockCount=table->rowsPerBlockCount;
    newtable->blockCount=table->blockCount;
    newtable->columns=table->columns;
    newtable->columnCount = table->columnCount;
    newtable->maxRowsPerBlock = table->maxRowsPerBlock;
    newtable->tableName = newRelationName;
    tableCatalogue.insertTable(newtable);
    int startBlock=0;
    int endBlock= newtable->blockCount;
    vector<vector<int>>data;
    for(int i=startBlock;i<endBlock;i++){
        Cursor cursor(relationName,i);
        int rows=newtable->rowsPerBlockCount[i];
        logger.log("no of rows to push "+to_string(rows));
        for(int j=0;j<rows;j++){
            data.push_back(cursor.getNext());
        }
        copyPageData(newtable->tableName,i,data,rows,newtable->columnCount);
        data.clear();
    }
    return newtable;
}


void executeJOIN()
{
    //sort table1 and table2 based on col1 and col2 
    Table* table1 = copyTable(parsedQuery.joinFirstRelationName);
    Table* table2 = copyTable(parsedQuery.joinSecondRelationName);
    string cmd1 = "SORT "+table1->tableName+ " By "+parsedQuery.joinFirstColumnName+ " IN ASC";
    sortJOINRelation(table1->tableName,parsedQuery.joinFirstColumnName);
    string cmd2 = "SORT "+table2->tableName+ " By "+parsedQuery.joinSecondColumnName+ " IN ASC";
    sortJOINRelation(table2->tableName,parsedQuery.joinSecondColumnName);
    logger.log("executeJOIN");
    BinaryOperator op = parsedQuery.joinBinaryOperator;
    vector<string>joinColumns;
    for(auto c:table1->columns){
       joinColumns.push_back(c);
    }
      for(auto c:table2->columns){
        joinColumns.push_back(c);
    }
    logger.log("created combined joincolumns");
    // joinColumns.insert((table1->columns).end(),(table2->columns).begin(),(table2->columns).end());
    logger.log("After join columns");
    Table* resultantTable = new Table(parsedQuery.joinResultRelationName, joinColumns);
    logger.log("cretaing result table");
    Cursor cursor1 = table1->getCursor();
    logger.log("creating cursor 1");
    vector<int> row1 = cursor1.getNext();
    logger.log("creating cursor 2");
    Cursor cursor2 = table2->getCursor();
    vector<int> row2 = cursor2.getNext();
    logger.log("getting column Index");
    
    int firstColumnIndex = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int secondColumnIndex = table2->getColumnIndex(parsedQuery.joinSecondColumnName);
    logger.log("first column index , second column index" + to_string(firstColumnIndex)+" "+to_string(secondColumnIndex));
    while(!row1.empty() and !row2.empty())
    {
        logger.log("row1 data"+ to_string(row1[0])+" "+to_string(row1[1]));
        logger.log("row2 data"+ to_string(row2[0])+" "+to_string(row2[1]));
        int flag=evaluateBinOpt(row1[firstColumnIndex],row2[secondColumnIndex],op);
        logger.log("binary operator function "+to_string(flag));
        if(flag==-1)
        {
            row2=cursor2.getNext();
        }
        else if(flag==1)
        {
            row1=cursor1.getNext();
        }
        else if(flag==0)
        {
            logger.log("binary operator satisfied");

            vector<int>row;
            for(auto each:row1)
                row.push_back(each);
            for(auto each:row2)
                row.push_back(each);
            // row.insert(row1.end(),row2.begin(),row2.end());
            logger.log("row written successfully");
            resultantTable->writeRow<int>(row);
            row.clear();
            logger.log("before deep copy of s");
            Cursor cursor_s=cursor2;
            vector<int>row_s=cursor_s.getNext();
            while(!row_s.empty() and (evaluateBinOpt(row1[firstColumnIndex],row_s[secondColumnIndex],op)==0))
            {   
                logger.log("evaluaton operator in s" +to_string(evaluateBinOp(row1[firstColumnIndex],row_s[secondColumnIndex],op)));
                logger.log("row_s data"+ to_string(row_s[0])+" "+to_string(row_s[1]));
                vector<int>rowTemp;
                for(auto each:row1)
                    rowTemp.push_back(each);
                for(auto each:row_s)
                    rowTemp.push_back(each);
                resultantTable->writeRow<int>(rowTemp);
                rowTemp.clear();
                row_s=cursor_s.getNext();
            }
            logger.log("finding potential s done");
            logger.log("before deep copy of l");
            Cursor cursor_l=cursor1;
            vector<int>row_l=cursor_l.getNext();
            while(!row_l.empty() and evaluateBinOpt(row_l[firstColumnIndex],row2[secondColumnIndex],op)==0)
            {
                logger.log("row_l data"+ to_string(row_l[0])+" "+to_string(row_l[1]));
                vector<int>rowTemp;
                for(auto each:row_l)
                    rowTemp.push_back(each);
                for(auto each:row2)
                    rowTemp.push_back(each);
                resultantTable->writeRow<int>(rowTemp);
                rowTemp.clear();
                row_l=cursor_l.getNext();
            }
            logger.log("finding potential s done");
            row1=cursor1.getNext();
            row2=cursor2.getNext();
        }

    }
    logger.log("sort merge join is done");
    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        resultantTable->unload();
        delete resultantTable;
    }
    logger.log(" join temp pages deleted ");
    tableCatalogue.deleteTable(table1->tableName);
    tableCatalogue.deleteTable(table2->tableName);
    
}