#ifndef __CONTRACTS_H
#define __CONTRACTS_H

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "output.hpp"

using namespace std;

typedef enum{_VOID, _BOOL, _INT, _BYTE, _STRING, _NO_ARGS} varType;
typedef enum{_FUNC, _IF, _WHILE, _CASE, _NEST} scopeType;
typedef enum{_EQ, _LE, _GE, _LT, _GT, _NEQ} relop;



typedef struct{
	vector<varType> argTypes;
	vector<string> argNames;
} varList;

typedef struct {
    int numVal;
    string stringVal;
    bool boolVal;
    bool isDeclaration;
    string varName;
    varType type;
}STYPE;


#define YYSTYPE STYPE
#define MAXBYTE 256
#define MAXINTLENGTH 11
#define LIBPRINT "print"
#define LIBPRINTI "printi"
#define MAIN "main"

#endif