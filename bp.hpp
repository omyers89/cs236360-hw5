#ifndef EX5_CODE_GEN
#define EX5_CODE_GEN

#include <vector>
#include <string>

class CodeBuffer{
	CodeBuffer();
	CodeBuffer(CodeBuffer const&);
    void operator=(CodeBuffer const&);
	std::vector<std::string> buffer;
	std::vector<std::string> dataDefs;
public:
	static CodeBuffer &instance();

	// ******** Methods to handle the code section ********** //

	//get a jump location for the next command
	std::string next();

	//write command to the buffer, returns its location in the buffer
	int emit(const std::string &command);

/*
    l is a list of commands in the buffer that need to be backpatched.
    loc is the label to jump to.
    e.g.
    
    before bp
    
    .
    .
    100: j 
    .
    .
    
    after bp([100], "l500")
    
    .
    .
    100: j l500
    .
    .
*/
	void bpatch(const std::vector<int>& address_list, const std::string &loc);


	//print the content of the code buffer to stdout including a .text header
	void printCodeBuffer();


	static std::vector<int> makelist(int litem);

	static std::vector<int> merge(const std::vector<int> &l1,const std::vector<int> &l2);

	// ******** Methods to handle the data section ********** //
	//write a line to the data section
	void emitData(const std::string& dataLine);
	//print the content of the data buffer to stdout including a .data header
	void printDataBuffer();

};

#endif

