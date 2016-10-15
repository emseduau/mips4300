#ifndef LOADER_H_INCLUDED
#define LOADER_H_INCLUDED
#include "memorySim.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
typedef int opType;
typedef std::string operationName;
typedef std::string memoryHandle;
typedef std::string textHandle;
typedef std::pair<memoryHandle, mem_word> idValue;
typedef std::pair<textHandle, mem_word> jumplocValue;
typedef std::pair<reg_word, opType> operatorAndType;
typedef std::pair<reg_word, std::string> operatorOperand;
struct Loader
{
    MipsMemory * workingMemory;
    std::map<std::string, mem_word> populateData(std::vector <idValue> dataPairs);
    std::map<std::string, mem_word> populateJumps(std::vector <jumplocValue> jumplocValuePairs);
    void fillOperandValues(mem_word bakaBaka[3], operatorOperand currentStrings, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpsToAddys, mem_addr tempPC);
    void populateText(std::vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpsToAddys);
    int handleJump(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys, mem_addr tempPC);
    Loader(MipsMemory * inputMemory, std::vector <idValue> dataEntries,std::vector <operatorOperand> textEntries,std::vector <jumplocValue> jumplocValuePairs);
    int handleLabel(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys);
    int handleOffset(std::string toWork);
    int handleImmediate(std::string toWork, int which, int numOperands);
    int handleReg(std::string toWork, int which);
    size_t find_nth(std::string haystack, size_t pos, const std::string& needle, size_t nth);
};
#endif // LOADER_H_INCLUDED
