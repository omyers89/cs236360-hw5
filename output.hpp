#ifndef _236360_4_
#define _236360_4_

#include <vector>
#include <string>

namespace output{
    void endScope();
    void printID(const char* id, int offset, const char* type);
    std::string makeFunctionType(const char* retType, std::vector<const char*> argTypes);
    void errorLex(int lineno);
    void errorSyn(int lineno);
    void errorUndef(int lineno, const char* id);
    void errorDef(int lineno, const char* id);
    void errorUndefFunc(int lineno, const char* id);
    void errorMismatch(int lineno);
    void errorPrototypeMismatch(int lineno, const char* id, std::vector<const char*> argTypes);
    void errorUnexpectedBreak(int lineno);
    void errorMainMissing();
    void errorByteTooLarge(int lineno, const char* value);
    //bonus errors
    void errorNotAllPaths(int lineno, const char* id);
    void errorDeadCode(int lineno);
}

#endif
