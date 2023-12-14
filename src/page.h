#include "logger.h"
#pragma once
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class Page{

    string tableName;
    int pageIndex;
    int columnCount;
    int rowCount;
    int rowIndex;
    int colIndex;

    public:

    vector<vector<int>> rows;
    int pageRowCount=0;
    int pageColumnCount=0;
    string matrixName;
    string pageName = "";
    Page();
    Page(string matrixName);
    Page(string matrixName, int rowIndex, int colIndex);
    Page(string tableName, int pageIndex);
    Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount);
    Page(string matrixName, int block_i,int block_j,vector<vector<int>> rows, int rowStart,int columnStart,int rowEnd,int columnEnd);
    vector<int> getRow(int rowIndex);
    vector<vector<int>>getMatrix();
    void writePage();
    void writePage(string matrixName,int rowStart,int columnStart,int rowEnd,int columnEnd);
};