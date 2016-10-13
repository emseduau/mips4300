#ifndef ACCMSIM_H_INCLUDED
#define ACCMSIM_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#define REGISTERCOUNT 32
#define STAGECOUNT 5
typedef unsigned char mem_byte;
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef std::pair<std::string, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;

void fillMemory(std::vector <idValue> dataEntries, std::vector <operatorOperand> textEntries, std::vector <jumplocValue> jumplocValuePairs);
void fetchInstruction();
void loadInAccumProgram(char * inputFilename);
void accumRun();
void populateText(std::vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpSet);
std::map<std::string, mem_word> populateJumps(std::vector <jumplocValue> jumplocValuePairs);
bool handleInstruction();
std::map<std::string, mem_word> populateData(std::vector <idValue> dataPairs);
int main(int argc, char *argv[]);
int handleLabel(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys);
#endif
