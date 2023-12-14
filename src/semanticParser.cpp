#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    if(tokenizedQuery[1]=="MATRIX"){
        switch(parsedQuery.queryType){
            case LOAD: return semanticParseLOADMatrix();
            case PRINT: return semanticParsePRINTMatrix();
            case RENAME: return semanticParseRENAMEMatrix();
            case EXPORT: return semanticParseEXPORTMatrix();
            case TRANSPOSE: return semanticParseTRANSPOSEMatrix();
        }
    }
    else{
        switch(parsedQuery.queryType){
            case CHECKSYMMETRY: return semanticParseCHECKSYMMETRYMatrix();
            case CLEAR: return semanticParseCLEAR();
            case COMPUTE :return semanticParseCOMPUTEMatrix();
            case CROSS: return semanticParseCROSS();
            case DISTINCT: return semanticParseDISTINCT();
            case EXPORT: return semanticParseEXPORT();
            case GROUPBY: return semanticParseGROUPBY();
            case INDEX: return semanticParseINDEX();
            case JOIN: return semanticParseJOIN();
            case LIST: return semanticParseLIST();
            case LOAD: return semanticParseLOAD();
            case ORDERBY:return semanticParseORDERBY();
            case PRINT: return semanticParsePRINT();
            case PROJECTION: return semanticParsePROJECTION();
            case RENAME: return semanticParseRENAME();
            case SELECTION: return semanticParseSELECTION();
            case SORT: return semanticParseSORT();
            case SOURCE: return semanticParseSOURCE();
            default: cout<<"SEMANTIC ERROR"<<endl;
        }
    }
    return false;
}