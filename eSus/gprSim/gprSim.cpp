#include "opCodes.h"
#include "syscodes.h"
#include "memorySim.h"
#include "fileParser.h"
#include "gprSim.h"
#include "mipsDataTypes.h"
#include "loader.h"
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

void wipeRegisters()
{
    int i;
    for(i = 0; i < REGISTERCOUNT; i++)
    {
        registers[i] = 777;
    }
}

int getCycles(const int cyclesArray[STAGECOUNT])
{
    int sum = 0;
    int currentStage = 0;
    for(currentStage = 0; currentStage < STAGECOUNT; currentStage++)
    {
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
        if(mipStorageAccum.read(registers[BOOLREGISTER]) == 0)
        {
            cout << "The ALU responded to the SYSCALL with: FALSE\n";
        }
        else if(mipStorageAccum.read(registers[BOOLREGISTER]) == 1)
        {
            cout << "The ALU responded to the SYSCALL with: TRUE\n";
        }
        else
        {
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
    reg_word operandA = workingInstruction.oneExtract;
    reg_word operandB = workingInstruction.twoExtract;
    reg_word operandC = workingInstruction.threeExtract;
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
        handleSystemCall(mipStorageAccum.read(operandA));
        currentCycles = getCycles(SYSCALLCYCLES);
        break;
    case ADDI:
        registers[operandA] = registers[operandB] + operandC;
        currentCycles = getCycles(ADDICYCLES);
        break;
    case SUBI:
        registers[operandA] = registers[operandB] - operandC;
        currentCycles = getCycles(SUBICYCLES);
        break;
    case BINST:
        programCounter += operandA;
        currentCycles = getCycles(BCYCLES);
        break;
    case BEQZ:
        if(registers[operandA] == 0)
        {
            programCounter += operandB;
        }
        currentCycles = getCycles(BEQZCYCLES);
        break;
    case LAINST:
        registers[operandA] = dataPlace + operandB;
        currentCycles = getCycles(LACYCLES);
        break;
    case BGE:
        if(registers[operandA] >= registers[operandB])
        {
            programCounter += operandC;
        }
        currentCycles = getCycles(BGECYCLES);
        break;
    case BNE:
        if(registers[operandA] != registers[operandB])
        {
            programCounter += operandC;
        }
        currentCycles = getCycles(BNECYCLES);
        break;
    case LBINST:
        registers[operandA] = mipStorageAccum.read(registers[operandB] + operandC);
        currentCycles = getCycles(LBCYCLES);
        break;
    case LIINST:
        registers[operandA] = operandB;
        currentCycles = getCycles(LICYCLES);
        break;
    default:
        printf("ERROR: Encountered an unknown instruction: %d\n", opCode);
        user_mode = false;
        break;
    }
    IC++;
    if (IC >= MAX_INSTRUCTIONS)
    {
        user_mode = false;
        cout << "ERROR: Max instruction number reached!\n";
    }
    C += currentCycles;

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
    Loader workingLoader(&mipStorageAccum,dataEntries,textEntries, jumplocValuePairs);
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
