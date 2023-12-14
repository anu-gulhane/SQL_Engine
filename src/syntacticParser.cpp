#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if(tokenizedQuery[1]=="MATRIX"){
        if (possibleQueryType == "LOAD")return syntacticParseLOADMatrix();
        else if (possibleQueryType == "PRINT")return syntacticParsePRINTMatrix();
        else if (possibleQueryType == "TRANSPOSE")return syntacticParseTRANSPOSEMatrix();
        else if (possibleQueryType == "EXPORT")return syntacticParseEXPORTMatrix();
        // else if (possibleQueryType == "CHECKSYMMETRY")return syntacticParseCHECKSYMMETRYMatrix();
        // else if (possibleQueryType == "COMPUTE")return syntacticParseCOMPUTEMatrix();
        else if (possibleQueryType == "RENAME")return syntacticParseRENAMEMatrix();
        else {
            cout << "SYNTAX ERROR" << endl;
                return false;
        }
    }
    else{
        if (possibleQueryType == "CLEAR")
            return syntacticParseCLEAR();
        else if (possibleQueryType == "CHECKSYMMETRY")return syntacticParseCHECKSYMMETRYMatrix();
        else if (possibleQueryType == "COMPUTE")return syntacticParseCOMPUTEMatrix();
        else if (possibleQueryType == "INDEX")
            return syntacticParseINDEX();
        else if (possibleQueryType == "LIST")
            return syntacticParseLIST();
        else if (possibleQueryType == "LOAD")
            return syntacticParseLOAD();
        else if (possibleQueryType == "PRINT")
            return syntacticParsePRINT();
        else if (possibleQueryType == "RENAME")
            return syntacticParseRENAME();
        else if(possibleQueryType == "EXPORT")
            return syntacticParseEXPORT();
        else if(possibleQueryType == "SOURCE")
            return syntacticParseSOURCE();
         else if (possibleQueryType == "SORT")
                return syntacticParseSORT();
        else
        {
            string resultantRelationName = possibleQueryType;
            if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
            {
                cout << "SYNTAX ERROR" << endl;
                return false;
            }
            possibleQueryType = tokenizedQuery[2];
            if (possibleQueryType == "PROJECT")
                return syntacticParsePROJECTION();
            else if (possibleQueryType == "SELECT")
                return syntacticParseSELECTION();
            else if (possibleQueryType == "JOIN")
                return syntacticParseJOIN();
            else if (possibleQueryType == "CROSS")
                return syntacticParseCROSS();
            else if (possibleQueryType == "DISTINCT")
                return syntacticParseDISTINCT();
            else if(possibleQueryType == "ORDER")
            return syntacticParseORDERBY();
            else if(possibleQueryType == "GROUP")
            return syntacticParseGROUPBY();
           
            else
            {
                cout << "SYNTAX ERROR" << endl;
                return false;
            }
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->groupbyBinaryOperator=NO_BINOP_CLAUSE;
    this->groupbyAttribute="";
    this->groupbyRelation="";
    this->aggregateAttribute="";
    this->aggregateFunction="";
    this->aggregateValue=0;
    this->returnAttribute="";
    this->returnFunction="";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    this->sortOrder={};
    this->sortColumns_order={};
    this->sortColumnName = {};
    this->sortRelationName = "";

    this->sourceFileName = "";

    this->orderbyResultRelationName="";
    this->orderbyColumnName="";
    this->orderbyRelationName="";
    this->orderbyStartegy="";
    logger.log("clear");
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}
