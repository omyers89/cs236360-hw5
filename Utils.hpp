//
// Created by Noam on 6/9/2017.
//

#ifndef CS236360_HW2_UTILS_H
#define CS236360_HW2_UTILS_H
#include "Contracts.hpp"
#include <iostream>

bool areEqualTypes(varType a,varType b);
bool isBoolean(varType a);
bool isNumeric(varType a);
bool isString(varType a);
bool isInt(varType a);
bool isByte(varType a);
bool validateFunc(std::vector<varType,string> argV,std::vector<varType> expV);
char* toString(int num);
std::vector<const char*> getTypeNames(std::vector<varType> vec);
const char*  typeToString(varType);

#endif //CS236360_HW2_UTILS_H
