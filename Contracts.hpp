#ifndef __CONTRACTS_H
#define __CONTRACTS_H

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <list>
#include "output.hpp"

using namespace std;

typedef enum{_VOID, _BOOL, _INT, _BYTE, _STRING, _NO_ARGS} varType;
typedef enum{_FUNC, _IF, _WHILE, _CASE, _NEST} scopeType;
typedef enum{_EQ, _LE, _GE, _LT, _GT, _NEQ} relop;
typedef enum{ _PLUS, _MINUS, _MUL, _DEV } binop;




typedef struct{
	vector<varType> argTypes;
	vector<string> argNames;
} varList;

typedef struct {
	string regName;
    int numVal;
    string stringVal;
    bool boolVal;
    bool isDeclaration;
    string varName;
    varType type;
	relop relVal;
	binop binVal;
	vector<int> trueList;
	vector<int> falseList;
	vector<int> nextList;
	vector<int> exitWhileList;
	vector<int> breakList;
	int quad;
	string instr;
	string alocatedRegister;
}STYPE;


#define YYSTYPE STYPE
#define MAXBYTE 256
#define MAXINTLENGTH 11
#define LIBPRINT "print"
#define LIBPRINTI "printi"
#define MAIN "main"
#define TRUE_VAL 1
#define FALSE_VAL 0
#define J "j"
#define JAL "jal"
#define CB (CodeBuffer::instance())


#endif