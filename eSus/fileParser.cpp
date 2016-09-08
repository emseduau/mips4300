#include  "fileParser.h"
using namespace std;
static const map<operationName, reg_word> operatorValueTable =
{
    {STO_STRING,  STO},
    {LOAD_STRING, LOAD},
    {POP_STRING,  POP},
    {PUSH_STRING, PUSH},
    {MUL_STRING,  MUL},
    {SUB_STRING,  SUB},
    {ADD_STRING,  ADD},
    {END_STRING,  END}
};
const std::string comment = "#";
const std::string dataLabel = ".data";
const std::string textLabel = ".text";


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

void FileParser::loadText()
{
    std::string workingString;
    std::string opCode;
    std::string operandString;
    getSectionLines(textLabel, dataLabel);
    for(size_t i = 0, loops = textLines.size(); i < loops; i++)
    {
        workingString = textLines.at(i);
        if(workingString.find(operatorOperandDelimiter) !=  std::string::npos)
        {
            operandString = "";
            opCode = workingString.substr(0, workingString.find(operatorOperandDelimiter));
            if (opCode !=   "END")
                operandString = workingString.substr(workingString.find(operatorOperandDelimiter) + 1, string::npos);
            operatorOperand linePair = make_pair(toOperator(opCode), operandString);
            operatorOperandPairs.push_back(linePair);
        }
    }
}

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

void FileParser::loadFile(const char * inputFilename)
{
    readLines(inputFilename);
    stripCommentsAndWhitespace();
    loadData();
    loadText();
}
// Utility function: Converts an ascii operator to it's encoded value;
reg_word FileParser::toOperator(std::string opString)
{
    const auto result = operatorValueTable.find(opString);
    if (result == operatorValueTable.end())
    {
        return -1;
    }
    return result->second;
}
std::vector <idValue> FileParser::getIdValuePairs()
{
    return idValuePairs;
}

std::vector <operatorOperand> FileParser::getOperatorOperandPairs()
{
    return operatorOperandPairs;
}
