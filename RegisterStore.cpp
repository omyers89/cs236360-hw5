#include "RegisterStore.hpp"

RegisterStore::RegisterStore() {
	for(int i=0,j=0;i<=TTYPE_REG_MAX,j<=STYPE_REG_MAX;++i,++j)
	{
		string t_reg = "$t";
		string s_reg = "$s";
		_registers.insert(s_reg.append(to_string(i)));
		_registers.insert(t_reg.append(to_string(j)));
	}
}

string RegisterStore::GetRegister() {
	if(_registers.empty())
		throw new EmptyStoreException();
	string returnVal = *_registers.begin();
	_registers.erase(returnVal);
	return returnVal;
}

void RegisterStore::ReturnRegister(string registerName) {
	if(!ValidateRegister(registerName))
		throw new InvalidRegisterNameException();
	_registers.insert(registerName);
}

bool RegisterStore::ValidateRegister(string registerName) {
	int regNum = registerName.back() - '0';
	return registerName.size() == 3 &&
		   registerName.front() == '$' &&
		   registerName[1] == 's' || registerName[1] == 't' &&
									 regNum <= TTYPE_REG_MAX &&
									 regNum >= 0;
}

int RegisterStore::NumberOfAvailableRegisters() {
	return _registers.size();
}
