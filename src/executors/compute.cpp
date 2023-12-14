#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */

//for matrix
bool syntacticParseCOMPUTEMatrix()
{
    logger.log("syntacticParseCOMPUTEMatrix");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTEMatrix()
{
    logger.log("semanticParseCOMPUTEMatrix");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTEMatrix()
{
    logger.log("executeTRANSPOSEMatrix");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->compute(parsedQuery.printRelationName);
    //cout<<"compute Done: ";
    //<<result;
    return;
}
