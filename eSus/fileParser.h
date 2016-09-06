#ifndef FILEPARSER_H_INCLUDED
#define FILEPARSER_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#define STO   7
#define LOAD  6
#define POP   5
#define PUSH  4
#define MUL   3
#define SUB   2
#define ADD   1
#define END   0
#define TRUE  1
#define FALSE 0
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef std::pair<std::string, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;
struct FileParser{
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
    void loadFile(const char * inputFilename);
    reg_word toOperator(std::string opString);
};
#endif
