#include"bufferManager.h"
#pragma once
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    public:
    Page page;
    int pageIndex;
    string tableName;
    string matrixName;
    int pagePointer;
    int rowPointer;
    int columnPointer;
    int rowIndex;
    int colIndex;

    public:
    Cursor(string tableName, int pageIndex);
    Cursor(string matrixName, int rowIndex,int columnIndex);
    vector<int> getNext();
    vector<int> getNext(int rowIndex,int columnIndex);
    void nextPage(int pageIndex);
    void operator=(const Cursor& C); 
};