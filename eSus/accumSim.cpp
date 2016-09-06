#include "memorySim.h"
#include "accumFileParser.h"
#include "accumSim.h"
using namespace std;
reg_word accumulator = 0;
reg_word instructionRegister = 0;
mem_addr programCounter;
struct MipsMemory mipStorage;
struct AccumFileParser fileParser;
void fetchInstruction(){
    mipStorage.load(&instructionRegister, programCounter);
}

bool handleInstruction(){
    bool user_mode = true;
    //cout << endl << "Accumulator value IS: " << accumulator << endl;
    //cout << endl << "OPCODE value IS: " << instructionRegister  << endl;
    //cout << endl << "OPERAND value IS: " << mipStorage.read(mipStorage.read(programCounter + 1))  << endl;
    reg_word opCode = instructionRegister;
    reg_word operandA = mipStorage.read(programCounter + 1);
    switch(opCode)
    {
    case LOAD:
        mipStorage.load(&accumulator, operandA);
        break;
    case STO:
        mipStorage.store(operandA, &accumulator);
        break;
    case MUL:
        accumulator *= mipStorage.read(operandA);
        break;
    case ADD:
        accumulator += mipStorage.read(operandA);
        break;
    case END:
        user_mode = false;
        break;
    default:
        break;
    }
    programCounter += 2;
    return user_mode;
}

void loadInProgram(char * inputFilename){
    char const * harg = inputFilename;
    fileParser.loadFile(harg);
    vector <idValue> dataEntries = fileParser.idValuePairs;
    vector <operatorOperand> textEntries = fileParser.operatorOperandPairs;
    mipStorage.initializeMips(dataEntries, textEntries);
    programCounter = mipStorage.textLocation;
}

void accumRun(){
    bool userMode = true;
    while(userMode){
        fetchInstruction();
        //mipStorage.load(&instructionRegister, programCounter);
        userMode = handleInstruction();
    }
    cout << "This value was left in the accumulator: " << accumulator << endl;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        cout << "Improper usage: Please enter a single filename." << endl;
        return 1;
    }
    else{
        loadInProgram(argv[1]);
        accumRun();
        mipStorage.tearDownMips();
        return 0;
    }
}
