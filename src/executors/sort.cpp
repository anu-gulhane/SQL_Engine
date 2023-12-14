#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    // parsedQuery.sortColumnName={};
    // parsedQuery.
    logger.log("syntacticParseSORT");
    int by_pos=-1;
    int in_pos=-1;
    for(int i=0;i<tokenizedQuery.size();i++){
        if(tokenizedQuery[i]=="BY")
        by_pos=i;
        else if(tokenizedQuery[i]=="IN")
        in_pos=i;
    }
    if(by_pos!=2 || by_pos==-1 || in_pos==-1 || by_pos>in_pos){
        cout<<"first"<<endl;
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    // parsedQuery.sortResultRelationName = tokenizedQuery[1];
    parsedQuery.sortRelationName = tokenizedQuery[1];
    for(int i=by_pos+1;i<in_pos;i++){
        parsedQuery.sortColumnName.push_back(tokenizedQuery[i]);
    }
    logger.log("order of columns: in_pos"+ to_string(in_pos));
    for(int i=in_pos+1;i<tokenizedQuery.size();i++){
        logger.log(tokenizedQuery[i]);
        if(tokenizedQuery[i]!="ASC" && tokenizedQuery[i]!="DESC")
            {
                logger.log("not matching "+tokenizedQuery[i]+" ASC"+" DESC");
                cout<<"SYNTAX ERROR"<<endl;
                return false;
            }
        if(tokenizedQuery[i]=="ASC")
        parsedQuery.sortOrder.push_back(0);
        else
        parsedQuery.sortOrder.push_back(1);
    }
    if(parsedQuery.sortOrder.size()!=parsedQuery.sortColumnName.size())
    {
        logger.log("size doesn't match" +to_string(parsedQuery.sortOrder.size())+" "+to_string(parsedQuery.sortColumnName.size()));
    return false;
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    // if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
    //     cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
    //     return false;
    // }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }
for(int i=0;i<parsedQuery.sortColumnName.size();i++){
    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName[i], parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }
}
 Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
vector<string>columns= table->columns; 
for(int j=0; j<parsedQuery.sortColumnName.size(); j++)
{
    for(int i=0;i<columns.size();i++){
        if(columns[i]==parsedQuery.sortColumnName[j]){
        parsedQuery.sortColumns_order.push_back(make_pair(i,parsedQuery.sortOrder[j]));
      }
    }

}
// for(int i=0;i<columns.size();i++){   
// for(int j=0;j<parsedQuery.sortColumnName.size();j++){
//       if(columns[i]==parsedQuery.sortColumnName[j]){
//         parsedQuery.sortColumns_order.push_back(make_pair(i,parsedQuery.sortOrder[j]));
//       }
// }
    
// }

    return true;
}
static bool customFn(vector<int>& row1,vector<int>& row2)
{
    // logger.log("inside custom function");
    for(auto each:parsedQuery.sortColumns_order)
    {
        // logger.log(to_string(each.first)+" "+to_string(each.second)+" "+to_string(row1.size())+ " "+ to_string(row2.size()));
        int colNumber=each.first;
        int order=each.second;
        if(order==0)
        {
            if(row1[colNumber]<row2[colNumber])
                return true;
            else if(row1[colNumber]>row2[colNumber])
                return false;
            else    
                continue;
        }
        else if(order==1)
        {
            if(row1[colNumber]>row2[colNumber])
                return true;
            else if(row1[colNumber]<row2[colNumber])
                return false;
            else    
                continue;
        }
    }
    return false;
}
class Compare {
public:
    bool operator()(pair<int,vector<int>> temp1, pair<int,vector<int>> temp2)
    {
        vector<int>row1=temp1.second;
        vector<int>row2=temp2.second;
        for(auto each:parsedQuery.sortColumns_order)
        {
            int colNumber=each.first;
            int order=each.second;
            if(order==1)
            {
                if(row1[colNumber]<row2[colNumber])
                    return true;
                else if(row1[colNumber]>row2[colNumber])
                    return false;
                else    
                    continue;
            }
            else if(order==0)
            {
                if(row1[colNumber]>row2[colNumber])
                    return true;
                else if(row1[colNumber]<row2[colNumber])
                    return false;
                else    
                    continue;
            }
        }
        return true;
    }
};
void internalSorting(int blockNumber,string relationName)
{
    logger.log("internalSorting "+ to_string(blockNumber));
    //prepare the path
    Table* table=tableCatalogue.getTable(relationName);
    string pageName = "../data/temp/" + relationName + "_Page" + to_string(blockNumber);
    Cursor cursor(relationName,blockNumber);
    int rows=table->rowsPerBlockCount[blockNumber];
    logger.log("number of rows in that block "+to_string(rows));
    vector<vector<int>>data;
    for(int i=0; i<rows; i++)
    {
        vector<int>d=cursor.getNext();
        data.push_back(d);
        logger.log(to_string(d[0])+ " "+to_string(d[1])+ " "+to_string(d[2])+" "+to_string(d.size()));
    }
    logger.log("data read success");
    logger.log("data size "+to_string(data.size()));
    sort(data.begin(),data.end(),customFn);
    logger.log("custom sorting is done");
    bufferManager.writePage(relationName,blockNumber,data,rows);
    logger.log("internally sorted data written back");

}

bool isPageFull(int n,int blockNumber,string relationName)
{
    // logger.log("in isPageFull function "+to_string(n)+" "+to_string(blockNumber));
    Table* table=tableCatalogue.getTable(relationName);
    // string pageName = "../data/temp/" + relationName + "_Page" + to_string(blockNumber);
    // Cursor cursor(relationName,blockNumber);
    int rows=table->rowsPerBlockCount[blockNumber];
    if(rows==n)
        return true;
    return false;
}
vector<vector<int>>readData(int blockNumber,string relationName)
{
 logger.log("reading data of block "+ to_string(blockNumber) + " in merge function");
    //prepare the path
    Table* table=tableCatalogue.getTable(relationName);
    string pageName = "../data/temp/" + relationName + "_Page" + to_string(blockNumber);
    logger.log("in read data "+pageName);
    Cursor cursor(relationName,blockNumber);
    int rows=table->rowsPerBlockCount[blockNumber];
    logger.log("number of rows in that block "+to_string(rows));
    vector<vector<int>>data;
    for(int i=0; i<rows; i++)
    {
        data.push_back(cursor.getNext());
    }   
    return data;
}
void writePage(string relationName,int blockNumber,vector<vector<int>>data,int rowCount,int columnCount)
{
    logger.log(" temp writePage");
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
vector<vector<int>>readDataTempPages(int blockNumber,string relationName,int columnCount)
{
    Table* table=tableCatalogue.getTable(relationName);
    int rowCount=table->rowsPerBlockCount[blockNumber];
    string pageName = "../data/temp/temp_" + relationName + "_Page" + to_string(blockNumber);
    logger.log("reading data from temp pages" + pageName);
    ifstream fin(pageName, ios::in);
    vector<vector<int>>rows(rowCount,vector<int>(columnCount,0));
    int number;
    for (int rowCounter = 0; rowCounter < rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
    return rows;
}
void merge(int startBlock,int endBlock,string relationName,vector<pair<int,int>>partitionRanges,int columnCount)
{

    int writePageNumber=startBlock;
    vector<vector<vector<int>>>readingPages(9);
    vector<int>pointers(9,0);
    for(int i=0; i<9; i++)
    {
        if(partitionRanges[i].first==-1)
            pointers[i]=-1;
    }
    for(int partitionNumber=0; partitionNumber<9; partitionNumber++)
    {
        int x=partitionRanges[partitionNumber].first;
        int y=partitionRanges[partitionNumber].second;
        if(x<=y and pointers[partitionNumber]!=-1)
        {
            logger.log("reading data of "+to_string(x));
            readingPages[partitionNumber]=readData(x,relationName);
            // partitionRanges[partitionNumber].first+=1;
        }
    }
    priority_queue<pair<int,vector<int>>, vector<pair<int,vector<int>>>, Compare> pq;
    for(int partitionNumber=0; partitionNumber<9; partitionNumber++)
    {
        if(pointers[partitionNumber]!=-1)
        {
            vector<int>row=readingPages[partitionNumber][pointers[partitionNumber]];
            pq.push({partitionNumber,row});
            pointers[partitionNumber]+=1;
        }
    }
    vector<vector<int>>data;
    while(!pq.empty())
    {
        auto elem=pq.top();
        pq.pop();
        int partitionNumber=elem.first;
        logger.log("partition number "+to_string(partitionNumber));
        vector<int>row=elem.second;
        logger.log("data from "+to_string(row[0])+ " "+to_string(row[1])+ " "+to_string(row[2]));
        data.push_back(row);
        if(isPageFull(data.size(),writePageNumber,relationName))
        {
            logger.log("writing page+ "+to_string(writePageNumber)+" "+to_string(data.size()));
            writePage("temp_"+relationName,writePageNumber,data,data.size(),columnCount);
            data.clear();
            writePageNumber++;
        }
        if(pointers[partitionNumber]==-1)
            continue;
        // pointers[partitionNumber]+=1;
        if(pointers[partitionNumber]==readingPages[partitionNumber].size())
        {
            partitionRanges[partitionNumber].first+=1;
            if(partitionRanges[partitionNumber].first>partitionRanges[partitionNumber].second)
            {
                continue;
            }
            else
            {
                pointers[partitionNumber]=0;
                logger.log("reading data of "+to_string(partitionRanges[partitionNumber].first));
                readingPages[partitionNumber]=readData(partitionRanges[partitionNumber].first,relationName);
                // partitionRanges[partitionNumber].first+=1;
            }
        }
        
        row=readingPages[partitionNumber][pointers[partitionNumber]];
        pq.push({partitionNumber,row});
        pointers[partitionNumber]+=1;
    }
    if(data.size()!=0)
    {
        logger.log("writing page+ "+to_string(writePageNumber)+" "+to_string(data.size()));
        writePage("temp_"+relationName,writePageNumber,data,data.size(),columnCount);
        data.clear();
        writePageNumber++;
        
    }
    for(int i=startBlock; i<=endBlock; i++)
    {

        vector<vector<int>>data=readDataTempPages(i,relationName,columnCount);
        bufferManager.writePage(relationName,i,data,data.size());
        string pageName = "../data/temp/temp_" + relationName + "_Page" + to_string(i);
        // Attempt to delete the file
        if (std::remove(pageName.c_str()) == 0) {
            // Deletion successful
            logger.log("File deleted successfully.\n");
        } else {
            // Deletion failed
            perror("Error deleting file");
        }

    }
    logger.log("merge completed");
}
void externalMergeSort(int startBlock,int endBlock,string relationName,int columnCount)
{

    if(startBlock>endBlock)
        return ;
    if(startBlock==endBlock)
    {
        internalSorting(startBlock,relationName);
        return ;
    }
        
    logger.log(" inside external merge sort "+ to_string(startBlock)+ " "+to_string(endBlock)+" "+relationName);
    int totalBlocks=endBlock-startBlock+1;
    int partitionSize=ceil((float)totalBlocks/9);
    logger.log("partition size "+to_string(partitionSize));
    int x=startBlock;
    vector<pair<int,int>>partitionRanges;
    for(int partitionNumber=0; partitionNumber<9; partitionNumber++)
    {
        if(totalBlocks%9==partitionNumber)
            partitionSize-=1;
        int y=x+partitionSize-1;
        logger.log("partition ranges "+to_string(x)+" "+to_string(y));
        externalMergeSort(x,y,relationName,columnCount);
        if(x<=y)
            partitionRanges.push_back({x,y});
        else
            partitionRanges.push_back({-1,-1});
        x=y+1;
    }
    logger.log(" calling merge function "+ to_string(startBlock)+ " "+to_string(endBlock)+" "+relationName);
    // if(startBlock==0 and endBlock==19)
    //     return ;
    merge(startBlock,endBlock,relationName,partitionRanges,columnCount);
}
void executeSORT(){
    for(auto c:parsedQuery.sortColumns_order){
        logger.log("pair ("+ to_string(c.first) + " " + to_string(c.second) +" )");
    }
    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    int startBlock=0;
    int endBlock= table->blockCount-1;
    int columnCount=table->columns.size();
    string relationName=parsedQuery.sortRelationName;
    logger.log("external merge sort "+ to_string(startBlock)+ " "+to_string(endBlock)+" "+relationName);
    externalMergeSort(startBlock,endBlock,relationName,columnCount);
    logger.log("executeSORT");
    return;
}






