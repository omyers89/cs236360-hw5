#include "SymbolTable.hpp"
#include "Utils.hpp"
#include <exception>

using namespace std;


bool GDBG = false;
#define DEBUG(obj) {if (GDBG) {cout << obj << endl;}}

Table::Table(Table* parentTable) : _parentTable(parentTable)
{
	//_vars = new(map<string, VarData>);
	_variables = new(vector<TableEntry>);
}

Table::~Table()
{
	delete(_variables);
	//delete(scopeList);
}

bool Table::get(string varName, VarData& dat){

	if (!contains(varName, dat)){
		return false;
	}
	return true;
}

bool Table::get(string varName, TableEntry& dat){

	if (!contains(varName, dat)){
		return false;
	}
	return true;
}

bool Table::addVar(string name, VarData d, bool isfunc)
{
	if (contains(name))
	{
		return false;
	}
	TableEntry tmpTent;
	tmpTent.name = name;
	tmpTent.offset = d.offset;
	tmpTent.t = d.t;
	tmpTent.isFunc = isfunc;
	_variables->push_back(tmpTent);
	return true;
}



bool Table::contains(string name, TableEntry &vd)
{
	if (_variables->size() == 0){
		return false;
	}
	TableIt it = _variables->begin();
	for (; it != _variables->end(); it++){
		if ((*it).name == name){
			vd.t = (*it).t;
			vd.offset = (*it).offset;
			vd.isFunc = (*it).isFunc;
			return true;
		}
	}
	return false;
}


bool Table::contains(string name, VarData &vd)
{
	if (_variables->size() == 0){
		return false;
	}
	TableIt it = _variables->begin();
	for (; it != _variables->end(); it++){
		if ((*it).name == name){
			vd.t = (*it).t;
			vd.offset = (*it).offset;
			return true;
		}
	}
	return false;
}


bool Table::contains(string name)
{
	if (_variables->size() == 0){
		return false;
	}
	TableIt it = _variables->begin();
	for (; it != _variables->end(); it++){
		if ((*it).name == name){
			return true;
		}
	}
	return false;
}

void Table::printScope(){
	if (_variables->size() == 0){
		return;
	}
	TableIt it = _variables->begin();
	//TODO: make sure function prints brackets;
	for (; it != _variables->end(); it++){
		if (it->isFunc){
			string funStrType = output::makeFunctionType(typeToString(it->t.retType),
				getTypeNames(it->t.args));
			output::printID((it->name).c_str(), it->offset, funStrType.c_str());
		}
		else{
			string varStrType = typeToString(it->t.retType);
			output::printID((it->name).c_str(), it->offset, varStrType.c_str());
		}
	}
}

void Offsets::push()
{
	int curOffset;
	if (_offsetsStack.size() == 0){
		curOffset = 0;
	}
	else { curOffset = _offsetsStack.top(); }
	_offsetsStack.push(curOffset);
	DEBUG("ofset_push-size is: ");
	DEBUG((_offsetsStack.size()));
	DEBUG("ofset_push-top is: ");
	DEBUG((_offsetsStack.top()));
}

bool Offsets::pop(){
	if (_offsetsStack.size() == 0){
		//throw new exception("trying to pop empty stack Offsets");
		return false;
	}
	_offsetsStack.pop();
	return true;
}

int& Offsets::top(){
	if (_offsetsStack.size() == 0){
		DEBUG("size is 0: ");
		throw exception();
	}
	return _offsetsStack.top();
}

void Tables::push(Table* t){
	_tableStack.push_back(t);
}

bool Tables::pop(){
	if (_tableStack.size() == 0){
		//throw new exception("trying to pop empty stack- Tables");
		return false;
	}
	_tableStack.pop_back();
	return true;
}

Table* Tables::top(){
	if (_tableStack.size() == 0){ return NULL; }
	return _tableStack.back();
}

Table* Tables::get(int i){
	if ((unsigned)i >= _tableStack.size()){
		return NULL;
	}
	vector<Table*>::iterator it = _tableStack.begin();
	advance(it, i);
	return *it;
}


bool SymbolTable::EndScope(){
	//cout << "table stack size is:" << _tables._tableStack.size() << endl;
	output::endScope();
	Table* tmpT = _tables.top();
	if (NULL == tmpT) {
		cout << "in EndScope: poping empty stack!" << endl;
		return false;
	}
	tmpT->printScope();
	DEBUG("scope ended: ");
	DEBUG((_offsetes._offsetsStack.size()));
	bool t = _tables.pop();
	bool o = _offsetes.pop();

	return t && o;
}

bool SymbolTable::EndProg(){

	DEBUG("End pro called ended: ");
	DEBUG((_offsetes._offsetsStack.size()));
	EndScope();
	return true;
}




SymbolTableResult SymbolTable::AddFunc(string funcName, varType newRetType){
	IdType idt;
	bool isfunc;
	if (funcName == "print" || funcName == "printi" || GetFunc(funcName, idt, isfunc)){
		return FAIL;
	}
	IdType newIdType;
	newIdType.retType = newRetType;
	vector<varType>::iterator it = formalList.argTypes.begin();
	for (; it != formalList.argTypes.end(); it++){
		newIdType.args.push_back(*it);
	}
	VarData newFuncData;
	newFuncData.t = newIdType;
	newFuncData.offset = 0;

	//Table* newFuncTable = new Table(_tables.top(), _FUNC);
	Table* newFuncVarsTable = new Table(_tables.top());
	vector<string>::iterator namesIt = formalList.argNames.begin();
	vector<varType>::iterator typesIt = formalList.argTypes.begin();
	int noffset = -1;
	for (; namesIt != formalList.argNames.end(); namesIt++, typesIt++){
		IdType nRetType;
		nRetType.retType = *typesIt;
		VarData nvarData;
		nvarData.t = nRetType;
		nvarData.offset = noffset;
		varType tmp;
		if (GetVar(*namesIt, tmp)){
			return FAIL;
		}
		if (!newFuncVarsTable->addVar(*namesIt, nvarData)){
			return FAIL;
		}
		noffset--;
	}
	_tables.top()->addVar(funcName, newFuncData, true);
	_offsetes.top() = 0;
	DEBUG("function added to scope : ");
	DEBUG((_tables._tableStack.size()));
	_tables.push(newFuncVarsTable);
	_offsetes.push();

	DEBUG("new funcScope at : ");
	DEBUG((_tables._tableStack.size()));
	return SUCCESS;
}


SymbolTableResult SymbolTable::AddFunc(string funcName, varType newRetType, string &badVarOut){
	IdType idt;
	bool isfunc;
	if (funcName == "print" || funcName == "printi" || GetFunc(funcName, idt, isfunc)){
		badVarOut = funcName;
		return FAIL;
	}
	IdType newIdType;
	newIdType.retType = newRetType;
	vector<varType>::iterator it = formalList.argTypes.begin();
	for (; it != formalList.argTypes.end(); it++){
		newIdType.args.push_back(*it);
	}
	VarData newFuncData;
	newFuncData.t = newIdType;
	newFuncData.offset = 0;

	//Table* newFuncTable = new Table(_tables.top(), _FUNC);
	Table* newFuncVarsTable = new Table(_tables.top());
	vector<string>::iterator namesIt = formalList.argNames.begin();
	vector<varType>::iterator typesIt = formalList.argTypes.begin();
	int noffset = -1;
	for (; namesIt != formalList.argNames.end(); namesIt++, typesIt++){
		IdType nRetType;
		nRetType.retType = *typesIt;
		VarData nvarData;
		nvarData.t = nRetType;
		nvarData.offset = noffset;
		varType tmp;
		if (GetVar(*namesIt, tmp)){
			badVarOut = *namesIt;
			return FAIL;
		}
		if (!newFuncVarsTable->addVar(*namesIt, nvarData)){
			badVarOut = *namesIt;
			return FAIL;
		}
		noffset--;
	}
	_tables.top()->addVar(funcName, newFuncData, true);
	_offsetes.top() = 0;
	DEBUG("function added to scope : ");
	DEBUG((_tables._tableStack.size()));
	_tables.push(newFuncVarsTable);
	_offsetes.push();

	DEBUG("new funcScope at : ");
	DEBUG((_tables._tableStack.size()));
	return SUCCESS;
}



bool CompareVecs(vector<varType> &callArgs, vector<varType> &expectedArgs){
	vector<varType>::iterator it_c = callArgs.begin();
	vector<varType>::iterator it_e = expectedArgs.begin();
	while (it_c != callArgs.end() && it_e != expectedArgs.end())
	{
		if (! areEqualTypes(*it_e, *it_c)){ return false; }
		it_c++;
		it_e++;
	}
	if (it_c == callArgs.end() && it_e == expectedArgs.end()){
		return true;
	}
	return false;
}

SymbolTableResult SymbolTable::CallFunc(string name, vector<varType> &expectedArgs, varType &ret){

	IdType funkyType;
	bool isfunc;
	bool exist = GetFunc(name, funkyType, isfunc);
	if (exist && isfunc){
		expectedArgs = funkyType.args;
		ret = funkyType.retType;
		if (CompareVecs(expList, expectedArgs)){
			return SUCCESS;
		}
		else
		{
			return PROTOTYPE_MISMATCH;
		}
	}
	return NOT_DEFINED;
}

bool SymbolTable::GetFunc(string name, IdType &funType, bool &isfunc){
	Table* curTable = _tables.top();
	while (curTable != NULL){
		TableEntry vd;
		
		if (curTable->get(name, vd)){
			funType = vd.t;
			isfunc = vd.isFunc;
			return true;
		}
		curTable = curTable->_parentTable;
	}
	return false;

}


bool SymbolTable::OpenScope(){
	Table* nt = new Table(_tables.top());
	_tables.push(nt);
	_offsetes.push();

	DEBUG("opened scope- cur scope is _offsetes: ");
	DEBUG((_offsetes._offsetsStack.size()));
	DEBUG("opened scope- cur scope is  _tables: ");
	DEBUG((_tables._tableStack.size()));
	return true;
}

bool SymbolTable::AddVar(string name, varType t){
	IdType idt;
	bool isfunc;
	if (name == "print" || name == "printi" || GetFunc(name, idt, isfunc)){
		return false;
	}
	VarData vd;
	IdType it;
	it.retType = t;
	vd.t = it;
	vd.offset = _offsetes.top();
	_tables.top()->addVar(name, vd);
	_offsetes.top()++;
	return true;
}

bool SymbolTable::GetVar(string name, varType& outVarType){
	IdType idt;
	bool isfunc;
	bool ex = GetFunc(name, idt, isfunc);
	outVarType = idt.retType;
	return ex;
}

bool SymbolTable::GetVarToAssign(string name, varType& outVarType){
	IdType idt;
	bool isfunc;
	bool ex = GetFunc(name, idt, isfunc);
	outVarType = idt.retType;
	return ex && !isfunc;
}

void SymbolTable::AddToFormalList(string varName, varType type){
	formalList.argNames.push_back(varName);
	formalList.argTypes.push_back(type);
}

void SymbolTable::AddToExpList(varType type){
	expList.insert(expList.begin(), type);
}

void SymbolTable::FlushFormalList(){
	formalList.argNames.clear();
	formalList.argTypes.clear();
}

void SymbolTable::FlushExpList(){
	expList.clear();
}

void SymbolTable::FlushExpList(int n){
	varIt begIt = expList.begin();
	varIt endIt = expList.begin();
	if (expList.size() >= n){
		expList.clear();
		return;
	}
	advance(endIt, n);
	expList.erase(begIt, endIt);
}


int main(){

	return 0;
}