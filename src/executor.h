#include"semanticParser.h"
#pragma once
void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeGROUPBY();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executeORDERBY();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executeLOADMatrix();
void executePRINTMatrix();
void executeTRANSPOSEMatrix();
void executeEXPORTMatrix();
        // else if (possibleQueryType == "CHECKSYMMETRY")return syntacticParseCHECKSYMMETRYMatrix();
        // else if (possibleQueryType == "COMPUTE")return syntacticParseCOMPUTEMatrix();
void executeRENAMEMatrix();
void executeCHECKSYMMETRYMatrix();
void executeCOMPUTEMatrix();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);