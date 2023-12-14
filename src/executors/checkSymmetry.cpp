#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */

//for matrix
bool syntacticParseCHECKSYMMETRYMatrix()
{
    logger.log("syntacticParseCHECKSYMMETRYMatrix");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRYMatrix()
{
    logger.log("semanticParseTRANSPOSEMatrix");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKSYMMETRYMatrix()
{
    logger.log("executeTRANSPOSEMatrix");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    bool result=matrix->checkSymmetry(parsedQuery.printRelationName);
    cout<<"Check Symmetry Done: "<<result;
    return;
}
