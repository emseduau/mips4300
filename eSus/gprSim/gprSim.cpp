#include "opCodes.h"
#include "syscodes.h"
#include "memorySim.h"
#include "fileParser.h"
#include "gprSim.h"
using namespace std;
static const int ADDICYCLES[STAGECOUNT] = {2, 1, 2, 0, 1};
static const int SUBICYCLES[STAGECOUNT] = {2, 1, 2, 0, 1};

static const int BCYCLES[STAGECOUNT] = {2, 0, 2, 0, 0};
static const int BEQZCYCLES[STAGECOUNT] = {2, 1, 2, 0, 0};
static const int BGECYCLES[STAGECOUNT] = {2, 1, 2, 0, 0};
static const int BNECYCLES[STAGECOUNT] = {2, 1, 2, 0, 0};

static const int LACYCLES[STAGECOUNT] = {2, 0, 2, 0, 1};
static const int LBCYCLES[STAGECOUNT] = {2, 1, 2, 0, 1};
static const int LICYCLES[STAGECOUNT] = {2, 0, 0, 0, 1};

struct Instruction{
            mem_word processedInstruction;
            mem_word instructionType;
            mem_word opcodeValue;
            mem_word RSvalue;
            mem_word RTvalue;
            mem_word RDvalue;
            mem_word SHIFTvalue;
            mem_word FUNCTIONvalue;
            mem_word IMMEDIATEvalue;
            mem_word TARGETvalue;
            mem_word opExtract;
            mem_word oneExtract;
            mem_word twoExtract;
            mem_word threeExtract;
            void encodeValue(){
                switch(instructionType){
                    case RTYPE:
                        processedInstruction = (opcodeValue << 26) | (RSvalue << 21) | (RTvalue << 16);
                        break;
                    case ITYPE:
                        //printf("Had values %d, %d, %d, %d.\n",opcodeValue, RSvalue,RTvalue,IMMEDIATEvalue );
                        processedInstruction = (opcodeValue << 26) | (RSvalue << 21) | (RTvalue << 16) | (IMMEDIATEvalue & 0xFFFF);
                        break;
                    case JTYPE:
                        processedInstruction = (opcodeValue << 26) | (TARGETvalue & 0x3FFFFFF);
                        break;
                    default:
                        break;
                }
            }
            void decodeValue(){
                opExtract = opcodeValue;
                switch(instructionType){
                    case RTYPE:
                    case ITYPE:
                        oneExtract = (processedInstruction >> 21) & 0x1F;
                        twoExtract = (processedInstruction >> 16) & 0x1F;
                        threeExtract = processedInstruction & 0xFFFF;
                        //printf("Had values %d, %d, %d, %d.\n",opcodeValue, RSvalue,RTvalue,IMMEDIATEvalue );
                        //processedInstruction = (opcodeValue << 26) | (RSvalue << 21) | (RTvalue << 16) | (IMMEDIATEvalue & 0xFFFF);
                        break;
                    case JTYPE:
                        //oneExtract = (processedInstruction >> 21) & 0x1F;
                        oneExtract = processedInstruction & 0x3FFFFFF;
                        if(oneExtract & 0x2000000){
                            oneExtract = oneExtract | 0xFC000000;
                        }
                        //twoExtract = (gunpay.processedInstruction >> 16) & 0x1F;
                        //threeExtract = gunpay.processedInstruction & 0xFFFF;
                        //processedInstruction = (opcodeValue << 26) | (IMMEDIATEvalue & 0x3FFFFFF);
                        break;
                    default:
                        break;
                }
            }
            void decodeOP(){
                opcodeValue = processedInstruction >> 26;
                // (gunpay.processedInstruction >> 21) & 0x1F,(gunpay.processedInstruction >> 16) & 0x1F,gunpay.processedInstruction & 0xFFFF);
            }

            mem_word getProcessedInstruction(){
                return processedInstruction;

            }

            void setInstructionType(){
                switch(opcodeValue){
                    case END:
                    case SYSCALL:
                    case ADDI:
                    case LAINST:
                    case SUBI:
                    case LBINST:
                    case LIINST:

                    case BEQZ:
                    case BGE:
                    case BNE:
                        //printf("Encoded %d as ITYPE.\n", opcodeValue);
                        instructionType = ITYPE;
                        break;
                    case BINST:
                        instructionType = JTYPE;
                        break;
                    default:
                        break;
                }
            }

            void setSectionTwo(int sectionValue){
                switch(instructionType){
                    case RTYPE:
                    case ITYPE:
                        RSvalue = sectionValue;
                        break;
                    case JTYPE:
                        TARGETvalue = sectionValue;
                        break;
                    default:
                        break;
                }
            }

            void setSectionThree(int sectionValue){
                switch(instructionType){
                    case RTYPE:
                    case ITYPE:
                        RTvalue = sectionValue;
                        break;
                    case JTYPE:
                        break;
                    default:
                        break;
                }
            }

            void setSectionFour(int sectionValue){
                switch(instructionType){
                    case RTYPE:
                        RDvalue = sectionValue;
                        break;
                    case ITYPE:
                        IMMEDIATEvalue = sectionValue;
                        break;
                    case JTYPE:
                        break;
                    default:
                        break;
                }
            }

            void loadIn(int inputOPcode, int sectionTwo, int sectionThree, int sectionFour){
                opcodeValue = inputOPcode;
                setInstructionType();
                setSectionTwo(sectionTwo);
                setSectionThree(sectionThree);
                setSectionFour(sectionFour);
                encodeValue();
            }

            void unpack(mem_word inputInstruction){
                processedInstruction = inputInstruction;
                decodeOP();
                setInstructionType();
                decodeValue();
            }
        };



static const int SYSCALLCYCLES[STAGECOUNT] = {2, 1, 2, 2, 1};
static const map<reg_word, const int(*)[STAGECOUNT]> operatorValueTable =
{
    {ADDI,   &ADDICYCLES},
    {BINST,  &BCYCLES},
    {BEQZ,   &BEQZCYCLES},
    {BGE,    &BGECYCLES},
    {BNE,    &BNECYCLES},
    {LAINST, &LACYCLES},
    {LBINST, &LBCYCLES},
    {LIINST, &LICYCLES},
    {SUBI,   &SUBICYCLES},
    {SYSCALL,&SYSCALLCYCLES}
};

// Simulates the accumulator based CPU. This is the entry point to the program.
mem_word BOOLREGISTER = 0;
mem_word SYSCALLREG = 20;
mem_word MAX_INSTRUCTIONS = 5000;
mem_word IC = 0;
mem_word C = 0;
reg_word accumulator = 0;
reg_word registers[REGISTERCOUNT];
reg_word instructionRegister = 0;
mem_addr programCounter;
mem_word dataPlace; // = mipStorageAccum.getDataCurrent();
struct MipsMemory mipStorageAccum;
struct FileParser fileParserAccum;

void wipeRegisters(){
int i;
for(i = 0; i < REGISTERCOUNT; i++)
    {
        registers[i] = 777;
    }
}

int getCycles(const int cyclesArray[STAGECOUNT]){
    int sum = 0;
    int currentStage = 0;
    for(currentStage = 0; currentStage < STAGECOUNT; currentStage++){
        sum += cyclesArray[currentStage];

    }
    return sum;
}

// Loads the initial program counter value into the internal program counter register.
void fetchInstruction()
{
    mipStorageAccum.load(&instructionRegister, programCounter);
}

// Allows the system to retrieve the value in the accumulator using the proper system call.
void handleSystemCall(reg_word sysCode)
{
    switch(registers[SYSCALLREG])
    {
    case SYSCALL_PRINT_BOOLEAN:
        if(mipStorageAccum.read(registers[BOOLREGISTER]) == 0){
            cout << "The ALU responded to the SYSCALL with: FALSE\n";
        }
        else if(mipStorageAccum.read(registers[BOOLREGISTER]) == 1){
            cout << "The ALU responded to the SYSCALL with: TRUE\n";
        }
        else{
            cout << "The ALU responded to the SYSCALL with: ERROR SYSCODE\n";
        }
        break;
    default:
        cout << "SYSCALL PRINT: " << accumulator << " is in the accumulator." << endl;
        break;

    }
}

// Handles a single instruction, read in from memory at program counter.
bool handleInstruction()
{
    bool user_mode = true;
    int currentCycles = 0;
    Instruction workingInstruction;
    mem_word packedInstruction = instructionRegister;
    workingInstruction.unpack(packedInstruction);
    reg_word opCode = workingInstruction.opExtract;
    //printf("current OPCODE: %d.\n", opCode);
    //printf("Instruction unpacked values: %d, %d, %d, %d, %d.\n",workingInstruction.processedInstruction,workingInstruction.opExtract,
    //       workingInstruction.oneExtract, workingInstruction.twoExtract,workingInstruction.threeExtract);
    reg_word operandA = workingInstruction.oneExtract;//mipStorageAccum.read(programCounter + 1);
    reg_word operandB = workingInstruction.twoExtract;//mipStorageAccum.read(programCounter + 2);
    reg_word operandC = workingInstruction.threeExtract;//mipStorageAccum.read(programCounter + 3);



    reg_word operandAX = operandA; //mipStorageAccum.read(programCounter + 1);
    reg_word operandBX = operandB; //mipStorageAccum.read(programCounter + 2);
    reg_word operandCX = operandC; //mipStorageAccum.read(programCounter + 3);

    //printf("One [%d vs real %d]. Two [%d vs real %d]. Thr [%d vs real %d].\n",operandAX, operandA,operandBX, operandB,operandCX, operandC);

    //printf("For Two it is the attempted %d vs the real %d.\n",operandBX, operandB);

    //printf("For Three it is the attempted %d vs the real %d.\n",operandCX, operandC);
    programCounter += 1;
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
    case SYSCALL:
        if(operandAX != operandA){
            printf("SYSCALL: One [%d vs real %d].\n",operandAX, operandA);
        }
        handleSystemCall(mipStorageAccum.read(operandA));
        currentCycles = getCycles(SYSCALLCYCLES);
        break;
    case ADDI: //operandA, operandB, operandC
        if((operandAX != operandA) || (operandBX != operandB) || (operandCX != operandC) ){
        printf("ADDI: One [%d vs real %d]. Two [%d vs real %d]. Thr [%d vs real %d].\n",operandAX, operandA,operandBX, operandB,operandCX, operandC);
    }
        registers[operandA] = registers[operandB] + operandC;
        currentCycles = getCycles(ADDICYCLES);
        //opA = handleReg(operandString, 0);
        //opB = handleReg(operandString, 1);
        //opC = handleImmediate(operandString, 2, 2);
        break;
    case SUBI:
                if((operandAX != operandA) || (operandBX != operandB) || (operandCX != operandC) ){

        printf("SUBI: One [%d vs real %d]. Two [%d vs real %d]. Thr [%d vs real %d].\n",operandAX, operandA,operandBX, operandB,operandCX, operandC);
                }
        registers[operandA] = registers[operandB] - operandC;
        currentCycles = getCycles(SUBICYCLES);

        //opA = handleReg(operandString, 0);
        //opB = handleReg(operandString, 1);
        //opC = handleImmediate(operandString, 2, 2);
        break;
    case BINST:
        if(operandAX != operandA){
        printf("BINST: One [%d vs real %d].\n",operandAX, operandA);
        }
        programCounter += operandA;
        currentCycles = getCycles(BCYCLES);
        //opA = handleLabel(operandString, 0, 0, jumpsToAddys);
        break;
    case BEQZ:
                if((operandAX != operandA) || (operandBX != operandB) ){

        printf("BEQZ: One [%d vs real %d]. Two [%d vs real %d].\n",operandAX, operandA,operandBX, operandB);
                }
        if(registers[operandA] == 0){
                programCounter += operandB;
        }
        currentCycles = getCycles(BEQZCYCLES);
        //opA = handleReg(operandString, 0);
        //opB = handleLabel(operandString, 1, 1, jumpsToAddys);
        break;
    case LAINST:
                        if((operandAX != operandA) || (operandBX != operandB) ){

        printf("LAINST: One [%d vs real %d]. Two [%d vs real %d].\n",operandAX, operandA,operandBX, operandB);
                        }
        registers[operandA] = dataPlace + operandB;
        currentCycles = getCycles(LACYCLES);
        //opA = handleReg(operandString, 0);
        //opB = handleLabel(operandString, 1, 1, jumpsToAddys);
        break;
    case BGE:
                        if((operandAX != operandA) || (operandBX != operandB) || (operandCX != operandC) ){

        printf("BGE: One [%d vs real %d]. Two [%d vs real %d]. Thr [%d vs real %d].\n",operandAX, operandA,operandBX, operandB,operandCX, operandC);
                        }
        if(registers[operandA] >= registers[operandB]){
                programCounter += operandC;
        }
        currentCycles = getCycles(BGECYCLES);
        //opA = handleReg(operandString, 0);
        //opB = handleReg(operandString, 1);
        //opC = handleLabel(operandString, 2, 2, jumpsToAddys);
        break;
    case BNE:
                        if((operandAX != operandA) || (operandBX != operandB) || (operandCX != operandC) ){

        printf("BNE: One [%d vs real %d]. Two [%d vs real %d]. Thr [%d vs real %d].\n",operandAX, operandA,operandBX, operandB,operandCX, operandC);
                        }
        if(registers[operandA] != registers[operandB]){
                programCounter += operandC;
        }
        currentCycles = getCycles(BNECYCLES);
        //opA = handleReg(operandString, 0);
        //opB = handleReg(operandString, 1);
        //opC = handleLabel(operandString, 2, 2, jumpsToAddys);
        break;
    case LBINST:
                        if((operandAX != operandA) || (operandBX != operandB) || (operandCX != operandC) ){

        printf("LBINST: One [%d vs real %d]. Two [%d vs real %d]. Thr [%d vs real %d].\n",operandAX, operandA,operandBX, operandB,operandCX, operandC);
                        }
        registers[operandA] = mipStorageAccum.read(registers[operandB] + operandC);
        currentCycles = getCycles(LBCYCLES);
        //opA = handleReg(operandString, 0);
        //opB = handleReg(operandString, 1);
        //opC = handleOffset(operandString);
        break;
    case LIINST:
                        if((operandAX != operandA) || (operandBX != operandB) ){

        printf("LIINST: One [%d vs real %d]. Two [%d vs real %d].\n",operandAX, operandA,operandBX,operandB);
                        }
        registers[operandA] = operandB;
        currentCycles = getCycles(LICYCLES);
        break;
    default:
        printf("ERROR: Encountered an unknown instruction: %d\n", opCode);
        user_mode = false;
        break;
    }
    IC++;
    if (IC >= MAX_INSTRUCTIONS){
        user_mode = false;
        cout << "ERROR: Max instruction number reached!\n";
    }
    //cout << "Current instruction took " << currentCycles << " cycles. Running total cycle count is: " << << ".\n";
    C += currentCycles;
    //cout << "Instruction " << IC << " took " << currentCycles << " cycles. Running total cycle count is: " << C << ".\n";

    return user_mode;
}

// Initializes mips memory from the given file and sets the initial program counter.
void loadInAccumProgram(char * inputFilename)
{
    char const * harg = inputFilename;
    dataPlace = mipStorageAccum.getDataLocation();
    fileParserAccum.loadFile(harg);
    vector <idValue> dataEntries = fileParserAccum.getIdValuePairs();
    vector <operatorOperand> textEntries = fileParserAccum.getOperatorOperandPairs();
    vector <jumplocValue> jumplocValuePairs = fileParserAccum.getJumpPairs();
    mipStorageAccum.initializeMips(dataEntries.size(), textEntries.size());
    fillMemory(dataEntries, textEntries, jumplocValuePairs);
    programCounter = mipStorageAccum.getTextLocation();
}

// Initializes the text and data sections in memory.
void fillMemory(std::vector <idValue> dataEntries, std::vector <operatorOperand> textEntries, std::vector <jumplocValue> jumplocValuePairs)
{
    std::map<std::string, mem_word> relationSet = populateData(dataEntries);
    std::map<std::string, mem_word> jumpSet = populateJumps(jumplocValuePairs);
    populateText(textEntries, relationSet, jumpSet);
}

// Maps each memory entry to an address in memory.
std::map<std::string, mem_word> populateData(vector <idValue> dataPairs)
{
    mem_word dataCurrent; // = mipStorageAccum.getDataCurrent();
    mem_word dataSize = dataPairs.size();
    std::map<std::string, mem_word> relationSet;
    for(int i = 0; i < dataSize; i++)
    {
        dataCurrent = mipStorageAccum.getDataCurrent();
        idValue extractedRelation = dataPairs.at(i);
        mem_addr dataLocation = mipStorageAccum.dataNewAddress();
        mipStorageAccum.write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataCurrent;
    }
    return relationSet;
}

size_t find_nth(std::string haystack, size_t pos, const string& needle, size_t nth)
{
    size_t found_pos = haystack.find(needle, pos);
    if(0 == nth || string::npos == found_pos)  return found_pos;
    return find_nth(haystack, found_pos+1, needle, nth-1);
}


std::map<std::string, mem_word> populateJumps(std::vector <jumplocValue> jumplocValuePairs)
{
    //mem_addr tempPC = mipStorageAccum.getTextLocation();
    mem_word jumpSize = jumplocValuePairs.size();
    std::map<std::string, mem_word> relationSet;
    for(int i = 0; i < jumpSize; i++)
    {
        jumplocValue extractedRelation = jumplocValuePairs.at(i);
        mem_addr dataLocation = mipStorageAccum.dataNewAddress();
        mipStorageAccum.write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataLocation;
    }
    return relationSet;
}


int handleReg(std::string toWork, int which){
    int lengThing = 0;
        std::string operandA; // operandB, operandC;

        int theLoc = find_nth(toWork, 0, "$", which) + 1;
        int curLoc = theLoc;
        while(std::isdigit(toWork[curLoc++])){
            lengThing++;
        }
        return atoi(toWork.substr(theLoc, lengThing).c_str());
}

int handleJump(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys, mem_addr tempPC){
std::string operandA; // operandB, operandC;
mem_word tempPCOffset = tempPC;
int labelOffset;
int storedOffset;
        if ((which == 0) && (which == numOperands)){
            labelOffset = jumpsToAddys[toWork];
        }
        if (which == numOperands){
            labelOffset = jumpsToAddys[toWork.substr(find_nth(toWork, 0, ",", numOperands -1) + 1)];
        }
        else if (which == 0){
           labelOffset = jumpsToAddys[toWork.substr(0, find_nth(toWork, 0, ",", 0))];
        }
        else{
            int firstComma = find_nth(toWork, 0, ",", which - 1);
            int secondComma = find_nth(toWork, 0, ",", which);
            labelOffset = jumpsToAddys[toWork.substr(firstComma + 1, secondComma - firstComma - 1)];
        }
        storedOffset = mipStorageAccum.read(labelOffset) - tempPCOffset;
        return storedOffset;
}

int handleLabel(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys){
std::string operandA; // operandB, operandC;
        if ((which == 0) && (which == numOperands)){
            return jumpsToAddys[toWork];
        }
        if (which == numOperands){
            return jumpsToAddys[toWork.substr(find_nth(toWork, 0, ",", numOperands -1) + 1)];
        }
        else if (which == 0){
           return jumpsToAddys[toWork.substr(0, find_nth(toWork, 0, ",", 0))];
        }
        else{
            int firstComma = find_nth(toWork, 0, ",", which - 1);
            int secondComma = find_nth(toWork, 0, ",", which);
            return jumpsToAddys[toWork.substr(firstComma + 1, secondComma - firstComma - 1)];

        }
        return 1;
}

int handleOffset(std::string toWork){
        int theLoc = find_nth(toWork, 0, "(", 0) -1;
        int curLoc = theLoc;
        int lengThing = 0;
        while(std::isdigit(toWork[curLoc]) && toWork[curLoc] != ','){
            curLoc--;
            lengThing++;
        }
        return atoi(toWork.substr(curLoc + 1, lengThing).c_str());
}

int handleImmediate(std::string toWork, int which, int numOperands){
    int startLoc = 0;
    int lengthCount = 0;
    if (which == 0){
            while(std::isdigit(toWork[lengthCount]) && toWork[lengthCount] != ',' ){
                    lengthCount++;
            }
        return atoi(toWork.substr(0,lengthCount).c_str());

    }
    startLoc = find_nth(toWork, 0, ",", which - 1) +1;
    if(which == numOperands){
        return atoi(toWork.substr(startLoc).c_str());
    }
    else{
        while(std::isdigit(toWork[startLoc + lengthCount]) && (toWork[startLoc + lengthCount] = ',')){
                lengthCount++;
        }
        return atoi(toWork.substr(startLoc, lengthCount).c_str());
    }
    return 1;
}

void fillOperandValues(mem_word bakaBaka[3], operatorOperand currentStrings, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpsToAddys, mem_addr tempPC){
    mem_word opCode = currentStrings.first;
    std::string operandString = currentStrings.second;
    mem_word opA = 0;
    mem_word opB = 1;
    mem_word opC = 2;
    switch(opCode)
    {
    case LOAD:
    case STO:
    case MUL:
    case ADD:
    case END:
    case SYSCALL:
        opA = varsToAddys[operandString];
        break;
    case ADDI:
        opA = handleReg(operandString, 0);
        opB = handleReg(operandString, 1);
        opC = handleImmediate(operandString, 2, 2);
        break;
    case SUBI:
        opA = handleReg(operandString, 0);
        opB = handleReg(operandString, 1);
        opC = handleImmediate(operandString, 2, 2);
        break;
    case BINST:
        opA = handleJump(operandString, 0, 0, jumpsToAddys, tempPC);
        break;
    case BEQZ:
        opA = handleReg(operandString, 0);
        opB = handleJump(operandString, 1, 1, jumpsToAddys, tempPC);
        break;
    case LAINST:
        opA = handleReg(operandString, 0);
        opB = handleLabel(operandString, 1, 1, varsToAddys);
        break;
    case BGE:
        opA = handleReg(operandString, 0);
        opB = handleReg(operandString, 1);
        opC = handleJump(operandString, 2, 2, jumpsToAddys, tempPC);
        break;
    case BNE:
        opA = handleReg(operandString, 0);
        opB = handleReg(operandString, 1);
        opC = handleJump(operandString, 2, 2, jumpsToAddys, tempPC);
        break;
    case LBINST:
        opA = handleReg(operandString, 0);
        opB = handleReg(operandString, 1);
        opC = handleOffset(operandString);
        break;
    case LIINST:
        opA = handleReg(operandString, 0);
        opB = handleImmediate(operandString, 1, 1);
        break;
    default:
        break;
    }
    bakaBaka[0] = opA;
    bakaBaka[1] = opB;
    bakaBaka[2] = opC;
}

// Replaces references to data variables with their memory addresses and stores the instructions in memory.
void populateText(vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpsToAddys)
{
    mem_addr tempPCOffset = 1; // Offset of where PC will be pointing when the instruction is run
    for(int i = 0, pairSize = inPairs.size(); i < pairSize; i++)
    {
        operatorOperand currentStrings = inPairs.at(i);
        mem_word opcode = currentStrings.first;
        mem_word operandValues[3];
        fillOperandValues(operandValues, currentStrings, varsToAddys, jumpsToAddys, tempPCOffset);
        Instruction currentInstruction;
        currentInstruction.loadIn(opcode,operandValues[0], operandValues[1], operandValues[2]);
        mipStorageAccum.write(mipStorageAccum.textNewAddress(), currentInstruction.processedInstruction);
        tempPCOffset += 1;
    }
}

// Runs the loaded program until END is encountered.
void accumRun()
{
    bool userMode = true;
    while(userMode)
    {
        fetchInstruction();
        userMode = handleInstruction();
    }
    cout << "Executed a total of " << IC << " instructions. Total cycle count is: " << C << ".\n";
    int top = IC * 8;
    float speedup = (float) top/(float) C;
    printf("Our calculated speedup is: %g", speedup);
}

// Takes in the filename of the program and starts up the simulation.
int main(int argc, char *argv[])
{
    wipeRegisters();
    if(argc != 2)
    {
        cout << "Improper usage: Please enter a single filename." << endl;
        return 1;
    }
    else
    {
        loadInAccumProgram(argv[1]);
        accumRun();
        mipStorageAccum.tearDownMips();
        return 0;
    }
}
