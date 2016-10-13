#ifndef ACCMSIM_H_INCLUDED
#define ACCMSIM_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef std::pair<std::string, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;
void fillMemory(std::vector <idValue> dataEntries, std::vector <operatorOperand> textEntries);
void fetchInstruction();
void loadInAccumProgram(char * inputFilename);
void accumRun();
void populateText(std::vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys);
bool handleInstruction();
std::map<std::string, mem_word> populateData(std::vector <idValue> dataPairs);
int main(int argc, char *argv[]);
#endif
