#include "RegisterStore.hpp"
#include <iostream>
#include <sstream>


string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}


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
	//std::cout << "in Return register : " << registerName << std::endl;
	//cout << "ReturnRegister :::register name is: " << registerName << endl;
	//cout << "registerName[0]: " << registerName[0] << endl;
	//cout << "registerName[1]: " << registerName[1] << endl;
	if(!ValidateRegister(registerName))
		throw new InvalidRegisterNameException();
	if(registerName != "$v0") // might consider list of special reisters
		_registers.insert(registerName);
}

bool RegisterStore::ValidateRegister(string registerName) {
	
	if (registerName.size() != 3) return false;
	
	int regNum = registerName[2] - '0';
	//cout << "reg num is: " << regNum << endl;

	return registerName[0] == '$' &&
		   (registerName[1] == 's' || registerName[1] == 't' || registerName[1] == 'v') &&
									 regNum <= TTYPE_REG_MAX &&
									 regNum >= 0;
}

int RegisterStore::NumberOfAvailableRegisters() {
	return _registers.size();
}
