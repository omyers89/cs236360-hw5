#include "RegisterStore.hpp"

RegisterStore::RegisterStore() {
	for(int i = 0; i <= TTYPE_REG_MAX; ++i)
	{
		ostringstream s;
		s << "$t" << i;
		_registers.insert(s.str());
	}
	for (int i = 0; i <= STYPE_REG_MAX; ++i)
	{
		ostringstream s;
		s << "$s" << i;
		_registers.insert(s.str());
	}
}

RegisterStore &RegisterStore::Instance() {
	static RegisterStore instance;
	return instance;
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
	int regNum = registerName[2] - '0';
	return registerName.size() == 3 &&
		   registerName[0] == '$' &&
		   registerName[1] == 's' || registerName[1] == 't' &&
									 regNum <= TTYPE_REG_MAX &&
									 regNum >= 0;
}

int RegisterStore::NumberOfAvailableRegisters() {
	return _registers.size();
}
