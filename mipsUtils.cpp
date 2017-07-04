
#include "SymbolTable.hpp"
#include <iostream>
#include <sstream>
#include "bp.hpp"
#include "RegisterStore.hpp"
#include "mipsUtils.hpp"


#define DEBUG_CB (do{ if (DBG) CodeBuffer::instance().printCodeBuffer();}while(false))

void printVec(vector<int> vec){
	for (vector<int>::iterator it = vec.begin(); it != vec.end(); it++){
		ostringstream t;
		t << "##" << *it;
		CB.emit(t.str());
	}

}

	
int AssGen::emit(string s){
		return CodeBuffer::instance().emit(s);
	}
string AssGen::next(){
		return CodeBuffer::instance().next();
	}
int AssGen::nextInstr(){
		return CB.nextInstr();
	}

void AssGen::backPatch(const std::vector<int>& address_list, const std::string &loc){
		CB.bpatch( address_list,  loc);
	}


	AssGen::AssGen(SymbolTable* nst) :tempIndexCounter(0){
		st = nst;
		_dataLabelCounter = 0;
	}


	int AssGen::emitPrintI(){
		emit("printi:");
		emit("lw $a0, 0($sp)"); // fetching argument on stack
		emit("li $v0, 1");
		emit("syscall");
		return emit("jr $ra");
	}


	int AssGen::emitPrint(){
		emit("print:");
		ostringstream t;
		t << "la $a0, " << STRING_DATA_NAME;
		emit(t.str());
		emit("li $v0, 4");
		emit("syscall");
		return emit("jr $ra");
	}

	void AssGen::emitDataLiteral(STYPE &V) {
		ostringstream t;
		t << STRING_DATA_NAME << _dataLabelCounter++ << ": .asciiz " << V.stringVal;
		CB.emitData(t.str());
	}

	void AssGen::emitLoadNumToReg(STYPE &v1, STYPE &parent){
		ostringstream t;
		if(RegisterStore::Instance().NumberOfAvailableRegisters() == 0)
		{
			cout << "error no more registers" << endl; //TODO:[TIO]<-[Noam] don't forget to insert valid ouput msg
		}
		string freshReg = RegisterStore::Instance().GetRegister();
		t << "li " << freshReg << ", " << v1.numVal;
		emit(t.str());
		v1.regName = parent.regName = freshReg;
	}

	void AssGen::emitLoadIdToReg(STYPE &v1, STYPE &parent){
		ostringstream t;
		if (RegisterStore::Instance().NumberOfAvailableRegisters() == 0)
		{
			cout << "error no more registers" << endl; //TODO:[TIO]<-[Noam] don't forget to insert valid ouput msg
		}
		string freshReg = RegisterStore::Instance().GetRegister();
		int ofst;
		varType vt;
		st->GetVarOfset(v1.varName, vt, ofst);
		string sign = (ofst == 0)?"":"-";
		t << "lw " << freshReg << " ," << sign << INTOFST*ofst << "($fp)";
		emit(t.str());
		v1.regName = parent.regName = freshReg;
	}

	void AssGen::emitPushLocal() {
		ostringstream t;
		t << "addu $sp, $sp, -4";
		emit(t.str());
	}

	void AssGen::emitPushInitializedLocal(STYPE &V) {
		ostringstream t;
		emitPushLocal();
		t << "sw " << V.regName << " ,0($sp)";
		//std::cout << "in emitPushInitializedLocal 1 befor return register : " << V.regName << std::endl;
		//std::cout << "emitPushInitializedLocal V is:" << V.varName << V.binVal << V.numVal << V.boolVal << V.stringVal;

		RegisterStore::Instance().ReturnRegister(V.regName);
		emit(t.str());
	}

	
	void AssGen::emitUpdateLocal(STYPE &v1, STYPE &v2){
		ostringstream t;
		int ofst;
		varType vt;
		st->GetVarOfset(v1.varName, vt, ofst);
		string sign = (ofst == 0)?"":"-";
		t << "sw " << v2.regName << " ," << sign << INTOFST*ofst <<"($fp)";
		emit(t.str());
	}

	void AssGen::emitReturnNonVoid(STYPE &V) {
		ostringstream t;
		t << "move $v0, " << V.regName;
		//std::cout << "emitReturnNonVoidV is:" << V.varName << V.binVal << V.numVal << V.boolVal << V.stringVal;

		RegisterStore::Instance().ReturnRegister(V.regName);
		emit(t.str());
		emitReturn();
	}

	void AssGen::emitReturn(){
		emit("move $sp, $fp");
		emit("jr $ra");
	}


	void AssGen::emitRestoreOnReturn(int numArgs) {
		ostringstream t;
		t << "addu $sp, $sp, " << INTOFST*numArgs; //pop arguments
		emit(t.str());
		emit("lw $ra, 0($sp)"); //restore caller $ra
		emit("addu $sp, $sp, 4");
		emit("lw $fp, 0($sp)"); // restore caller $fp
		emit("move $sp, $fp"); //adjust $sp
	}


	void AssGen::emitPushInitializedLocalFromReg(STYPE &v1, string regName){
		ostringstream t;
		emitPushLocal();
		t << "sw " << regName << " ,0($sp)";
		//std::cout << "in emitPushInitializedLocalfromreg 1 befor return register : " << v1.varName << ", reg is:" << regName << std::endl;
		//std::cout << "emitPushInitializedLocalfromreg V is:" << v1.varName << v1.binVal << v1.numVal << v1.boolVal << v1.stringVal;

		RegisterStore::Instance().ReturnRegister(regName);
		emit(t.str());

	}

	void AssGen::emitLoadBoolToReg(STYPE &v1, STYPE &parent){
		ostringstream t;
		if (RegisterStore::Instance().NumberOfAvailableRegisters() == 0)
		{
			cout << "error no more registers" << endl; //TODO:[TIO]<-[Noam] don't forget to insert valid ouput msg
			return;
		}
		string freshReg = RegisterStore::Instance().GetRegister();
		
		int BoolVal = v1.boolVal ? 1 : 0;
		t << "li " << freshReg << ", " << BoolVal;
		emit(t.str());
		v1.regName = parent.regName = freshReg;
	}


	string AssGen::emitLoadBoolToReg(bool boolVal){
		ostringstream t;
		if (RegisterStore::Instance().NumberOfAvailableRegisters() == 0)
		{
			cout << "error no more registers" << endl; //TODO:[TIO]<-[Noam] don't forget to insert valid ouput msg
			return "" ;
		}
		string freshReg = RegisterStore::Instance().GetRegister();

		int BlVal = boolVal ? 1 : 0;
		t << "li " << freshReg << ", " << BlVal;
		emit(t.str());
		return freshReg;
	}


	string AssGen::getBinOp(binop bo){
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


	void AssGen::emitBin(STYPE &v1, STYPE &v2, STYPE &parent, binop op) {
		string sop = getBinOp(op);
		ostringstream t;
		t << sop << " " << v1.regName << ", " << v1.regName << ", " << v2.regName;
		emit(t.str());

		//std::cout << "in emeit bin befor return register : " << v2.regName << std::endl;
		//cout << "emit bin :" << sop << " " << v1.regName << ", " << v1.regName << ", " << v2.regName;
		RegisterStore::Instance().ReturnRegister(v2.regName);
		parent.regName = v1.regName;
	}

	string AssGen::getRelOpBranch(relop ro){

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
	
	int AssGen::emitRelopEval(STYPE &VV, STYPE &v1, relop op, STYPE &v2) {
		string reg1 = v1.regName;
		string reg2 = v2.regName;
	
		string target = ""; //empty target for later backpatching
		string branchCond = getRelOpBranch(op);
		ostringstream t;
		VV.trueList = CB.makelist(nextInstr());
		VV.falseList = CB.makelist(nextInstr() + 1);
		t << branchCond << " " << reg1 << ", " << reg2 << ", " << target;
		emit(t.str());
		RegisterStore::Instance().ReturnRegister(reg1);
		RegisterStore::Instance().ReturnRegister(reg2);

		return emit(J);
	}

	void AssGen::printAssembly(){
		CodeBuffer::instance().printDataBuffer();
		CodeBuffer::instance().printCodeBuffer();
	}

	void AssGen::bpOr(STYPE &VV, STYPE &v1, STYPE &M, STYPE &v2){
		backPatch(v1.falseList, M.instr);
		VV.trueList = CodeBuffer::merge(v1.trueList, v2.trueList);
		VV.falseList = v2.falseList;

	}
	
	void AssGen::bpAnd(STYPE &VV, STYPE &v1, STYPE &M, STYPE &v2){
		//printVec(v1.trueList);
		backPatch(v1.trueList, M.instr);
		VV.trueList = v2.trueList;
		VV.falseList = CodeBuffer::merge(v1.falseList, v2.falseList);
	}

	void AssGen::bpNot(STYPE &VV, STYPE &v1){
		VV.trueList = v1.falseList;
		VV.falseList = v1.trueList;
	}

	void AssGen::bpParen(STYPE &VV, STYPE &v1){
		VV.trueList = v1.trueList;
		VV.falseList = v1.falseList;
	}

	
	int AssGen::emitTrue(STYPE &VV){
		VV.trueList = CB.makelist(nextInstr());
		return emit(J);
	}

	int AssGen::emitFalse(STYPE &VV){
		VV.falseList = CB.makelist(nextInstr());
		return emit(J);
	}

	
	
	vector<int> AssGen::mergeLists(vector<int> &L1, vector<int> &L2) {
		return CodeBuffer::instance().merge(L1,L2);
	}

	void AssGen::bpIf(STYPE &S, STYPE &B, STYPE &M1, STYPE &S1){
		backPatch(B.trueList, M1.instr);
		backPatch(B.falseList, next());
		S.nextList = CodeBuffer::merge(B.falseList, S1.nextList);
		//cout <<  "bpIf: " << B.regName << ", " << endl;
		RegisterStore::Instance().ReturnRegister(B.regName);

		//emit("int bpIf");
		//printVec(S.nextList);
	}

	
	void AssGen::bpSwitchCase(STYPE &S, STYPE &E, STYPE &N,STYPE &CL){
		backPatch(N.nextList,next());
		while(!CL.valueList.empty()) {
			int value = CL.valueList.top();
			string instr = CL.instrList.top();
			CL.valueList.pop();
			CL.instrList.pop();
			emitSwitchCase(E,value,instr);
		}
		S.nextList = CB.merge(CL.nextList,CB.makelist(nextInstr()));
		emit(J);
	}

	void AssGen::emitSwitchCase(STYPE &E, int value, string instr){
		ostringstream t;
		t << "beq " << E.regName << " ,$" << value << " ," << instr;
		emit(t.str());
	}

	void AssGen::bpIfElse(STYPE &S, STYPE &B, STYPE &M1, STYPE &S1, STYPE &N, STYPE &M2, STYPE &S2){
		
		//emit("bpIfElse B true:");
		//printVec(B.trueList);
		backPatch(B.trueList, M1.instr);
		//emit("bpIfElse B false:");
		//printVec(B.falseList);

		backPatch(B.falseList, M2.instr);
		S.nextList = CodeBuffer::merge(CodeBuffer::merge(S1.nextList, N.nextList), S2.nextList);
		//emit("bpIfElse s nextList:");
		//printVec(S.nextList);
		//RegisterStore::Instance().ReturnRegister(B.regName);
		//emit("int bpIfElse");
		//printVec(S.nextList);
	}

	int AssGen::emitWhile(STYPE &S, STYPE &M1, STYPE &B, STYPE &M2, STYPE &S1){
		backPatch(S1.nextList, M1.instr);
		backPatch(B.trueList, M2.instr);
		S.nextList = B.falseList;
		return emit(J + M1.instr);
	}

	void AssGen::addNextInstr(STYPE &M){
		M.instr = CB.next();
	}


	int AssGen::emitNbp(STYPE &N){
		N.nextList = CB.makelist(nextInstr());
		//emit("### in emitNbp");
		//ostringstream t;
		//t <<"##"<< nextInstr();
		//emit(t.str());
		return emit(J);
	}

	void AssGen::bpStmntList(STYPE &L, STYPE &L1, STYPE &M, STYPE &S){
		
		//emit("### bpStmntList print L1 list:");
		//printVec(L1.nextList);
		
		//printVec(S.nextList);
		backPatch(L1.nextList, M.instr);
		L.nextList = S.nextList;
		//emit("### print L list:");

		//printVec(S.nextList);
		
		

	}

	void AssGen::setEmptyNextList(STYPE &S){
		//emit("### in bpStmnt");
		vector<int> emptyList;
		S.nextList = emptyList;
		

	}

	void AssGen::bpStmnt(STYPE &L, STYPE &S){
		//emit("### in bpStmnt");
		L.nextList = S.nextList;
		backPatch(L.nextList, next());
		//emit("### print L list:");

		//printVec(S.nextList);
		
	}

	void AssGen::emitStoreArguments(int numCallArgs){
		
		int last = st->expList.size();
		if (last == 0){
			return;
		}
		last--;
		for (int i = numCallArgs; i > 0; i--){
			STYPE tmpVar = st->expList[last];

			//std::cout << "tmpVar is:" << tmpVar.varName << "," << tmpVar.binVal << "," << tmpVar.numVal << "," << tmpVar.boolVal;
			emitPushInitializedLocal(tmpVar);
			last--;
		}

	}

	
	void AssGen::emitNewStackFrame() {
		emit("move $fp, $sp"); //load the current sp value to fp
	}

void AssGen::emitCallFuncById(STYPE &C, STYPE &I1, int numCallArgs){
	//save registers
	// *** ignored for now...
	//old frame pointer
	emit("addu $sp, $sp, -4");
	emit("sw $fp, 0($sp)"); //save old fp

	//return address
	emit("addu $sp, $sp, -4");
	emit("sw $ra, 0($sp)"); //store return address
	//Arguments
	if (I1.varName != LIBPRINT)
		//cout << I1.varName << endl;
		emitStoreArguments(numCallArgs); //put all arguments on stack
	emit("move $fp, $sp"); //load the current sp value to fp
	ostringstream t;
	t << "jal " << I1.varName;
	emit(t.str());
	emitRestoreOnReturn(numCallArgs);
}
	void AssGen::emitFuncLable(string funcName){
		
		ostringstream t;
		t << funcName << ":";
		emit(t.str());
	}

	void AssGen::bpExpList(STYPE &E){
		vector<int> newVec;
		string l = next();
		backPatch(E.falseList, l);
		E.falseList = newVec;
		backPatch(E.trueList, l);
		E.falseList = newVec;

	}

	void AssGen::fixBoolAssign(STYPE &S, STYPE &I, STYPE &E){
		vector<int> newVec;
		//int ofst;
		//varType vt;
		//st->GetVarOfset(I.varName, vt, ofst);
		//ostringstream t;
		//emit("##fixBoolAssign truelist:");

		//printVec(E.trueList);
		backPatch(E.trueList, next());
		E.trueList = newVec;
		string newReg = emitLoadBoolToReg(true);
		emitPushInitializedLocalFromReg(I, newReg);
		S.nextList = CB.makelist(nextInstr());
		emit(J);
		//emit("##fixBoolAssign flist:");
		//printVec(E.falseList);
		backPatch(E.falseList, next());
		E.falseList = newVec;
		string newReg2 = emitLoadBoolToReg(false);
		emitPushInitializedLocalFromReg(I, newReg2);
		
		S.nextList = CodeBuffer::merge(S.nextList, CB.makelist(nextInstr()));
		emit(J);
		//emit("##fixBoolAssign s nextList:");
		//printVec(S.nextList);
		backPatch(S.nextList, next());
		S.nextList = newVec;
	}



	void AssGen::fixBoolAssignUpdate(STYPE &S, STYPE &I, STYPE &E){
		vector<int> newVec;
		//int ofst;
		//varType vt;
		//st->GetVarOfset(I.varName, vt, ofst);
		//ostringstream t;
		backPatch(E.trueList, next());
		E.trueList = newVec;
		STYPE ItmpT;
		ItmpT.regName = emitLoadBoolToReg(true);
		emitUpdateLocal(I, ItmpT);
		S.nextList = CB.makelist(nextInstr());
		emit(J);

		backPatch(E.falseList, next());
		E.falseList = newVec;

		STYPE ItmpF;
		ItmpF.regName = emitLoadBoolToReg(false);
		emitUpdateLocal(I, ItmpF);
		S.nextList = CodeBuffer::merge(S.nextList, CB.makelist(nextInstr()));
		emit(J);

		backPatch(S.nextList, next());
		S.nextList = newVec;
	}


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

