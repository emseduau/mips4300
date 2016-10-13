#ifndef STACKSIM_H_INCLUDED
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef std::pair<std::string, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;
void fetchInstructionStack();
bool handleInstructionStack();
void fillMemory(std::vector <idValue> dataEntries, std::vector <operatorOperand> textEntries);
std::map<std::string, mem_word> populateData(std::vector <idValue> dataPairs);
void populateText(std::vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys);
void loadInStackProgram(char * inputFilename);
void stackRun();
int main(int argc, char *argv[]);
#endif
