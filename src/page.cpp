#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowIndex=0;
    this->colIndex=0;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    // logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

Page::Page(string matrixName, int rowIndex, int columnIndex)
{
    logger.log("Page::Page Matrix");
    this->matrixName = matrixName;
    this->rowIndex = rowIndex;
    this->colIndex = columnIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(rowIndex)+"_"+to_string(columnIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(this->matrixName);
    this->columnCount = matrix.columnCount;
    this->rowCount = matrix.rowCount;
    uint maxRowCount = matrix.maxRowsPerBlock;
    uint maxColumnCount = matrix.maxColumnsPerBlock;
    int r=maxRowCount;
    int c=maxColumnCount;
    logger.log(to_string(matrix.maxRowsPerBlock)+" "+to_string(matrix.maxColumnsPerBlock));
    logger.log(to_string(matrix.subMatrixRowCount)+"  f "+to_string(matrix.subMatrixColumnCount));
    if(rowIndex==matrix.subMatrixRowCount-1)
        r=matrix.rowCount%maxRowCount;
    if(columnIndex==matrix.subMatrixColumnCount-1)
        c=matrix.columnCount%maxColumnCount;
    logger.log("rows and columns in each page"+to_string(r)+" "+to_string(c));
    vector<int> row(c, 0);
    this->rows.assign(r, row);
    logger.log("opening "+this->pageName);
    ifstream fin(pageName, ios::in);
    // this->rowCount = matrix.rowsPerBlockCount[pageIndex];
    int number;
    // int cr=min((int)maxRowCount,(int)matrix.rowCount);
    // int cc=min((int)maxColumnCount,(int)matrix.columnCount);
    // logger.log("---->"+to_string(cc)+" "+to_string(cr));
    for (uint rowCounter = 0; rowCounter < r; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < c; columnCounter++)
        {
            fin >> number;
           // logger.log(to_string(number));
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}


Page::Page(string matrixName)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    // this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    // logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}
vector<vector<int>>Page::getMatrix()
{
    return this->rows;
}
Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}
Page::Page(string matrixName, int block_i,int block_j,vector<vector<int>> rows, int rowStart,int columnStart,int rowEnd,int columnEnd)
{
    logger.log("Page::Page");
    this->pageRowCount=rowEnd-rowStart;
    this->pageColumnCount=columnEnd-columnStart;
    this->tableName = matrixName;
    this->rowIndex = block_i;
    this->colIndex=block_j;
    this->rows = rows;
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(rowIndex)+"_"+to_string(colIndex);
    logger.log("setting rows and columns"+ to_string(this->pageRowCount)+" "+to_string(this->pageColumnCount));
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}
void Page::writePage(string matrixName,int rowStart,int columnStart,int rowEnd,int columnEnd){
    logger.log("writePage :"+this->pageName);
    ofstream fout(this->pageName, ios::out);
    for (int rowCounter = rowStart; rowCounter < rowEnd; rowCounter++)
    {
        for (int columnCounter = columnStart; columnCounter <columnEnd; columnCounter++)
        {
            // if (columnCounter != columnStart)
            //logger.log(to_string(rows[rowCounter][columnCounter]));
            fout << rows[rowCounter][columnCounter];
            fout << " ";
        }
        fout << endl;
    }
    // this->pageRowCount=rowEnd-rowStart+1;
    // this->pageColumnCount=columnEnd-columnStart+1;
    fout.close();

}
