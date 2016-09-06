#include "memorySim.h"
#include "accumFileParser.h"
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
struct AccumFileParser fileParserStack;

void fetchInstructionStack(){
    mipStorageStack.load(&instructionRegisterStack, programCounterStack);
}

bool handleInstructionStack(){
    bool user_mode = true;
    //cout << endl << "OPCODE value IS: " << instructionRegisterStack  << endl;
    //cout << endl << "OPERAND value IS: " << mipStorageStack.read(mipStorageStack.read(programCounterStack + 1))  << endl;
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
        upperElement = myStack.back();
        myStack.pop_back();
        lowerElement = myStack.back();
        myStack.pop_back();
        upperElement += lowerElement;
        myStack.push_back(upperElement);
        break;
    case MUL:
        upperElement = myStack.back();
        myStack.pop_back();
        lowerElement = myStack.back();
        myStack.pop_back();
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
    vector <idValue> dataEntries = fileParserStack.idValuePairs;
    vector <operatorOperand> textEntries = fileParserStack.operatorOperandPairs;
    mipStorageStack.initializeMips(dataEntries, textEntries);
    programCounterStack = mipStorageStack.textLocation;
}

void stackRun(){
    bool userMode = true;
    while(userMode){
        fetchInstructionStack();
        mipStorageStack.load(&instructionRegisterStack, programCounterStack);
        userMode = handleInstructionStack();
    }
    cout << "This value was left on top of the stack: " << myStack.back() << endl;
}

int main2(int argc, char *argv[]){
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
