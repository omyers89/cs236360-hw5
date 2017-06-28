#ifndef _MIPS_UTILS_H
#define _MIPS_UTILS_H

#include "SymbolTable.hpp"
#include <iostream>
#include <sstream>
#include "bp.hpp"


#define DEBUG_CB (do{ if (DBG) CodeBuffer::instance().printCodeBuffer();}while(false))


class AssGen{
private:
	int tempIndexCounter;
	SymbolTable* st;
	int emit(string s){
		return CodeBuffer::instance().emit(s);
	}
	string next(){
		return CodeBuffer::instance().next();
	}
	int nextInstr(){
		return CB.nextInstr();
	}

	void backPatch(const std::vector<int>& address_list, const std::string &loc){
		CB.bpatch( address_list,  loc);
	}

public:
	AssGen(SymbolTable* nst) :tempIndexCounter(0){
		st = nst;
	}

	string newTempReg(){
		tempIndexCounter = tempIndexCounter % 10;
		ostringstream t;
		t << "$t" << tempIndexCounter++;
		return t.str();
	}


	bool freeTempReg(int n = 1){
		tempIndexCounter -= n;
		if (tempIndexCounter < 0){
			throw new exception();
		}
	}

	int emitPrintI(string id){ 
		emit("lw $a0, 0($sp)");
		emit("li $v0, 1");
		emit("syscall");
		return emit("jr $ra");
	}


	int emitPrint(string id){ 
		emit("lw $a0, 0($sp)");
		emit("li $v0, 4");
		emit("syscall");
		return emit("jr $ra");
	}


	bool emitLoadSTYPEtoReg(STYPE v1, string &curReg){
		int ofst;
		curReg = newTempReg();
		ostringstream t;
		if (v1.varName == ""){
			t << "li " << curReg << ", " << v1.numVal << "($sp)";
			emit(t.str());
			return true;
		}
		else{
			if (st->GetVar(v1.varName, ofst)){
				t << "lw " << curReg << ", " << ofst << "($sp)";
				emit(t.str());
				return true;
			}
		}
		return false;
	}

	string getBinOp(binop bo){
		switch (bo)
		{
		case _PLUS:
			return "addu";
		case _MINUS:
			return "subu";
		case _MUL:
			return "mulo";
		case _DEV:
			return "divu";

		default:
			return "";
		}
	}

	int emitBin(STYPE &VV, STYPE &v1, STYPE &v2, binop op) {
		string reg1;
		string reg2;
		string resReg = newTempReg();
		string sop = getBinOp(op);
		ostringstream t;
		if (!(emitLoadSTYPEtoReg(v1, reg1) && emitLoadSTYPEtoReg(v2, reg2))){
			return -1;

		}
		t << sop << " " << resReg << ", " << reg1 << ", " /*<< hsould add label with BP*/;
		return emit(t.str());

	
	}

	int emitBin(string reg1, string reg2, string sop) {
		string resReg = newTempReg();
		//string sop = getBinOp(op);
		ostringstream t;
		t << sop << " " << resReg << ", " << reg1 << ", " << reg2;
		return emit(t.str());
	}

	string egtRelOpBranch(relop ro){

		switch (ro)
		{
		case _EQ:
			return "beq";
		case _LE:
			return "ble";
		case _GE:
			return "bge";
		case _LT:
			return "blt";
		case _GT:
			return "bgt";
		case _NEQ:
			return "bne";

		default:
			return "";
		}

	}

	int emitRelopEval(STYPE &VV, STYPE &v1, STYPE &v2, relop op) {
		string reg1;
		string reg2;
		string target = ""; //empty target for later backpatching
		string branchCond = egtRelOpBranch(op);
		ostringstream t;
		if (emitLoadSTYPEtoReg(v1, reg1) && emitLoadSTYPEtoReg(v2, reg2)){ //change to just get new reg.
			VV.trueList = CB.makelist(nextInstr());
			VV.falseList = CB.makelist(nextInstr() + 1);
			t << branchCond << " " << reg1 << ", " << reg2 << ", " << target;
			emit(t.str());
			return emit(J);
		}
		return -1;

	}

	//int emitBoolEval(string reg1, string reg2, string sop) {
	//	string resReg = newTempReg();
	//	//string sop = getBinOp(op);
	//	ostringstream t;
	//	t << sop << " " << resReg << ", " << reg1 << ", " << reg2;
	//	return emit(t.str());
	//}
	//


	void bpOr(STYPE &VV, STYPE &v1, STYPE &M, STYPE &v2){
		backPatch(v1.falseList, M.instr);
		VV.trueList = CodeBuffer::merge(v1.trueList, v2.trueList);
		VV.falseList = v2.falseList;

	}
	
	void bpAnd(STYPE &VV, STYPE &v1, STYPE &M, STYPE &v2){
		backPatch(v1.trueList, M.instr);
		VV.trueList = v2.trueList;
		VV.falseList = CodeBuffer::merge(v1.falseList, v2.falseList);
	}

	void bpNot(STYPE &VV, STYPE &v1){
		VV.trueList = v1.falseList;
		VV.falseList = v1.trueList;
	}

	void bpParen(STYPE &VV, STYPE &v1){
		VV.trueList = v1.trueList;
		VV.falseList = v1.falseList;
	}

	
	int emitTrue(STYPE &VV){
		VV.trueList = CB.makelist(nextInstr());
		return emit(J);
	}

	int emitFalse(STYPE &VV){
		VV.falseList = CB.makelist(nextInstr());
		return emit(J);
	}

	void bpIf(STYPE &S, STYPE &B, STYPE &M1, STYPE &S1){
		backPatch(B.trueList, M1.instr);
		S.nextList = CodeBuffer::merge(B.falseList, S1.nextList);
	}

	void bpIfElse(STYPE &S, STYPE &B, STYPE &M1, STYPE &S1, STYPE &N, STYPE &M2, STYPE &S2){
		backPatch(B.trueList, M1.instr);
		backPatch(B.falseList, M2.instr);
		S.nextList = CodeBuffer::merge(CodeBuffer::merge(S1.nextList, N.nextList), S2.nextList);
	}

	int emitWhile(STYPE &S, STYPE &M1, STYPE &B,  STYPE &M2, STYPE &S1){
		backPatch(S1.nextList, M1.instr);
		backPatch(B.trueList, M2.instr);
		S.nextList = B.falseList;
		return emit(J + M1.instr);
	}

	void addNextInstr(STYPE &M){
		M.instr = CB.next();
	}


	int emitNbp(STYPE &N){
		N.nextList = CB.makelist(nextInstr());
		return emit(J);
	}

	void bpStmntList(STYPE &L, STYPE &L1, STYPE &M, STYPE &S){
		backPatch(L1.nextList, M.instr);
		L.nextList = S.nextList;
	}


	void bpStmnt(STYPE &L, STYPE &S){
		L.nextList = S.nextList;
	}

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

