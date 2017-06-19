//
// Created by Noam on 6/9/2017.
//

#include "Utils.hpp"

bool isByte(varType a){
    return a == _BYTE;
}

bool isInt(varType a){
    return a == _INT;
}

bool isBoolean(varType a){
    return a == _BOOL;
}

bool isString(varType a){
    return a == _STRING;
}

bool areEqualTypes(varType a,varType b){
    return a == b || ((a == _INT) && (b == _BYTE));
}

bool isNumeric(varType a){
    return isInt(a) || isByte(a);
}

char * toString(int num){
    char * buffer = (char*)malloc(MAXINTLENGTH* sizeof(char));
    sprintf(buffer,"%d",num);
    return buffer;
}
const char* typeToString(varType vt){
	switch (vt)
	{
	case (_BOOL) :	return "BOOL";
		break;		 
	case (_INT) :	return "INT";
		break;		 
	case (_BYTE) :	return "BYTE";
		break;		 
	case (_STRING):	return "STRING";
		break;		 
	case (_VOID) :	return "VOID";
		break;
	default:
		return "";
	}

}
std::vector<const char*> getTypeNames(std::vector<varType> vec){
    std::vector<const char*> ret;
    for(int i=0;i<vec.size();++i)
    {
		if (vec[i] == _NO_ARGS){
			continue;
		}
		ret.push_back(typeToString(vec[i]));
    }
    return ret;
}