#ifndef STACKSIM_H_INCLUDED
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef std::pair<std::string, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;
void fetchInstructionStack();
bool handleInstructionStack();
void loadInStackProgram(char * inputFilename);
void stackRun();
int main(int argc, char *argv[]);
#endif
