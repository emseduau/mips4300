#include "memorySim.h"
#include "fileParser.h"
#include "accumSim.h"
using namespace std;
reg_word accumulator = 0;
reg_word instructionRegister = 0;
mem_addr programCounter;
struct MipsMemory mipStorageAccum;
struct FileParser fileParserAccum;

void fetchInstruction(){
    mipStorageAccum.load(&instructionRegister, programCounter);
}

bool handleInstruction(){
    bool user_mode = true;
    //cout << endl << "Accumulator value IS: " << accumulator << endl;
    //cout << endl << "OPCODE value IS: " << instructionRegister  << endl;
    //cout << endl << "OPERAND value IS: " << mipStorage.read(mipStorage.read(programCounter + 1))  << endl;
    reg_word opCode = instructionRegister;
    reg_word operandA = mipStorageAccum.read(programCounter + 1);
    switch(opCode)
    {
    case LOAD:
        mipStorageAccum.load(&accumulator, operandA);
        break;
    case STO:
        mipStorageAccum.store(operandA, &accumulator);
        break;
    case MUL:
        accumulator *= mipStorageAccum.read(operandA);
        break;
    case ADD:
        accumulator += mipStorageAccum.read(operandA);
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

void loadInAccumProgram(char * inputFilename){
    char const * harg = inputFilename;
    fileParserAccum.loadFile(harg);
    vector <idValue> dataEntries = fileParserAccum.getIdValuePairs();
    vector <operatorOperand> textEntries = fileParserAccum.getOperatorOperandPairs();
    mipStorageAccum.initializeMips(dataEntries.size(), textEntries.size() * 2);
    fillMemory(dataEntries, textEntries);
    programCounter = mipStorageAccum.getTextLocation();
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
        mem_addr dataLocation = mipStorageAccum.dataNewAddress();
        mipStorageAccum.write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataLocation;
    }
    return relationSet;
}

void populateText(vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys){
    for(int i = 0, pairSize = inPairs.size(); i < pairSize; i++){
        operatorOperand currentStrings = inPairs.at(i);
        mem_word opcode = currentStrings.first;
        mem_word operandValue = varsToAddys[currentStrings.second];
        mipStorageAccum.write(mipStorageAccum.textNewAddress(), opcode);
        mipStorageAccum.write(mipStorageAccum.textNewAddress(), operandValue);
    }
}

void accumRun(){
    bool userMode = true;
    while(userMode){
        fetchInstruction();
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
        loadInAccumProgram(argv[1]);
        accumRun();
        mipStorageAccum.tearDownMips();
        return 0;
    }
}
