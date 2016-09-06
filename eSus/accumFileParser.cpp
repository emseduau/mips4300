#include  "accumFileParser.h"
using namespace std;
const std::string comment = "#";
std::string dataLabel = ".data";
std::string textLabel = ".text";
void AccumFileParser::loadData(){
    std::string workingString;
    std::string value;
    bool inData = false;
    for(size_t i = 0, loops = lines.size(); i < loops; i++)
    {
        workingString = lines.at(i);
        workingString.erase( std::remove_if( workingString.begin(), workingString.end(), ::isspace ), workingString.end() );
        if(workingString.find(comment) != std::string::npos)
            workingString.resize(workingString.find(comment));
        if(!inData)
        {
            if(workingString.find(dataLabel) != std::string::npos)
                inData = true;
        }
        else
        {
            if(workingString.find(textLabel) != std::string::npos)
                inData = false;
            else
            {
                if(workingString.find(":") !=  std::string::npos)
                {
                    mem_addr extractedValue;
                    std::string label = workingString.substr(0, workingString.find(":"));
                    value = workingString.substr(workingString.find(":") + 1, string::npos);

                    std::stringstream convert(value);
                    if( !(convert >> extractedValue))
                        extractedValue = 0;
                    //cout << label << " " << extractedValue << endl;
                    idValue extractedPair = std::make_pair(label, extractedValue);
                    idValuePairs.push_back(extractedPair);
                }
            }

        }
    }
}

void AccumFileParser::loadText(){
    std::string workingString;
    std::string opCode;
    std::string operandString;
    bool inText = false;
    for(size_t i = 0, loops = lines.size(); i < loops; i++)
    {
        workingString = lines.at(i);
        workingString.erase( std::remove_if( workingString.begin(), workingString.end(), ::isspace ), workingString.end() );
        if(workingString.find(comment) != std::string::npos)
            workingString.resize(workingString.find(comment));
        if(!inText)
        {
            if(workingString.find(textLabel) != std::string::npos)
                inText = true;
        }
        else
        {
            if(workingString.find(dataLabel) != std::string::npos)
                inText = false;
            else
            {
                if(workingString.find(":") !=  std::string::npos)
                {
                    operandString = "";
                    opCode = workingString.substr(0, workingString.find(":"));
                    if (opCode !=   "END")
                        operandString = workingString.substr(workingString.find(":") + 1, string::npos);
                    //cout << opCode << " " << operandString << endl;
                    operatorOperand linePair = make_pair(toOperator(opCode), operandString);
                    operatorOperandPairs.push_back(linePair);
                }
            }
        }
    }
}

void AccumFileParser::readLines(char const * inpy){
    ifstream myfile (inpy);
    string line;
    if (myfile.is_open()){
        while ( getline (myfile,line) )
            lines.push_back(line);
        myfile.close();
    }
    else cout << "Unable to open file";
}

void AccumFileParser::loadFile(const char * inputFilename){
    readLines(inputFilename);
    loadData();
    loadText();
    //operatorOperandStringPairs = getText(fileLines);
    //mipStorage.initializeMips(dataEntries, textEntries);
}
// Utility function: Converts an ascii operator to it's encoded value;
reg_word AccumFileParser::toOperator(std::string opString){
    reg_word output = 0;
    if(opString == "LOAD")
        output = LOAD;
    else if(opString == "STO")
        output = STO;
    else if(opString == "MUL")
        output = MUL;
    else if(opString == "ADD")
        output = ADD;
    else if(opString == "END")
        output = END;
    else if(opString == "PUSH")
        output = PUSH;
    else if(opString == "POP")
        output = POP;
    return output;
}

