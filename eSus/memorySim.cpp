#include "memorySim.h"
using namespace std;
void MemSegment::allocateSection(mem_word storageSize, long int bottom, long int top){
    memStorage = new mem_word[storageSize];
    memSize = storageSize;
    memBottom = bottom;
    memTop = top;
    currentSize = 0;
}
void MipsMemory::initializeMips(vector <idValue> dataEntries, vector <operatorOperand> textEntries){
    textLocation = textOffset;
    textSize = (2 * textEntries.size());
    dataSize = dataEntries.size();
    systemText.allocateSection(textSize, textOffset, textOffset + textSize);
    systemData.allocateSection(dataSize, dataOffset, dataOffset + dataSize);
    systemStac.allocateSection(MemorySize, stackOffset - MemorySize, stackOffset);
    kernelText.allocateSection(MemorySize, kTextOffset, kTextOffset + MemorySize);
    kernelData.allocateSection(MemorySize, kDataOffset, kDataOffset + MemorySize);
    std::map<std::string, mem_word> variablesToMemory = populateData(dataEntries);
    populateText(textEntries, variablesToMemory);
}

void MipsMemory::tearDownMips(){
    delete[] systemText.memStorage;
    delete[] systemData.memStorage;
    delete[] systemStac.memStorage;
    delete[] kernelText.memStorage;
    delete[] kernelData.memStorage;
}

//Takes a raw memory address and gets it's contents from the proper section.

mem_word MipsMemory::mipsRetreive(mem_addr targetAddress)
{
    if(targetAddress >= TEXTBOT && targetAddress < TEXTTOP)
        return TEXTSECTION[targetAddress - TEXTBOT];
    else if(targetAddress >= DATABOT && targetAddress < DATATOP)
        return DATASECTION[targetAddress - DATABOT];
    else if(targetAddress >= STACKBOT && targetAddress < STACKTOP)
        return STACKSECTION[targetAddress - STACKBOT];
    else if(targetAddress >= KTEXTBOT && targetAddress < KTEXTTOP)
        return KTEXTSECTION[targetAddress - KTEXTBOT];
    else if(targetAddress >= KDATABOT && targetAddress < KDATATOP)
        return KDATASECTION[targetAddress - KDATABOT];
    else
        return INVALID_ADDRESS;
}

//Takes a raw memory address and data to store there, placing the data in the proper section.

void MipsMemory::mipsStore(mem_addr targetAddress, mem_word toStore)
{
    if(targetAddress >= TEXTBOT && targetAddress < TEXTTOP)
        TEXTSECTION[targetAddress - TEXTBOT] = toStore;
    else if(targetAddress >= DATABOT && targetAddress < DATATOP)
        DATASECTION[targetAddress - DATABOT] = toStore;
    else if(targetAddress >= STACKBOT && targetAddress < STACKTOP)
        STACKSECTION[targetAddress - STACKBOT] = toStore;
    else if(targetAddress >= KTEXTBOT && targetAddress < KTEXTTOP)
        KTEXTSECTION[targetAddress - KTEXTBOT] = toStore;
    else if(targetAddress >= KDATABOT && targetAddress < KDATATOP)
        KDATASECTION[targetAddress - KDATABOT] = toStore;
    else
        throw "Fatal Error: Simulated memory access violation.";
}

//Populates the  section of MIPS memory with data loaded from file.

std::map<std::string, mem_word> MipsMemory::populateData(vector <idValue> dataPairs){
    std::map<std::string, mem_word> relationSet;
    for(int i = 0; i < dataSize; i++){
        idValue extractedRelation = dataPairs.at(i);
        mem_addr dataLocation = dataNewAddress();
        write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataLocation;
    }
    return relationSet;
}

//Populates the text section of MIPS memory with data loaded from file.

void MipsMemory::populateText(vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys){
    for(int i = 0; i < textSize /2; i++){
        operatorOperand currentStrings = inPairs.at(i);
        mem_word opcode = currentStrings.first;
        mem_word operandValue = varsToAddys[currentStrings.second];
        write(textNewAddress(), opcode);
        write(textNewAddress(), operandValue);
    }
}

//Expands the data section of MIPS memory.

void MipsMemory::expandData(long int extraBytes)
{
    long oldSize = DATATOP - DATABOT;
    long newSize = oldSize + extraBytes;
    if(DATATOP + extraBytes > STACKBOT)
        throw "Fatal Error: Ran out of memory in expand data.";
    DATATOP += extraBytes;
    systemData.memSize += extraBytes;
    DATASECTION = (mem_word *)realloc(DATASECTION, newSize);
    if(DATASECTION == NULL)
        throw "Fatal Error: realloc failed in expand data.";
    for(int i = oldSize; i < newSize; i++ )
        DATASECTION[i] = 0;
}

//Expands the stack section of MIPS memory.

void MipsMemory::expandStack(long int extraBytes)
{
    long oldSize = STACKTOP - STACKBOT;
    long newSize = oldSize + extraBytes;
    mem_word * newSegment;
    mem_word pointerOld, pointerNew;

    if(STACKBOT - extraBytes < DATATOP)
        throw "Fatal Error: Ran out of memory in expand stack.";
    newSegment = (mem_word *) malloc(newSize);
    pointerOld = oldSize - 1;
    pointerNew = newSize - 1;
    for(; pointerOld >= 0; ) newSegment[pointerNew--] = STACKSECTION[pointerOld--];
    for(; pointerNew >= 0; ) newSegment[pointerNew--] = 0;
    delete [] STACKSECTION;
    STACKSECTION = newSegment;
    STACKBOT -= (newSize - oldSize);
    systemStac.memSize += extraBytes;
}

//Expands the kernel data section of MIPS memory.

void MipsMemory::expandKData(long int extraBytes)
{
    long oldSize = KDATATOP - KDATABOT;
    long newSize = oldSize + extraBytes;
    KDATATOP += extraBytes;
    kernelData.memSize += extraBytes;
    KDATASECTION = (mem_word *)realloc(KDATASECTION, newSize);
    if(KDATASECTION == NULL)
        throw "Fatal Error: realloc failed in expand kernel data.";
    for(int i = oldSize; i < newSize; i++ )
        KDATASECTION[i] = 0;

}


//Retrieves an unallocated address in the data section of MIPS memory, expanding it if needed.

mem_addr MipsMemory::dataNewAddress(){
    mem_addr returnedAddress;
    if(systemData.currentSize == systemData.memSize)
        expandData(MemoryExpansionChunk);
    returnedAddress = systemData.memBottom + systemData.currentSize;
    systemData.currentSize++;
    return returnedAddress;
}

//Retrieves an unallocated address in the text section of MIPS memory, raising an error if the section is full.

mem_addr MipsMemory::textNewAddress(){
    mem_addr returnedAddress;
    if(systemText.currentSize == systemText.memSize)
        return INVALID_ADDRESS;
    returnedAddress = systemText.memBottom + systemText.currentSize;
    systemText.currentSize++;
    return returnedAddress;
}

//Retrieves an unallocated address in the stack section of MIPS memory, expanding it if needed.

mem_addr MipsMemory::stackNewAddress(){
    mem_addr returnedAddress;
    if(systemStac.currentSize == systemStac.memSize)
        expandStack(MemoryExpansionChunk);
    returnedAddress = systemStac.memTop - systemStac.currentSize;
    systemStac.currentSize++;
    return returnedAddress;
}

//Retrieves an unallocated address in the kernel text section of MIPS memory.

mem_addr MipsMemory::kTextNewAddress(){
    mem_addr returnedAddress;
    if(kernelText.currentSize == kernelText.memSize)
        return INVALID_ADDRESS;
    returnedAddress = kernelText.memBottom + kernelText.currentSize;
    kernelText.currentSize++;
    return returnedAddress;
}

//Retrieves an unallocated address in the kernel text section of MIPS memory.

mem_addr MipsMemory::kDataNewAddress(){
    mem_addr returnedAddress;
    if(kernelData.currentSize == kernelData.memSize)
        return INVALID_ADDRESS;
    returnedAddress = kernelData.memBottom + kernelData.currentSize;
    kernelData.currentSize++;
    return returnedAddress;
}
//Reads the contents of an address in MIPS memory.
mem_word MipsMemory::read(mem_addr targetAddress){
    return mipsRetreive(targetAddress);
}

//Writes to an address in MIPS memory.
void MipsMemory::write(mem_addr targetAddress, mem_word valueToWrite){
    mipsStore(targetAddress, valueToWrite);
}

//Takes a register and address and reads a value from a MIPS memory address into the register.
void MipsMemory::load(reg_word * regis, mem_addr targetAddress){
    * regis = mipsRetreive(targetAddress);
}

//Takes an address and register and writes the value of the register into the address in MIPS memory.
void MipsMemory::store(mem_addr targetAddress, reg_word * regis){
    mipsStore(targetAddress, * regis);
}
