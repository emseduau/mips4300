#include "fileParser.h"
using namespace std;
//Maps strings to their OPcode value.
const std::map<operationName, operatorAndType> FileParser::operatorValueTable =
    {
        {SUBI_STRING,    operatorAndType(SUBI, SUBI_TYPE)},
        {LIINST_STRING,  operatorAndType(LIINST, LIINST_TYPE)},
        {LBINST_STRING,  operatorAndType(LBINST,LBINST_TYPE)},
        {LAINST_STRING,  operatorAndType(LAINST,LAINST_TYPE)},
        {BNE_STRING,     operatorAndType(BNE,BNE_TYPE)},
        {BGE_STRING,     operatorAndType(BGE,BGE_TYPE)},
        {BEQZ_STRING,    operatorAndType(BEQZ,BEQZ_TYPE)},
        {BINST_STRING,   operatorAndType(BINST,BINST_TYPE)},
        {ADDI_STRING,    operatorAndType(ADDI,ADDI_TYPE)},
        {STO_STRING,     operatorAndType(STO,STO_TYPE)},
        {LOAD_STRING,    operatorAndType(LOAD,LOAD_TYPE)},
        {POP_STRING,     operatorAndType(POP,POP_TYPE)},
        {PUSH_STRING,    operatorAndType(PUSH,PUSH_TYPE)},
        {MUL_STRING,     operatorAndType(MUL,MUL_TYPE)},
        {SUB_STRING,     operatorAndType(SUB,SUB_TYPE)},
        {ADD_STRING,     operatorAndType(ADD,ADD_TYPE)},
        {END_STRING,     operatorAndType(END,END_TYPE)},
        {SYSCALL_STRING, operatorAndType(SYSCALL,SYSCALL_TYPE)}
    };
const std::map<reg_word, opType> FileParser::operatorTypeTable =
    {
        {SUBI, SUBI_TYPE},
        {LIINST, LIINST_TYPE},
        {LBINST,LBINST_TYPE},
        {LAINST,LAINST_TYPE},
        {BNE,BNE_TYPE},
        {BGE,BGE_TYPE},
        {BEQZ,BEQZ_TYPE},
        {BINST,BINST_TYPE},
        {ADDI,ADDI_TYPE},
        {STO,STO_TYPE},
        {LOAD,LOAD_TYPE},
        {POP,POP_TYPE},
        {PUSH,PUSH_TYPE},
        {MUL,MUL_TYPE},
        {SUB,SUB_TYPE},
        {ADD,ADD_TYPE},
        {END,END_TYPE},
        {SYSCALL,SYSCALL_TYPE}
    };



const std::string FileParser::comment = "#";
const std::string FileParser::dataLabel = ".data";
const std::string FileParser::textLabel = ".text";

//Loads a line into its associated section.
//section: The label of the intended section.
//sectionEntry: The processed entry.
void FileParser::getSectionLine(std::string section, std::string sectionEntry)
{
    if (section == dataLabel)
    {
        dataLines.push_back(sectionEntry);
    }
    else
    {
        textLines.push_back(sectionEntry);
    }
}

//Iterates over the lines in a file, getting only those in the targetSection.
//targetSection: The label of the intended section.
//otherSection: The label indicating the possible end of the section.
void FileParser::getSectionLines(std::string targetSection, std::string otherSection)
{
    std::string sectionLines;
    std::string workingString;
    bool inSection = false;
    for(size_t i = 0, loops = lines.size(); i < loops; i++)
    {
        workingString = lines.at(i);
        if(!inSection)
        {
            inSection = (workingString.find(targetSection) != std::string::npos);
        }
        else
        {
            inSection = workingString.find(otherSection) == std::string::npos;
            if(inSection)
                getSectionLine(targetSection, workingString);
        }
    }
}

//Fills idValuePairs with the entries from the data section.
void FileParser::loadData()
{
    std::string workingString;
    std::string value;
    getSectionLines(dataLabel, textLabel);
    for(size_t i = 0, loops = dataLines.size(); i < loops; i++)
    {
        workingString = dataLines.at(i);
        if(workingString.find(labelValueDelimiter) !=  std::string::npos)
        {
            mem_addr extractedValue;
            std::string label = workingString.substr(0, workingString.find(labelValueDelimiter));
            value = workingString.substr(workingString.find(labelValueDelimiter) + 1, string::npos);
            std::stringstream convert(value);
            if( !(convert >> extractedValue))
                extractedValue = 0;
            idValue extractedPair = std::make_pair(label, extractedValue);
            idValuePairs.push_back(extractedPair);
        }
    }
}

//Fills operatorOperandPairs with the entries from the text section.
void FileParser::loadText()
{
    mem_word currentLabelOffset = 0;
    int labelTracker[textLines.size()];
    for(size_t i = 0, loops = textLines.size(); i < loops; i++)
    {
        labelTracker[i] = 0;
    }
    std::string workingString;
    std::string opCode;
    std::string operandString;
    std::string labelString;
    int operatorStart = 0;
    getSectionLines(textLabel, dataLabel);
    for(size_t i = 0, loops = textLines.size(); i < loops; i++)
    {
        operatorStart = 0;
        workingString = textLines.at(i);
        if(workingString.find(textLabelDelimiter) !=  std::string::npos){
            labelString = workingString.substr(0, workingString.find(textLabelDelimiter));
            operatorStart = workingString.find(textLabelDelimiter) + 1;
            jumplocValue jumpPair = make_pair(labelString, currentLabelOffset);
            jumplocValuePairs.push_back(jumpPair);

        }
        if(workingString.find(operatorOperandDelimiter) !=  std::string::npos)
        {
            operandString = "";
            opCode = workingString.substr(operatorStart, workingString.find(operatorOperandDelimiter) - operatorStart);
            if (opCode !=   END_STRING)
                operandString = workingString.substr(workingString.find(operatorOperandDelimiter) + 1, string::npos);
            operatorOperand linePair = make_pair(toOperator(opCode), operandString);
            operatorOperandPairs.push_back(linePair);
        currentLabelOffset += 1;
        }
    }
}

//Removes whitespace and comments after lines has been initialized.
void FileParser::stripCommentsAndWhitespace()
{
    std::string workingString;
    std::string value;
    for(size_t i = 0, loops = lines.size(); i < loops; i++)
    {
        workingString = lines.at(i);
        workingString.erase( std::remove_if( workingString.begin(), workingString.end(), ::isspace ), workingString.end() );
        if(workingString.find(comment) != std::string::npos)
            workingString.resize(workingString.find(comment));
        lines[i] = workingString;
    }
}

//Fills lines, getting the raw lines out of a file.
void FileParser::readLines(char const * inpy)
{
    ifstream myfile (inpy);
    string line;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
            lines.push_back(line);
        myfile.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }
}

//Initializes and processes all data from the file.
void FileParser::loadFile(const char * inputFilename)
{
    readLines(inputFilename);
    stripCommentsAndWhitespace();
    loadData();
    loadText();
}

opType FileParser::getType(reg_word currentOperation){
    const auto result = operatorTypeTable.find(currentOperation);
    if (result == operatorTypeTable.end())
    {
        return -1;
    }
    return result->second;
}
// Utility function: Converts an ascii operator to it's encoded value;
reg_word FileParser::toOperator(std::string opString)
{
    const auto result = operatorValueTable.find(opString);
    if (result == operatorValueTable.end())
    {
        return -1;
    }

    return result->second.first;
}
// Private variable access function. Returns the processed data section entries.
std::vector <idValue> FileParser::getIdValuePairs()
{
    return idValuePairs;
}
// Private variable access function. Returns the processed text section entries.
std::vector <operatorOperand> FileParser::getOperatorOperandPairs()
{
    return operatorOperandPairs;
}
std::vector <jumplocValue> FileParser::getJumpPairs(){
    return jumplocValuePairs;
}
