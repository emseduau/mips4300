#include "loader.h"
#include "opCodes.h"
using namespace std;

std::map<std::string, mem_word> Loader::populateData(vector <idValue> dataPairs)
{
    mem_word dataCurrent; // = mipStorageAccum.getDataCurrent();
    mem_word dataSize = dataPairs.size();
    std::map<std::string, mem_word> relationSet;
    for(int i = 0; i < dataSize; i++)
    {
        dataCurrent = (*workingMemory).getDataCurrent();
        idValue extractedRelation = dataPairs.at(i);
        mem_addr dataLocation = (*workingMemory).dataNewAddress();
        (*workingMemory).write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataCurrent;
    }
    return relationSet;
}
std::map<std::string, mem_word> Loader::populateJumps(std::vector <jumplocValue> jumplocValuePairs)
{
    mem_word jumpSize = jumplocValuePairs.size();
    std::map<std::string, mem_word> relationSet;
    for(int i = 0; i < jumpSize; i++)
    {
        jumplocValue extractedRelation = jumplocValuePairs.at(i);
        mem_addr dataLocation = (*workingMemory).dataNewAddress();
        (*workingMemory).write(dataLocation, extractedRelation.second);
        relationSet[extractedRelation.first] = dataLocation;
    }
    return relationSet;
}

void Loader::fillOperandValues(mem_word bakaBaka[3], operatorOperand currentStrings, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpsToAddys, mem_addr tempPC)
{
    mem_word opCode = currentStrings.first;
    std::string operandString = currentStrings.second;
    mem_word opA = 0;
    mem_word opB = 0;
    mem_word opC = 0;
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

void Loader::populateText(vector<operatorOperand> inPairs, std::map<std::string, mem_word> varsToAddys, std::map<std::string, mem_word> jumpsToAddys)
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
        (*workingMemory).write((*workingMemory).textNewAddress(), currentInstruction.processedInstruction);
        tempPCOffset += 1;
    }
}

int Loader::handleJump(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys, mem_addr tempPC)
{
    std::string operandA; // operandB, operandC;
    mem_word tempPCOffset = tempPC;
    int labelOffset;
    int storedOffset;
    if ((which == 0) && (which == numOperands))
    {
        labelOffset = jumpsToAddys[toWork];
    }
    if (which == numOperands)
    {
        labelOffset = jumpsToAddys[toWork.substr(find_nth(toWork, 0, ",", numOperands -1) + 1)];
    }
    else if (which == 0)
    {
        labelOffset = jumpsToAddys[toWork.substr(0, find_nth(toWork, 0, ",", 0))];
    }
    else
    {
        int firstComma = find_nth(toWork, 0, ",", which - 1);
        int secondComma = find_nth(toWork, 0, ",", which);
        labelOffset = jumpsToAddys[toWork.substr(firstComma + 1, secondComma - firstComma - 1)];
    }
    storedOffset = (*workingMemory).read(labelOffset) - tempPCOffset;
    return storedOffset;
}

int Loader::handleLabel(std::string toWork, int which, int numOperands, std::map<std::string, mem_word> jumpsToAddys)
{
    std::string operandA; // operandB, operandC;
    if ((which == 0) && (which == numOperands))
    {
        return jumpsToAddys[toWork];
    }
    if (which == numOperands)
    {
        return jumpsToAddys[toWork.substr(find_nth(toWork, 0, ",", numOperands -1) + 1)];
    }
    else if (which == 0)
    {
        return jumpsToAddys[toWork.substr(0, find_nth(toWork, 0, ",", 0))];
    }
    else
    {
        int firstComma = find_nth(toWork, 0, ",", which - 1);
        int secondComma = find_nth(toWork, 0, ",", which);
        return jumpsToAddys[toWork.substr(firstComma + 1, secondComma - firstComma - 1)];

    }
    return 1;
}

int Loader::handleOffset(std::string toWork)
{
    int theLoc = find_nth(toWork, 0, "(", 0) -1;
    int curLoc = theLoc;
    int lengThing = 0;
    while(std::isdigit(toWork[curLoc]) && toWork[curLoc] != ',')
    {
        curLoc--;
        lengThing++;
    }
    return atoi(toWork.substr(curLoc + 1, lengThing).c_str());
}

int Loader::handleImmediate(std::string toWork, int which, int numOperands)
{
    int startLoc = 0;
    int lengthCount = 0;
    if (which == 0)
    {
        while(std::isdigit(toWork[lengthCount]) && toWork[lengthCount] != ',' )
        {
            lengthCount++;
        }
        return atoi(toWork.substr(0,lengthCount).c_str());

    }
    startLoc = find_nth(toWork, 0, ",", which - 1) +1;
    if(which == numOperands)
    {
        return atoi(toWork.substr(startLoc).c_str());
    }
    else
    {
        while(std::isdigit(toWork[startLoc + lengthCount]) && (toWork[startLoc + lengthCount] = ','))
        {
            lengthCount++;
        }
        return atoi(toWork.substr(startLoc, lengthCount).c_str());
    }
    return 1;
}

Loader::Loader(MipsMemory * inputMemory, std::vector <idValue> dataEntries,std::vector <operatorOperand> textEntries,std::vector <jumplocValue> jumplocValuePairs)
{
    workingMemory = inputMemory;
    std::map<std::string, mem_word> relationSet = populateData(dataEntries);
    std::map<std::string, mem_word> jumpSet = populateJumps(jumplocValuePairs);
    populateText(textEntries, relationSet, jumpSet);
}

int Loader::handleReg(std::string toWork, int which)
{
    int lengThing = 0;
    std::string operandA; // operandB, operandC;

    int theLoc = find_nth(toWork, 0, "$", which) + 1;
    int curLoc = theLoc;
    while(std::isdigit(toWork[curLoc++]))
    {
        lengThing++;
    }
    return atoi(toWork.substr(theLoc, lengThing).c_str());
}

size_t Loader::find_nth(std::string haystack, size_t pos, const string& needle, size_t nth)
{
    size_t found_pos = haystack.find(needle, pos);
    if(0 == nth || string::npos == found_pos)  return found_pos;
    return find_nth(haystack, found_pos+1, needle, nth-1);
}
