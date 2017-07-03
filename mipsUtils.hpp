#ifndef _MIPS_UTILS_H
#define _MIPS_UTILS_H

#include "SymbolTable.hpp"


#define DEBUG_CB (do{ if (DBG) CodeBuffer::instance().printCodeBuffer();}while(false))
#define INTOFST 4
#define STRING_DATA_NAME "str"

class AssGen{
private:
	int tempIndexCounter;
	SymbolTable* st;
	

	string next();
	int nextInstr();

	void backPatch(const std::vector<int>& address_list, const std::string &loc);

public:
	AssGen(SymbolTable* nst);
	int emit(string s);
	int emitPrintI();
	int emitPrint();
	void emitLoadNumToReg(STYPE &v1, STYPE &parent);
	void emitLoadIdToReg(STYPE &v1, STYPE &parent);
	void emitLoadBoolToReg(STYPE &v1, STYPE &parent);
	string emitLoadBoolToReg( bool boolVal);
	string getBinOp(binop bo);
	void emitBin(STYPE &v1, STYPE &v2, STYPE &parent, binop op);
	string getRelOpBranch(relop ro);
	int emitRelopEval(STYPE &VV, STYPE &v1, relop op, STYPE &v2);
	//int emitBoolEval(STYPE &VV, STYPE &v1, relop op, STYPE &v2);
	void printAssembly();
	void bpOr(STYPE &VV, STYPE &v1, STYPE &M, STYPE &v2);
	void bpAnd(STYPE &VV, STYPE &v1, STYPE &M, STYPE &v2);
	void bpNot(STYPE &VV, STYPE &v1);
	void bpParen(STYPE &VV, STYPE &v1);
	int emitTrue(STYPE &VV);
	int emitFalse(STYPE &VV);
	void bpIf(STYPE &S, STYPE &B, STYPE &M1, STYPE &S1);
	void bpIfElse(STYPE &S, STYPE &B, STYPE &M1, STYPE &S1, STYPE &N, STYPE &M2, STYPE &S2);
	int emitWhile(STYPE &S, STYPE &M1, STYPE &B, STYPE &M2, STYPE &S1);
	void addNextInstr(STYPE &M);
	int emitNbp(STYPE &N);
	void bpStmntList(STYPE &L, STYPE &L1, STYPE &M, STYPE &S);
	void bpStmnt(STYPE &L, STYPE &S);
	void emitStoreArguments(int numCallArgs);
	void emitCallFuncById(STYPE &C, STYPE &I1, int numCallArgs);
	void emitPushLocal();
	void emitPushInitializedLocal(STYPE &V);
	void emitUpdateLocal(STYPE &v1, STYPE &v2);
	void emitUpdateLocalFromReg(STYPE &v1, string regName);
	void emitFuncLable(string funcName);
	void fixBoolAssign(STYPE &S, STYPE &I, STYPE &E);
	void emitReturnNonVoid(STYPE &V);
	void emitRestoreOnReturn();
	void emitReturn();
	vector<int> mergeLists(vector<int> &L1, vector<int> &L2);
	void bpSwitchCase(STYPE &S, STYPE &E, STYPE &N,STYPE &CL);
	void emitSwitchCase(STYPE &E, int value, string instr);
	void emitNewStackFrame();
	void emitDataLiteral(STYPE &V);
};

#endif





/*
add		$t0,$t1,$t2	#  $t0 = $t1 + $t2;   add as signed (2's complement) integers
sub		$t2,$t3,$t4	#  $t2 = $t3 - $t4
addi	$t2,$t3, 5	#  $t2 = $t3 + 5;   "add immediate" (no sub immediate)
addu	$t1,$t6,$t7	#  $t1 = $t6 + $t7;   add as unsigned integers
subu	$t1,$t6,$t7	#  $t1 = $t6 + $t7;   subtract as unsigned integers

mult	$t3,$t4		#  multiply 32-bit quantities in $t3 and $t4, and store 64-bit
#  result in special registers Lo and Hi:  (Hi,Lo) = $t3 * $t4
div		$t5,$t6		#  Lo = $t5 / $t6   (integer quotient)
#  Hi = $t5 mod $t6   (remainder)
mfhi	$t0		#  move quantity in special register Hi to $t0:   $t0 = Hi
mflo	$t1		#  move quantity in special register Lo to $t1:   $t1 = Lo
#  used to get at result of product or quotient

move	$t2,$t3	#  $t2 = $t3

*/

//****** add asign to ID, jump to func, decler func 

/*
void emitTwoAddresses(string id, string id1, string id2, string op) {
	Symbol* s1 = symbolsTable.GetSymbol(id1, yylineno);
	Symbol* s2 = symbolsTable.GetSymbol(id2, yylineno);
	//s2 = castBase(s1, s2);

	int address = symbolsTable.GetSymbol(id, yylineno)->_offset;
	int address1 = s1->_offset;
	int address2 = s2->_offset;


	ostringstream tmpCode;
	tmpCode.str("");
	tmpCode << "s[" << address << "]=s[" << address1 << "]" << op << "s[" << address2 << "]";

	emit(tmpCode.str());
}

void emitAssignBool(yystype* left_statement, yystype* r_id, yystype* r_exp) {
	list<int> emptyList;

	int address = symbolsTable.GetSymbol(r_id->_id, yylineno)->_offset;
	ostringstream tmpCode;

	tmpCode.str("");
	tmpCode << "s[" << address << "]=1";

	bp(r_exp->trueList, next());
	r_exp->trueList = emptyList;

	emit(tmpCode.str());
	left_statement->nextList = makelist(next());
	emit("goto ");

	tmpCode.str("");
	tmpCode << "s[" << address << "]=0";

	bp(r_exp->falseList, next());
	r_exp->falseList = emptyList;

	emit(tmpCode.str());
	left_statement->nextList = merge(left_statement->nextList, makelist(next()));
	emit("goto ");

	bp(left_statement->nextList, next());
	left_statement->nextList = emptyList;
}

#endif

*/


/*
add markers at the right places for brach and all that

make sure code is writen when defining a function..


*/

