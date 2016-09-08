#include "memorySim.h"
#include "fileParser.h"
#include "stackSim.h"
using namespace std;
std::vector<reg_word> myStack;
reg_word toWrite;
reg_word upperElement;
reg_word lowerElement;
reg_word accumulatorStack = 0;
reg_word instructionRegisterStack = 0;
mem_addr programCounterStack;
struct MipsMemory mipStorageStack;
struct FileParser fileParserStack;

void fetchInstructionStack(){
    mipStorageStack.load(&instructionRegisterStack, programCounterStack);
}

void getStackOperands(){
    upperElement = myStack.back();
    myStack.pop_back();
    lowerElement = myStack.back();
    myStack.pop_back();
}

bool handleInstructionStack(){
    bool user_mode = true;
    reg_word opCode = instructionRegisterStack;
    reg_word operandA = mipStorageStack.read(programCounterStack + 1);
    switch(opCode)
    {
    case PUSH:
        myStack.push_back(mipStorageStack.read(operandA));
        break;
    case POP:
        toWrite = myStack.back();
        myStack.pop_back();
        mipStorageStack.write(operandA, toWrite);
        break;
    case ADD:
        getStackOperands();
        upperElement += lowerElement;
        myStack.push_back(upperElement);
        break;
    case MUL:
        getStackOperands();
        upperElement *= lowerElement;
        myStack.push_back(upperElement);
        break;
    case END:
        user_mode = false;
        break;
    default:
        break;
    }
    programCounterStack += 2;
    return user_mode;
}

void loadInStackProgram(char * inputFilename){
    char const * harg = inputFilename;
    fileParserStack.loadFile(harg);
    vector <idValue> dataEntries = fileParserStack.getIdValuePairs(); //idValuePairs();
    vector <operatorOperand> textEntries = fileParserStack.getOperatorOperandPairs();
    mipStorageStack.initializeMips(dataEntries.size(), textEntries.size() * 2);
    fillMemory(dataEntries, textEntries);
    programCounterStack = mipStorageStack.getTextLocation();
}

void fillMemory(std::vector <idValue> dataEntries, std::vector <operatorOperand> textEntries){
    std::map<std::string, mem_word> relationSet = populateData(dataEntries);
    populateText(textEntries, relationSet);
}

std::map<std::string, mem_word> populateData(vector <idValue> dataPairs){
    mem_word dataSize = dataPairs.size();
    std::map<std::string, mem_word> relationSet;
    for(int i = 0; i < dataSize; i++){
        idValue extractedRelation = dataPairs.at(i);
        mem_addr dataLocation = mipStorageStack.dataNewAddress();
        mipStorageStack.write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataLocation;
    }
    return relationSet;
}

void populateText(vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys){
    for(int i = 0, pairSize = inPairs.size(); i < pairSize; i++){
        operatorOperand currentStrings = inPairs.at(i);
        mem_word opcode = currentStrings.first;
        mem_word operandValue = varsToAddys[currentStrings.second];
        mipStorageStack.write(mipStorageStack.textNewAddress(), opcode);
        mipStorageStack.write(mipStorageStack.textNewAddress(), operandValue);
    }
}

void stackRun(){
    bool userMode = true;
    while(userMode){
        fetchInstructionStack();
        userMode = handleInstructionStack();
    }
    cout << "This value was left on top of the stack: " << myStack.back() << endl;
}

int main(int argc, char *argv[]){
    if(argc != 2)
    {
        cout << "Improper usage: Please enter a single filename." << endl;
        return 1;
    }
    else{
    loadInStackProgram(argv[1]);
    stackRun();
    mipStorageStack.tearDownMips();
    return 0;
    }
}
