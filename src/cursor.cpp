#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}
Cursor::Cursor(string matrixName, int rowIndex,int columnIndex){
    logger.log("Cursor::cursor matrix");
    this->page = bufferManager.getPage(matrixName, rowIndex, columnIndex);
    logger.log("getting page rows"+to_string((this->page).pageRowCount));
    logger.log("getting page columns"+to_string((this->page).pageColumnCount));
    this->pagePointer=0;
    this->rowPointer = 0;
    this->columnPointer = 0;
    this->matrixName = matrixName;
}
/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    // logger.log("Cursor::geNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}
vector<int> Cursor::getNext(int rowindex,int columnIndex)
{
    logger.log("Cursor::geNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    // if(result.empty()){
    //     matrixCatalogue.getMatrix(this->matrixName)->getNextPage(this);
    //     if(!this->pagePointer){
    //         result = this->page.getRow(this->pagePointer);
    //         this->pagePointer++;
    //     }
    // }
    return result;
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

void Cursor::operator=(const Cursor& c) 
{ 
     logger.log("Cursor::Cursor operator overloading");
    this->page = bufferManager.getPage(c.tableName,c.pageIndex);
    this->pagePointer = c.pagePointer;
    this->tableName = c.tableName;
    this->pageIndex = c.pageIndex;
} 