#ifndef FILEPARSER_H_INCLUDED
#define FILEPARSER_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

#define STO   7
#define STO_STRING "STO"

#define LOAD  6
#define LOAD_STRING "LOAD"

#define POP   5
#define POP_STRING "POP"

#define PUSH  4
#define PUSH_STRING "PUSH"

#define MUL   3
#define MUL_STRING "MUL"

#define SUB   2
#define SUB_STRING "SUB"

#define ADD   1
#define ADD_STRING "ADD"

#define END   0
#define END_STRING "END"

#define TRUE  1
#define FALSE 0

#define operatorOperandDelimiter ":"
#define labelValueDelimiter ":"

typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef std::string operationName;
typedef std::string memoryHandle;
typedef std::pair<memoryHandle, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;

class FileParser{
    std::vector <std::string> lines;
    std::vector <std::string> textLines;
    std::vector <std::string> dataLines;
    std::vector <idValue> idValuePairs;
    std::vector <operatorOperand> operatorOperandPairs;
    void getSectionLine(std::string section, std::string sectionEntry);
    void getSectionLines(std::string targetSection, std::string otherSection);
    void stripCommentsAndWhitespace();
    void loadData();
    void loadText();
    void readLines(char const * inpy);

    reg_word toOperator(std::string opString);
public:
    void loadFile(const char * inputFilename);
    std::vector <idValue> getIdValuePairs();
    std::vector <operatorOperand> getOperatorOperandPairs();
    //return idValuePairs;

};
#endif
