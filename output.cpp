#include <iostream>
#include "output.hpp"
#include <sstream>

using namespace std;

void output::endScope(){
    cout << "---end scope---" << endl;
}

void output::printID(const char* id, int offset, const char* type  ){
    cout << id << " " << type <<  " " << offset <<  endl;
}

string typeListToString(const std::vector<const char*>& argTypes) {
	stringstream res;
	res << "(";
	for(int i = 0; i < argTypes.size(); ++i) {
		res << argTypes[i];
		if (i + 1 < argTypes.size())
			res << ",";
	}
	res << ")";
	return res.str();
}

string output::makeFunctionType(const char* retType, std::vector<const char*> argTypes)
{
	stringstream res;
	res << typeListToString(argTypes) << "->" << retType;
	return res.str();
}


void output::errorLex(int lineno){
    cout << "line " << lineno << ":" << " lexical error" << endl;
}

void output::errorSyn(int lineno){
    cout << "line " << lineno << ":" << " syntax error" << endl;
}

void output::errorUndef(int lineno, const char* id){
    cout << "line " << lineno << ":" << " variable " << id << " is not defined" << endl;
}

void output::errorDef(int lineno, const char* id){
    cout << "line " << lineno << ":" << " identifier " << id << " is already defined" << endl;
}

void output::errorUndefFunc(int lineno, const char* id) {
    cout << "line " << lineno << ":" << " function " << id << " is not defined" << endl;
}

void output::errorMismatch(int lineno){
    cout << "line " << lineno << ":" << " type mismatch" << endl;
}

void output::errorPrototypeMismatch(int lineno, const char* id, std::vector<const char*> argTypes) {
    cout << "line " << lineno << ": prototype mismatch, function " << id << " expects arguments " << typeListToString(argTypes) << endl;
}
	
void output::errorUnexpectedBreak(int lineno) {
	cout << "line " << lineno << ":" << " unexpected break statement" << endl;	
}

void output::errorMainMissing() {
	cout << "Program has no 'void main()' function" << endl;
}

void output::errorByteTooLarge(int lineno, const char* value) {
	cout << "line " << lineno << ": byte value " << value << " out of range"<< endl;
} 

void output::errorNotAllPaths(int lineno, const char* id) {
	cout << "line " << lineno << ":" << " not all paths return a value (" << id << ")" << endl;
}

void output::errorDeadCode(int lineno) {
	cout << "line " << lineno << ":" << " dead code" << endl;
}
