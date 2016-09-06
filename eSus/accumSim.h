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
void fetchInstruction();
bool handleInstruction();
void loadInProgram(char * inputFilename);
void accumRun();
int main(int argc, char *argv[]);
#endif
