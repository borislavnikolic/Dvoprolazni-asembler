#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <cmath>
#include "types.h"

using namespace std;

map<string, Symbol*> symbolTable;
vector<Section> sections;
int lineNum;
bool errorDetected;

void reportError(int lineNum, string message) {
	errorDetected = true;
	cout << "Greska na liniji " << lineNum << ": " << message << endl;
}

int8_t getOpCode(string opName) {
	if (opName == "halt")
		return 1;
	else if(opName=="xchg")
		return 2;
	else if(opName=="int")
		 return 3;
	else if(opName=="mov")
		 return 4;
	else if(opName=="add")
			return 5;
    else if(opName=="sub")
			 return 6;
   else if(opName=="mul")
			 return 7;
   else if(opName=="div")
  			 return 8;
   else if(opName=="cmp")
  			 return 9;
   else if(opName=="not")
     	   return 10;
   else if(opName=="and")
     		return 11;
   else if(opName=="or")
  			 return 12;
   else if(opName=="xor")
     	   return 13;
   else if(opName=="test")
     		return 14;
   else if(opName=="shl")
  			 return 15;
   else if(opName=="shr")
     	   return 16;
   else if(opName=="push")
     		return 17;
   else if(opName=="pop")
  			 return 18;
   else if(opName=="jmp")
     	   return 19;
   else if(opName=="jeq")
     		return 20;
   else if(opName=="jne")
  			 return 21;
   else if(opName=="jgt")
     	   return 22;
   else if(opName=="call")
     		return 23;
   else if(opName=="ret")
  			 return 24;
   else if(opName=="iret")
     	   return 25;


	// dodatati po 1 if za sve preostale instrukcije
	return 0;
}

OperandDescription analyzeOperand(string operand, bool isByte) {
	OperandDescription result;
	result.regNum = 0;
	result.immidiate = 0;
	result.symbolName = "";
	result.isRegHigh = 0;
	if (operand[0] == '&') {
		result.adrType = IMM;
		result.symbolName = operand.substr(1);
	}
	else if (operand[0] == '*') {
		result.adrType = MEM;
		result.immidiate = atoi(operand.substr(1).c_str());
	}
	else if (operand[0] == '$') {
		if (isByte)
			result.adrType = REG_IND_BYTE;
		else
			result.adrType = REG_IND_WORD;
		result.regNum = 7;
		result.symbolName = operand.substr(1);
	}
	else if (operand[0] == '[') {
		if (operand.size() >= 4 && operand[1] == 'r' && isdigit(operand[2])) {
			result.adrType = REG_IND;
			result.regNum = operand[2] - '0';
			if (operand[3] == 'h')
				result.isRegHigh = true;
		}
		else
			reportError(lineNum, "Neregularan format operanda " + operand + " !");
	}
	else if (isdigit(operand[0])) {
		result.adrType = IMM;
		result.immidiate = atoi(operand.c_str());
	}
	else if ((operand.size() == 2 || operand.size() == 3) && operand[0] == 'r' && isdigit(operand[1])) {
		result.adrType = REG;
		result.regNum = operand[1] - '0';
		if (operand.size() == 3 && operand[2] == 'h')
			result.isRegHigh = true;
	}
	else if (operand.size() > 2 && operand[0] == 'r' && isdigit(operand[1])) {
		if (isByte)
			result.adrType = REG_IND_BYTE;
		else
			result.adrType = REG_IND_WORD;
		result.regNum = operand[1] - '0';
		if (operand[2] == 'h' || operand[2] == 'l') {

			//operand = operand.substr(4, operand.size() - 5);
			if (operand[2] == 'h'){
				result.isRegHigh = true;

			}
			operand = operand.substr(4, operand.size() - 5);
		}
		else
			operand = operand.substr(3, operand.size() - 4);
		if (isdigit(operand[0]))//if (isdigit(operand[3]))
			result.immidiate = atoi(operand.c_str());
		else
			result.symbolName = operand;
	}
	else {
		result.adrType = MEM;
		result.symbolName = operand;
	}
	return result;
}

string trim(string toTrim) {
	size_t start = toTrim.find_first_not_of(" \t");
	toTrim = toTrim.substr(start);
	size_t end = toTrim.find_first_of(" \t");
	if (end != string::npos)
		toTrim = toTrim.substr(0, end);
	return toTrim;
}

int main(int argc, char* argv[]) {

	char* inputFileName = argv[3];
	ifstream infile(inputFileName);
	string line;
	string currentSection = "UND";
	int locationCounter = 0;
	while (getline(infile, line))
	{
		lineNum++;
		if(line.empty())
			continue;
		char* cline = new char[100];
		cline = const_cast<char*> (line.c_str());
		char* cfirstToken = strtok(cline, " \t");
		if (cfirstToken == NULL)
			continue;
		string firstToken = cfirstToken;
		if(firstToken.at(firstToken.size()-1) == ':') {
	 		firstToken = firstToken.substr(0, firstToken.size()-1);

	 		map<string, Symbol*>::iterator iter = symbolTable.find(firstToken);
	 		Symbol* sym = NULL;
	 		if(iter == symbolTable.end()) {
	 			sym = new Symbol();
	 			sym->name = firstToken;
	 			sym->section = currentSection;
	 			sym->value = locationCounter;
	 			sym->visibility = 'l';
	 			sym->ordinal = symbolTable.size();
	 			symbolTable[firstToken] = sym;
	 		}
	 		else {
	 			sym = iter->second;
	 			if (sym->section == "UND") {
					if (sym->visibility == 'g' && sym->value == 0)
						reportError(lineNum, "Simbol sa imenom " + sym->name + " se uvozi, pa ne sme biti definisan!");
		 			sym->section = currentSection;
		 			sym->value = locationCounter;

	 			}
	 			else {
	 				reportError(lineNum, "Simbol sa imenom " + firstToken + " je vec definisan u tabeli simbola!");
	 			}
	 		}

			cfirstToken = strtok(NULL, " \t");
			if(cfirstToken == NULL)
				continue;
			firstToken = cfirstToken;
		}
		char* ctoken = strtok(NULL, ",");
		vector<string> arguments;
 		while(ctoken != NULL) {
			string token = ctoken;
			arguments.push_back(trim(token));
			ctoken = strtok(NULL, ",");
		}

		if (firstToken == ".global" || firstToken == ".extern") {
			for (unsigned i = 0; i < arguments.size(); ++i) {
				map<string, Symbol*>::iterator iter = symbolTable.find(arguments[i]);

				if (iter == symbolTable.end()) {
					Symbol* sym = new Symbol();
					sym->name = arguments[i];
					sym->section = currentSection;
					if (firstToken == ".global")
						sym->value = -1;
					else
						sym->value = 0;
					sym->visibility = 'g';
					sym->ordinal = symbolTable.size();
					symbolTable[arguments[i]] = sym;
				}
				else {
					iter->second->visibility = 'g';
					if (iter->second->section == "UND") {
						if (firstToken == ".global")
							iter->second->value = -1;
						else
							iter->second->value = 0;
					}
				}
			}
		}
		else if (firstToken == ".equ") {
			map<string, Symbol*>::iterator iter = symbolTable.find(arguments[0]);

			if (iter == symbolTable.end()) {
				Symbol* sym = new Symbol();
				sym->name = arguments[0];
				sym->section = "CON";
				sym->value = atoi(arguments[1].c_str());
				sym->visibility = 'l';
				sym->ordinal = symbolTable.size();
				symbolTable[arguments[0]] = sym;
			}
			else {
				if (iter->second->visibility == 'l' && iter->second->section == "UND") {
					iter->second->section = "CON";
					iter->second->value = atoi(arguments[1].c_str());
				}
				else {
					reportError(lineNum, "Simbol sa imenom " + iter->second->name + " ne moze predstavljati konstantu, ili je vec definisan!");
				}
			}
		}
		else if (firstToken == ".data" || firstToken == ".text" || firstToken == ".bss" || firstToken == ".section") {
			string sectionName;
			if (firstToken == ".section")
				sectionName = arguments[0];
			else
				sectionName = firstToken;

			currentSection = sectionName;
			locationCounter = 0;

			map<string, Symbol*>::iterator iter = symbolTable.find(sectionName);
			Symbol* sym = NULL;
			if (iter == symbolTable.end()) {
				sym = new Symbol();
				sym->name = sectionName;
				sym->section = currentSection;
				sym->value = locationCounter;
				sym->visibility = 'l';
				sym->ordinal = symbolTable.size();
				symbolTable[sectionName] = sym;
			}
			else {
				sym = iter->second;
				if (sym->section == "UND") {
					if (sym->visibility == 'g')
						reportError(lineNum, "Simbol sa imenom " + sym->name + " ne moze biti sekcija jer je globalni!");
					sym->section = currentSection;
					sym->value = locationCounter;
				}
				else {
					reportError(lineNum, "Simbol sa imenom " + sectionName + " je vec definisan u tabeli simbola!");
				}
			}
		}
		else if (firstToken == ".end")
			break;
		else if (firstToken == ".byte" || firstToken == ".word") {
			if (currentSection == "UND") {
				reportError(lineNum, "Direktiva " + firstToken + " se mora nalaziti unutar sekcije!");
				continue;
			}
			locationCounter += arguments.size();
			if (firstToken == ".word")
				locationCounter += arguments.size();
			for (unsigned i = 0; i < arguments.size(); ++i) {
				OperandDescription desc = analyzeOperand(arguments[i], false);
				if (!desc.symbolName.empty()) {
					map<string, Symbol*>::iterator iter = symbolTable.find(desc.symbolName);
					if (iter == symbolTable.end()) {
						Symbol* sym = new Symbol();
						sym->name = desc.symbolName;
						sym->section = "UND";
						sym->value = 0;
						sym->visibility = 'l';
						sym->ordinal = symbolTable.size();
						symbolTable[desc.symbolName] = sym;
					}
				}
			}
		}
		else if (firstToken == ".align" || firstToken == ".skip") {
			if (currentSection == "UND") {
				reportError(lineNum, "Direktiva " + firstToken + " se mora nalaziti unutar sekcije!");
				continue;
			}
			int toSkip;
			if (firstToken == ".skip")
				toSkip = atoi(arguments[0].c_str());
			else {
				int alignMod = pow(2, atoi(arguments[0].c_str()));
				toSkip = locationCounter / alignMod;
				if (alignMod * toSkip < locationCounter)
					toSkip = alignMod * (toSkip + 1) - locationCounter;
			}
			locationCounter += toSkip;
		}
		else {
			if (currentSection == "UND") {
				reportError(lineNum, "Instrukcija " + firstToken + " se mora nalaziti unutar sekcije!");
				continue;
			}
			bool isByte;
			if (firstToken[firstToken.size() - 1] == 'b')
				isByte = true;
			else if (firstToken[firstToken.size() - 1] == 'w')
				isByte = false;
			else if(firstToken != "halt" && firstToken != "ret" && firstToken != "iret"){
				reportError(lineNum, "Instrukcija " + firstToken + " se ne zavrsava sa b ili w!");
				continue;
			}

			if(firstToken != "halt" && firstToken != "ret" && firstToken != "iret"){
			  firstToken = firstToken.substr(0, firstToken.size() - 1);
			}
			if (firstToken == "halt" || firstToken == "ret" || firstToken == "iret") {
				if (!arguments.empty()) {
					reportError(lineNum, "Instrukcija " + firstToken + " ne sme imati argumente!");
					continue;
				}
				locationCounter++;
			}
			else if (firstToken == "int" || firstToken == "not" || firstToken == "push" || firstToken == "pop"
				|| firstToken == "jmp" || firstToken == "jeq" || firstToken == "jne" || firstToken == "jgt" || firstToken == "call") {
				if (arguments.size() != 1) {
					reportError(lineNum, "Instrukcija " + firstToken + " mora imati tacno 1 argument!");
					continue;
				}
				OperandDescription desc = analyzeOperand(arguments[0], isByte);
				if (firstToken == "pop" && desc.adrType == IMM) {
					reportError(lineNum, "Instrukcija " + firstToken + " ne sme koristiti neposredno adresiranje za operand!");
					continue;
				}
				if (!desc.symbolName.empty()) {
					map<string, Symbol*>::iterator iter = symbolTable.find(desc.symbolName);
					if (iter == symbolTable.end()) {
						Symbol* sym = new Symbol();
						sym->name = desc.symbolName;
						sym->section = "UND";
						sym->value = 0;
						sym->visibility = 'l';
						sym->ordinal = symbolTable.size();
						symbolTable[desc.symbolName] = sym;
					}
				}
				locationCounter += 2;
				if (desc.adrType == REG_IND_BYTE)
					locationCounter++;
				else if (desc.adrType == REG_IND_WORD || desc.adrType == MEM)
					locationCounter += 2;
				else if (desc.adrType == IMM) {
					if (isByte)
						locationCounter++;
					else
						locationCounter += 2;
				}
			}
			else if (firstToken == "xchg" || firstToken == "mov" || firstToken == "add" || firstToken == "mul"
				|| firstToken == "div" || firstToken == "cmp" || firstToken == "and" || firstToken == "or" || firstToken == "xor"
				|| firstToken == "test" || firstToken == "shl" || firstToken == "shr" || firstToken == "sub") {
				if (arguments.size() != 2) {
					reportError(lineNum, "Instrukcija " + firstToken + " mora imati tacno 2 argumenta!");
					continue;
				}
				OperandDescription desc0 = analyzeOperand(arguments[0], isByte);
				OperandDescription desc1 = analyzeOperand(arguments[1], isByte);
				if (firstToken == "xchg" && (desc0.adrType == IMM || desc1.adrType == IMM)) {
					reportError(lineNum, "Instrukcija " + firstToken + " ne sme koristiti neposredno adresiranje za operande!");
					continue;
				}
				else if (firstToken != "cmp" && firstToken != "test" && desc0.adrType == IMM) {
					reportError(lineNum, "Instrukcija " + firstToken + " ne sme koristiti neposredno adresiranje za prvi operand!");
					continue;
				}
				if (!desc0.symbolName.empty()) {
					map<string, Symbol*>::iterator iter = symbolTable.find(desc0.symbolName);
					if (iter == symbolTable.end()) {
						Symbol* sym = new Symbol();
						sym->name = desc0.symbolName;
						sym->section = "UND";
						sym->value = 0;
						sym->visibility = 'l';
						sym->ordinal = symbolTable.size();
						symbolTable[desc0.symbolName] = sym;
					}
				}
				if (!desc1.symbolName.empty()) {
					map<string, Symbol*>::iterator iter = symbolTable.find(desc1.symbolName);
					if (iter == symbolTable.end()) {
						Symbol* sym = new Symbol();
						sym->name = desc1.symbolName;
						sym->section = "UND";
						sym->value = 0;
						sym->visibility = 'l';
						sym->ordinal = symbolTable.size();
						symbolTable[desc1.symbolName] = sym;
					}
				}
				locationCounter += 3;
				if (desc0.adrType == REG_IND_BYTE)
					locationCounter++;
				else if (desc0.adrType == REG_IND_WORD || desc0.adrType == MEM)
					locationCounter += 2;
				else if (desc0.adrType == IMM) {
					if (isByte)
						locationCounter++;
					else
						locationCounter += 2;
				}
				if (desc1.adrType == REG_IND_BYTE)
					locationCounter++;
				else if (desc1.adrType == REG_IND_WORD || desc1.adrType == MEM)
					locationCounter += 2;
				else if (desc1.adrType == IMM) {
					if (isByte)
						locationCounter++;
					else
						locationCounter += 2;
				}
			}
			else {
				reportError(lineNum, "Ne postoji instrukcija sa imenom " + firstToken + " !");
			}
		}
	}

	for (map<string, Symbol*>::iterator iter = symbolTable.begin(); iter != symbolTable.end(); iter++) {
		if (iter->second->section == "UND") {
			if (iter->second->visibility == 'l')
				reportError(lineNum, "Simbol sa imenom " + iter->second->name + " je lokalni, a nije definisan!");
			else if (iter->second->value == -1)
				reportError(lineNum, "Simbol sa imenom " + iter->second->name + " se izvozi, a nije definisan!");
		}
	}

	if (errorDetected) {
		infile.close();
		return 0;
	}

	char* outputFileName = argv[2];
	ofstream outfile(outputFileName);

	outfile << "#tabela simbola" << endl;
	outfile << "#";
	outfile << setw(9) << setfill(' ') << "ime";
	outfile << setw(10) << setfill(' ') << "sek";
	outfile << setw(10) << setfill(' ') << "vr.";
	outfile << setw(10) << setfill(' ') << "vid.";
	outfile << setw(10) << setfill(' ') << "r.b.";
	outfile << endl;
	for (map<string, Symbol*>::iterator iter = symbolTable.begin(); iter != symbolTable.end(); iter++) {
		outfile << setw(10) << setfill(' ') << iter->second->name;
		outfile << setw(10) << setfill(' ') << iter->second->section;
		outfile << setw(10) << setfill(' ') << iter->second->value;
		outfile << setw(10) << setfill(' ') << iter->second->visibility;
		outfile << setw(10) << setfill(' ') << iter->second->ordinal;
		outfile << endl;
	}
	outfile << endl;


	//drugi prolaz



	infile.seekg(0);

	while (getline(infile, line))
	{
		if(line.empty())
			continue;
		char* cline = new char[100];
		cline = const_cast<char*> (line.c_str());
		char* cfirstToken = strtok(cline, " \t");
		if (cfirstToken == NULL)
			continue;
		string firstToken = cfirstToken;

		if(firstToken.at(firstToken.size()-1) == ':') {
			cfirstToken = strtok(NULL, " \t");
			if(cfirstToken == NULL)
				continue;
			firstToken = cfirstToken;
		}
		char* ctoken = strtok(NULL, ",");
		vector<string> arguments;
 		while(ctoken != NULL) {
			string token = ctoken;
			arguments.push_back(trim(token));
			ctoken = strtok(NULL, ",");
		}

		if (firstToken == ".data" || firstToken == ".text" || firstToken == ".bss" || firstToken == ".section") {
			string sectionName;
			if (firstToken == ".section")
				sectionName = arguments[0];
			else
				sectionName = firstToken;

			sections.push_back(Section(sectionName));
		}
		else if (firstToken == ".end")
			break;
		else if (firstToken == ".byte" || firstToken == ".word") {
			for (unsigned i = 0; i < arguments.size(); ++i) {
				OperandDescription desc = analyzeOperand(arguments[i], false);
				if (!desc.symbolName.empty()) {
					if (firstToken == ".word") {
						Relocation rel;
						rel.offset = sections.back().byteCode.size();
						rel.type = ABS;
						Symbol* sym = symbolTable[desc.symbolName];
						int16_t code = 0;
						if (sym->visibility == 'l') {
							code += sym->value;
							sym = symbolTable[sym->section];
						}
						rel.value = sym->ordinal;
						sections.back().relocationTable.push_back(rel);
						sections.back().byteCode.push_back(code);
						sections.back().byteCode.push_back(code >> 8);
					}
				}
				else {
					sections.back().byteCode.push_back(desc.immidiate);
					if (firstToken == ".word")
						sections.back().byteCode.push_back(desc.immidiate >> 8);
				}
			}
		}
		else if (firstToken == ".align" || firstToken == ".skip") {
			int toSkip;
			if (firstToken == ".skip")
				toSkip = atoi(arguments[0].c_str());
			else {
				int alignMod = pow(2, atoi(arguments[0].c_str()));
				toSkip = locationCounter / alignMod;
				if (alignMod * toSkip < locationCounter)
					toSkip = alignMod * (toSkip + 1) - locationCounter;
			}
			for (int i = 0; i < toSkip; ++i) {
				sections.back().byteCode.push_back(0);
			}
		}
		else if (firstToken != ".global" && firstToken != ".extern" && firstToken != ".equ") {
			bool isByte;
			if (firstToken[firstToken.size() - 1] == 'b')
				isByte = true;
			else if (firstToken[firstToken.size() - 1] == 'w')
				isByte = false;
			if(firstToken != "halt" && firstToken != "ret" && firstToken != "iret"){
			   firstToken = firstToken.substr(0, firstToken.size() - 1);
			}
			int8_t instrDescr = getOpCode(firstToken);
			instrDescr <<= 1;
			if (!isByte)
				instrDescr |= 1;
			instrDescr <<= 2;

			sections.back().byteCode.push_back(instrDescr);

			if (firstToken == "int" || firstToken == "not" || firstToken == "push" || firstToken == "pop"
				|| firstToken == "jmp" || firstToken == "jeq" || firstToken == "jne" || firstToken == "jgt" || firstToken == "call") {
				OperandDescription desc = analyzeOperand(arguments[0], isByte);
				int8_t op1Descr = desc.adrType;
				op1Descr <<= 4;
				op1Descr |= desc.regNum;
				op1Descr <<= 1;
				if (desc.isRegHigh)
					op1Descr |= 1;

				sections.back().byteCode.push_back(op1Descr);

				if (desc.adrType != REG && desc.adrType != REG_IND) {
					Symbol* sym = symbolTable[desc.symbolName];
					if (!desc.symbolName.empty() && sym->section != "CON") {
						int16_t code = 0;
						if (desc.regNum == 7 && sym->section == sections.back().name) {
							code = sym->value - sections.back().byteCode.size() - 1;
							if (!isByte)
								code--;
						}
						else {
							Relocation rel;
							rel.offset = sections.back().byteCode.size();
							rel.type = ABS;
							if (desc.regNum == 7)
								rel.type = PC_REL;
							if (rel.type == PC_REL) {
								if (!isByte)
									code = -2;
								else
									code = -1;
							}
							if (sym->visibility == 'l') {
								code += sym->value;
								sym = symbolTable[sym->section];
							}
							rel.value = sym->ordinal;
							sections.back().relocationTable.push_back(rel);
						}
						sections.back().byteCode.push_back(code);
						if (!isByte)
							sections.back().byteCode.push_back(code >> 8);
					}
					else {
						int imm = desc.immidiate;
						if (sym != NULL)
							imm = sym->value;
						sections.back().byteCode.push_back(imm);
						if (!isByte)
							sections.back().byteCode.push_back(imm >> 8);
					}
				}
			}
			else if (firstToken == "xchg" || firstToken == "mov" || firstToken == "add" || firstToken == "mul"
				|| firstToken == "div" || firstToken == "cmp" || firstToken == "and" || firstToken == "or" || firstToken == "xor"
				|| firstToken == "test" || firstToken == "shl" || firstToken == "shr" || firstToken == "sub") {
				OperandDescription desc0 = analyzeOperand(arguments[0], isByte);
				OperandDescription desc1 = analyzeOperand(arguments[1], isByte);

				int8_t op1Descr = desc0.adrType;
				op1Descr <<= 4;
				op1Descr |= desc0.regNum;
				op1Descr <<= 1;
				if (desc0.isRegHigh)
					op1Descr |= 1;

				sections.back().byteCode.push_back(op1Descr);

				if (desc0.adrType != REG && desc0.adrType != REG_IND) {
					Symbol* sym = symbolTable[desc0.symbolName];
					if (!desc0.symbolName.empty() && sym->section != "CON") {
						int16_t code = 0;
						if (desc0.regNum == 7 && sym->section == sections.back().name) {
							code = sym->value - sections.back().byteCode.size() - 1;
							if (!isByte)
								code--;
						}
						else {
							Relocation rel;
							rel.offset = sections.back().byteCode.size();
							rel.type = ABS;
							if (desc0.regNum == 7)
								rel.type = PC_REL;
							if (rel.type == PC_REL) {
								if (!isByte)
									code = -2;
								else
									code = -1;
							}
							if (sym->visibility == 'l') {
								code += sym->value;
								sym = symbolTable[sym->section];
							}
							rel.value = sym->ordinal;
							sections.back().relocationTable.push_back(rel);
						}
						sections.back().byteCode.push_back(code);
						if (!isByte)
							sections.back().byteCode.push_back(code >> 8);
					}
					else {
						int imm = desc0.immidiate;
						if (sym != NULL)
							imm = sym->value;
						sections.back().byteCode.push_back(imm);
						if (!isByte)
							sections.back().byteCode.push_back(imm >> 8);
					}
				}

				int8_t op2Descr = desc1.adrType;
				op2Descr <<= 4;
				op2Descr |= desc1.regNum;
				op2Descr <<= 1;
				if (desc1.isRegHigh)
					op2Descr |= 1;

				sections.back().byteCode.push_back(op2Descr);

				if (desc1.adrType != REG && desc1.adrType != REG_IND) {
					Symbol* sym = symbolTable[desc1.symbolName];
					if (!desc1.symbolName.empty() && sym->section != "CON") {
						int16_t code = 0;
						if (desc1.regNum == 7 && sym->section == sections.back().name) {
							code = sym->value - sections.back().byteCode.size() - 1;
							if (!isByte)
								code--;
						}
						else {
							Relocation rel;
							rel.offset = sections.back().byteCode.size();
							rel.type = ABS;
							if (desc1.regNum == 7)
								rel.type = PC_REL;
							if (rel.type == PC_REL) {
								if (!isByte)
									code = -2;
								else
									code = -1;
							}
							if (sym->visibility == 'l') {
								code += sym->value;
								sym = symbolTable[sym->section];
							}
							rel.value = sym->ordinal;
							sections.back().relocationTable.push_back(rel);
						}
						sections.back().byteCode.push_back(code);
						if (!isByte)
							sections.back().byteCode.push_back(code >> 8);
					}
					else {
						int imm = desc1.immidiate;
						if (sym != NULL)
							imm = sym->value;
						sections.back().byteCode.push_back(imm);
						if (!isByte)
							sections.back().byteCode.push_back(imm >> 8);
					}
				}
			}
		}
	}

	for (unsigned i = 0; i < sections.size(); ++i) {
		if (!sections[i].relocationTable.empty()) {
			outfile << "#.rel" << sections[i].name << endl;
			outfile << "#";
			outfile << setw(9) << setfill(' ') << "ofset";
			outfile << setw(10) << setfill(' ') << "tip";
			outfile << setw(10) << setfill(' ') << "vr.";
			outfile << endl;
			for (unsigned j = 0; j < sections[i].relocationTable.size(); ++j) {
				outfile << setw(10) << setfill(' ') << sections[i].relocationTable[j].offset;
				outfile << setw(10) << setfill(' ') << (sections[i].relocationTable[j].type ? "PC_REL" : "ABS");
				outfile << setw(10) << setfill(' ') << sections[i].relocationTable[j].value;
				outfile << endl;
			}
			outfile << endl;
		}
	}

	for (unsigned i = 0; i < sections.size(); ++i) {
		if (!sections[i].byteCode.empty()) {
			outfile << "#" << sections[i].name << "  " << sections[i].byteCode.size() << endl;
			if (sections[i].name == ".bss")
				continue;
			outfile << hex;
			for (unsigned j = 0; j < sections[i].byteCode.size(); ++j) {
				outfile << setw(2) << setfill('0') << (((int)sections[i].byteCode[j]) & 0xFF);
				if ( (j+1) % 16)
					outfile << " ";
				else
					outfile << endl;
			}
			outfile << dec << endl << endl;
		}
	}
    if(!errorDetected)
    	cout<<"program zavrsen bez gresaka"<<endl;
	infile.close();
	outfile.close();
}
