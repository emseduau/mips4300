#ifndef FILEPARSER_H_INCLUDED
#define FILEPARSER_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include "mipsDataTypes.h"
#include "opCodes.h"
#define TRUE  1
#define FALSE 0

#define operatorOperandDelimiter ":"
#define labelValueDelimiter ":"
#define textLabelDelimiter ";"
typedef int opType;
typedef std::string operationName;
typedef std::string memoryHandle;
typedef std::string textHandle;
typedef std::pair<memoryHandle, mem_word> idValue;
typedef std::pair<textHandle, mem_word> jumplocValue;
typedef std::pair<reg_word, opType> operatorAndType;
typedef std::pair<reg_word, std::string> operatorOperand;

class FileParser{
    static const std::map<reg_word, opType> operatorTypeTable;
    static const std::map<operationName, operatorAndType> operatorValueTable;
    static  std::string  const comment;
    static const std::string dataLabel;
    static const std::string textLabel;
    std::vector <std::string> lines;
    std::vector <std::string> textLines;
    std::vector <std::string> dataLines;
    std::vector <idValue> idValuePairs;
    std::vector <operatorOperand> operatorOperandPairs;
    std::vector <jumplocValue> jumplocValuePairs;
    void getSectionLine(std::string section, std::string sectionEntry);
    void getSectionLines(std::string targetSection, std::string otherSection);
    void stripCommentsAndWhitespace();
    void loadData();
    void loadText();
    void readLines(char const * inpy);
    reg_word toOperator(std::string opString);
    int numOperands(operationName currentOperation);
public:
    void loadFile(const char * inputFilename);
    std::vector <idValue> getIdValuePairs();
    std::vector <operatorOperand> getOperatorOperandPairs();
    std::vector <jumplocValue> getJumpPairs();
    opType getType(reg_word currentOperation);
    //return idValuePairs;

};
#endif
