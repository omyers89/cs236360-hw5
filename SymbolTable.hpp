#ifndef _TABLES_H_
#define _TABLES_H_


#include <map>
#include <list>
#include <stack>
#include <iostream>
#include "Contracts.hpp"

using namespace std;

typedef enum{ SUCCESS,FAIL, FAIL_ALREADY_DEFINED, NOT_DEFINED, PROTOTYPE_MISMATCH} SymbolTableResult;


struct IdType{
	varType retType;
	vector<varType> args;
};


typedef vector<varType>::iterator varIt;
typedef vector<string>::iterator nameIt;

typedef struct {
	IdType t;
	int offset;
} VarData;

typedef struct {
	string name;
	IdType t;
	int offset;
	bool isFunc;
} TableEntry;

typedef vector<TableEntry>::iterator TableIt;

class Table {
private:
	//map<string, VarData>* _vars;
	vector<TableEntry>* _variables;

	//list<scopeType>* scopeList;
public: 
	Table* _parentTable;
	Table(Table* parentTable);
	bool get(string varName, VarData& dat);
	bool get(string varName, TableEntry& dat);
	bool addVar(string name, VarData d, bool isfunc = false);
	bool contains(string name);
	bool contains(string name, TableEntry &vd);
	bool contains(string name, VarData &vd);
	void printScope();
	~Table();
};

class Offsets{
private:
	
public:
	stack<int> _offsetsStack;
	void push();
	bool pop();
	int& top();

};

class Tables{
private:
	
public:
	vector<Table*> _tableStack;
	void push(Table* t);
	bool pop();
	Table* top();
	Table* get(int i);
};


class SymbolTable{
private:
	Tables _tables;
	Offsets _offsetes;
	bool GetFunc(string name, IdType &funType, bool &isfunc);
	bool GetSymbolOfst(string name, IdType &funType, bool &isfunc, int &ofst);


public:
	//bool findVarByName(string name);
	bool IsMainDefined;
	int WhileCount;
	bool IsWhileOrIfIntact;
	bool IsWhileOrIfScopeOpened;
	varType LastExpTypeIs;
	varList formalList;
	vector<varType> expList;
	varType ReturnType;
	SymbolTable() :IsMainDefined(false),
								WhileCount(0),
								IsWhileOrIfIntact(false),
								IsWhileOrIfScopeOpened(false),
								LastExpTypeIs(_NO_ARGS),
								ReturnType(_VOID){}
	bool EndScope(); //just pop tables and offsets
	bool EndProg(); //just pop tables and offsets
	//SymbolTableResult AddFunc(string name, varType retType, varList &argNameTypes);
	SymbolTableResult AddFunc(string name, varType retType);
	SymbolTableResult AddFunc(string name, varType retType, string & badVarOut);
	//SymbolTableResult CallFunc(string name, vector<varType> &callArgs, vector<varType> &expectedArgs, varType &ret);
	SymbolTableResult CallFunc(string name, vector<varType> &expectedArgs, varType &ret);

	bool OpenScope();//make new table, add to tables and update offsets
	bool AddVar(string name, varType t); //insert at top table (name, tyoe, offset), and update offset
	bool GetVar(string name, varType& outVarType); //return a reference to the object, or null and false otherwise
	bool GetVarOfset(string name, varType& outVarData, int &outOfst); //return a reference to the object, or null and false otherwise
	bool GetVarToAssign(string name, varType& outVarType);
	void AddToFormalList(string varName, varType type);
	void AddToExpList(varType type);
	void FlushFormalList();

	void FlushExpList();
	void FlushExpList(int n);


};

#endif //_TABLES_H_
