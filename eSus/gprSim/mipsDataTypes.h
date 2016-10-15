#ifndef MIPSDATATYPES_H_INCLUDED
#define MIPSDATATYPES_H_INCLUDED
#include <cstdint>
typedef int32_t mem_addr;
typedef int32_t mem_word;
typedef int32_t reg_word;
struct Instruction
{
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
    void encodeValue();
    void decodeValue();
    void decodeOP();
    mem_word getProcessedInstruction();
    void setInstructionType();
    void setSections(int sectionTwo, int sectionThree, int sectionFour);
    void setSectionTwo(int sectionValue);
    void setSectionThree(int sectionValue);
    void setSectionFour(int sectionValue);
    void loadIn(int inputOPcode, int sectionTwo, int sectionThree, int sectionFour);
    void unpack(mem_word inputInstruction);
};

#endif // MIPSDATATYPES_H_INCLUDED
