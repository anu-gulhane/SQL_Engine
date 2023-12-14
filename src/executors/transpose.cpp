#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */

//for matrix
bool syntacticParseTRANSPOSEMatrix()
{
    logger.log("syntacticParseTRANSPOSEMatrix");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSEMatrix()
{
    logger.log("semanticParseTRANSPOSEMatrix");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTRANSPOSEMatrix()
{
    logger.log("executeTRANSPOSEMatrix");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->transposeMatrix(parsedQuery.printRelationName);
    return;
}
