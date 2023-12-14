#include "tableCatalogue.h"
#include "matrixCatalogue.h"
#pragma once
using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    GROUPBY,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    ORDERBY,
    SOURCE,
    UNDETERMINED,
    CHECKSYMMETRY,
    COMPUTE,
    TRANSPOSE
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};
enum AggregateFunction{
    MAX,
    MIN,
    SUM,
    COUNT,
    AVG
};
enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    string groupbyResultRelation="";
    string groupbyAttribute="";
    string aggregateAttribute="";
    string aggregateFunction="";
    long long int aggregateValue=0;
    string returnFunction="";
    string returnAttribute="";
    string groupbyRelation="";
    BinaryOperator groupbyBinaryOperator = NO_BINOP_CLAUSE;

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    vector<string> sortColumnName = {};
    vector<int>sortOrder={};
    vector<pair<int,int>>sortColumns_order={};
    string sortRelationName = "";

    string sourceFileName = "";
    string renameFromMatrixName="";
    string renameToMatrixName="";

    string orderbyResultRelationName="";
    string orderbyRelationName="";
    string orderbyColumnName="";
    string orderbyStartegy="";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseGROUPBY();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseORDERBY();
bool syntacticParseSOURCE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
bool syntacticParseLOADMatrix();
bool syntacticParsePRINTMatrix();
bool syntacticParseTRANSPOSEMatrix();
bool syntacticParseEXPORTMatrix();
bool syntacticParseRENAMEMatrix();
bool syntacticParseCHECKSYMMETRYMatrix();
bool syntacticParseCOMPUTEMatrix();