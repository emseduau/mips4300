#ifndef MEMORYSIM_H_INCLUDED
#define MEMORYSIM_H_INCLUDED
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

#define TEXTSECTION (systemText.memStorage)
#define TEXTBOT systemText.memBottom
#define TEXTTOP (systemText.memTop)
#define DATASECTION (systemData.memStorage)
#define DATABOT (systemData.memBottom)
#define DATATOP (systemData.memTop)
#define STACKSECTION (systemStac.memStorage)
#define STACKBOT (systemStac.memBottom)
#define STACKTOP (systemStac.memTop)
#define KTEXTSECTION (kernelText.memStorage)
#define KTEXTBOT (kernelText.memBottom)
#define KTEXTTOP (kernelText.memTop)
#define KDATASECTION (kernelData.memStorage)
#define KDATABOT (kernelData.memBottom)
#define KDATATOP (kernelData.memTop)
#define MemorySize 5000
#define MemoryLimit 50000
#define MemoryExpansionChunk 20
#define textOffset    0x400000
#define dataOffset  0x10000000
#define stackOffset 0x7fffefff
#define kTextOffset 0x80000000
#define kDataOffset 0x90000000
#define INVALID_ADDRESS 0
#define STO   7
#define LOAD  6
#define POP   5
#define PUSH  4
#define MUL   3
#define SUB   2
#define ADD   1
#define END   0
#define TRUE  1
#define FALSE 0
struct MemSegment;
struct MipsMemory;
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
typedef int32_t register_number;
typedef std::pair<std::string, mem_word> idValue;
typedef std::pair<reg_word, std::string> operatorOperand;
void tearDownMips();
mem_word mipsRetreive(mem_addr targetAddress);
void mipsStore(mem_addr targetAddress, mem_word toStore);
std::map<std::string, mem_word> populateData(std::vector <idValue> dataPairs);
void populateText(std::vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys);
void expandData(long int extraBytes);
void expandStack(long int extraBytes);
void expandKData(long int extraBytes);
mem_addr dataNewAddress();
mem_addr textNewAddress();
mem_addr stackNewAddress();
mem_addr kTextNewAddress();
mem_addr kDataNewAddress();
mem_word read(mem_addr targetAddress);
void write(mem_addr targetAddress, mem_word valueToWrite);
void load(reg_word * regis, mem_addr targetAddress);
void store(mem_addr targetAddress, reg_word * regis);
void fetchInstruction();
struct MemSegment{
    mem_word * memStorage;
    long int memSize, memBottom, memTop, currentSize;
    void allocateSection(mem_word storageSize, long int bottom, long int top);
};

struct MipsMemory
{
    MemSegment systemText;
    MemSegment systemData;
    MemSegment systemStac;
    MemSegment kernelText;
    MemSegment kernelData;
    mem_word textLocation;
    mem_word textSize;
    mem_word dataSize;
    void initializeMips(std::vector <idValue> dataEntries, std::vector <operatorOperand> textEntries);
    void tearDownMips();
    mem_word mipsRetreive(mem_addr targetAddress);
    void mipsStore(mem_addr targetAddress, mem_word toStore);
    std::map<std::string, mem_word> populateData(std::vector <idValue> dataPairs);
    void populateText(std::vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys);
    void expandData(long int extraBytes);
    void expandStack(long int extraBytes);
    void expandKData(long int extraBytes);
    mem_addr dataNewAddress();
    mem_addr textNewAddress();
    mem_addr stackNewAddress();
    mem_addr kTextNewAddress();
    mem_addr kDataNewAddress();
    mem_word read(mem_addr targetAddress);
    void write(mem_addr targetAddress, mem_word valueToWrite);
    void load(reg_word * regis, mem_addr targetAddress);
    void store(mem_addr targetAddress, reg_word * regis);
    //void fetchInstruction(reg_word instRegister);
    reg_word toOperator(std::string opString);
};
#endif
