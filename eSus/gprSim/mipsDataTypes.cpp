#include "mipsDataTypes.h"
#include "opCodes.h"
using namespace std;
void Instruction::encodeValue()
{
    switch(instructionType)
    {
    case RTYPE:
        processedInstruction = (opcodeValue << 26) | (RSvalue << 21) | (RTvalue << 16);
        break;
    case ITYPE:
        processedInstruction = (opcodeValue << 26) | (RSvalue << 21) | (RTvalue << 16) | (IMMEDIATEvalue & 0xFFFF);
        break;
    case JTYPE:
        processedInstruction = (opcodeValue << 26) | (TARGETvalue & 0x3FFFFFF);
        break;
    default:
        break;
    }
}
void Instruction::decodeValue()
{
    opExtract = opcodeValue;
    switch(instructionType)
    {
    case RTYPE:
    case ITYPE:
        oneExtract = (processedInstruction >> 21) & 0x1F;
        twoExtract = (processedInstruction >> 16) & 0x1F;
        threeExtract = processedInstruction & 0xFFFF;
        break;
    case JTYPE:
        oneExtract = processedInstruction & 0x3FFFFFF;
        if(oneExtract & 0x2000000)
        {
            oneExtract = oneExtract | 0xFC000000;
        }
        break;
    default:
        break;
    }
}
void Instruction::decodeOP()
{
    opcodeValue = processedInstruction >> 26;
}

mem_word Instruction::getProcessedInstruction()
{
    return processedInstruction;
}

void Instruction::setInstructionType()
{
    switch(opcodeValue)
    {
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
        instructionType = ITYPE;
        break;
    case BINST:
        instructionType = JTYPE;
        break;
    default:
        break;
    }
}

void Instruction::setSections(int sectionTwo, int sectionThree, int sectionFour)
{
    switch(instructionType)
    {
    case RTYPE:
        RSvalue = sectionTwo;
        RTvalue = sectionThree;
        RDvalue = sectionFour;
        break;

    case ITYPE:
        RSvalue = sectionTwo;
        RTvalue = sectionThree;
        IMMEDIATEvalue = sectionFour;
        break;
    case JTYPE:
        TARGETvalue = sectionTwo;
    default:
        break;
    }
}

void Instruction::loadIn(int inputOPcode, int sectionTwo, int sectionThree, int sectionFour)
{
    opcodeValue = inputOPcode;
    setInstructionType();
    setSections(sectionTwo, sectionThree, sectionFour);
    encodeValue();
}

void Instruction::unpack(mem_word inputInstruction)
{
    processedInstruction = inputInstruction;
    decodeOP();
    setInstructionType();
    decodeValue();
}
