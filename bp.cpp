#include "bp.hpp"
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

CodeBuffer::CodeBuffer() : buffer(), dataDefs() {
}

CodeBuffer &CodeBuffer::instance() {
	static CodeBuffer inst;//only instance
	return inst;
}

string CodeBuffer::next(){
	std::stringstream label;
	label << "label_";
	label << buffer.size();
	std::string ret(label.str());
	label << ":";
	emit(label.str());
	return ret;
}

int CodeBuffer::emit(const string &s){
    buffer.push_back(s);
	return buffer.size() - 1;
}

void CodeBuffer::bpatch(const vector<int>& l, const std::string &label){
    for(vector<int>::const_iterator i = l.begin(); i != l.end(); i++){
		buffer[*i] += label;
    }
}

void CodeBuffer::printCodeBuffer(){
	std::cout << ".text" << std::endl;
	for (std::vector<string>::const_iterator it = buffer.begin(); it != buffer.end(); ++it) 
	{
		cout << *it << endl;
    }
}

vector<int> CodeBuffer::makelist(int litem)
{
	vector<int> newList;
	newList.push_back(litem);
	return newList;
}

vector<int> CodeBuffer::merge(const vector<int> &l1,const vector<int> &l2)
{
	vector<int> newList(l1.begin(),l1.end());
	newList.insert(newList.end(),l2.begin(),l2.end());
	return newList;
}

// ******** Methods to handle the data section ********** //
void CodeBuffer::emitData(const std::string& dataLine) 
{
	dataDefs.push_back(dataLine);
}

void CodeBuffer::printDataBuffer()
{
	std::cout << ".data" << std::endl;
	for (std::vector<string>::const_iterator it = dataDefs.begin(); it != dataDefs.end(); ++it)
	{
		cout << *it << endl;
	}
}

