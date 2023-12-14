#include"global.h"

void executeCommand(){
    if(tokenizedQuery[1]=="MATRIX"){
        switch(parsedQuery.queryType){
            
            case LOAD:executeLOADMatrix(); break;
            case PRINT:executePRINTMatrix(); break;
            case TRANSPOSE :executeTRANSPOSEMatrix();break;
            case EXPORT:executeEXPORTMatrix(); break;
            case RENAME:executeRENAMEMatrix(); break;
            default: cout<<"PARSING ERROR"<<endl;

        }
        
    }
    else{

    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case CHECKSYMMETRY: executeCHECKSYMMETRYMatrix(); break;
        case COMPUTE:executeCOMPUTEMatrix();break;
        case GROUPBY:executeGROUPBY();break;
        case INDEX: executeINDEX(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case ORDERBY:executeORDERBY();break;
        case PRINT: executePRINT(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }
    }

    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}