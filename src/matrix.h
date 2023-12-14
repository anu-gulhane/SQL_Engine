#include "cursor.h"

#pragma once
/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Matrix
{

public:
    vector<string> columns;
    string sourceFileName = "";
    string matrixName = "";
    int columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    int maxRowsPerBlock ;
    int maxColumnsPerBlock;
    vector<uint> rowsPerBlockCount;
    //addition
    uint subMatrixRowCount = 0;
    uint subMatrixColumnCount = 0;
    //----------------------------------------------------------
    
    
    bool blockify();
    bool extractColumnNames(string firstLine);
    void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string matrixName);
    bool load();
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    // int getColumnIndex(string columnName);
    // void stats(int blocksRead,int blocksAccess,int blocksWritten);
    void unload();
    void renameMatrix(string renameFromMatrixName, string renameToMatrixName);
    void transposeMatrix(string matrixName);
    bool checkSymmetry(string matrixName);
    void compute(string matrixName);
    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Matrix::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << endl;
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Matrix::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}
};