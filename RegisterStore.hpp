
#ifndef CS236360_HW5_REGISTERSTORE_HPP
#define CS236360_HW5_REGISTERSTORE_HPP

#include <string>
#include <queue>
#include <exception>
#include <stdlib.h>
#include <set>


#define STYPE_REG_MAX 7
#define TTYPE_REG_MAX 9

using namespace std;

class RegisterStore {
    set<string> _registers;
    bool ValidateRegister(string registerName);
public:
    RegisterStore();
    string GetRegister();
    void ReturnRegister(string registerName);
    int NumberOfAvailableRegisters();
};

class EmptyStoreException : public exception{
    const char * what () const throw(){
        return "No more registers";
    }
};

class InvalidRegisterNameException : public exception{
    const char * what () const throw(){
        return "Invalid register name";
    }
};

#endif //CS236360_HW5_REGISTERSTORE_HPP
