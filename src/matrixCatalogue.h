#include "matrix.h"
#pragma once
/**
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue. 
 *
 */
class MatrixCatalogue
{

    

public:
unordered_map<string, Matrix*> matrices;
    MatrixCatalogue() {}
    void insertMatrix(Matrix* matrix);
    void deleteMatrix(string matrixName);
    Matrix* getMatrix(string matrixName);
    bool isMatrix(string matrixName);
    // bool isColumnFromTable(string columnName, string tableName);
    void print();
    ~MatrixCatalogue();
};