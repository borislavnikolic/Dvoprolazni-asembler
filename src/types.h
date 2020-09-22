#include <string>
#include <vector>
#include <inttypes.h>

using namespace std;

class Symbol {
public:
	string name;
	string section;
	int value;
	char visibility;
	int ordinal;
};

enum AddrType{
	IMM, REG, REG_IND, REG_IND_BYTE, REG_IND_WORD, MEM
};

class OperandDescription {
public:
	string symbolName;
	int immidiate;
	int regNum;
	AddrType adrType;
	bool isRegHigh;
};

enum RelType{
	ABS, PC_REL
};

class Relocation {
public:
	int offset;
	RelType type;
	int value;
};

class Section {
public:
	string name;
	vector<Relocation> relocationTable;
	vector<int8_t> byteCode;
	int size;

	Section(string name) {
		this->name = name;
	}
};


