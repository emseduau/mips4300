#include  "fileParser.h"
using namespace std;
const std::string comment = "#";
const std::string dataLabel = ".data";
const std::string textLabel = ".text";
void FileParser::getSectionLine(std::string section, std::string sectionEntry){
    if (section == dataLabel){
        dataLines.push_back(sectionEntry);
    }
    else{
        textLines.push_back(sectionEntry);
    }
}

void FileParser::getSectionLines(std::string targetSection, std::string otherSection){
    std::string sectionLines;
    std::string workingString;
    bool inSection = false;
    for(size_t i = 0, loops = lines.size(); i < loops; i++){
        workingString = lines.at(i);
        if(!inSection){
            inSection = (workingString.find(targetSection) != std::string::npos);
        }
        else{
            inSection = workingString.find(otherSection) == std::string::npos;
            if(inSection)
                getSectionLine(targetSection, workingString);
        }
    }


}

void FileParser::loadData(){
    std::string workingString;
    std::string value;
    getSectionLines(dataLabel, textLabel);
    for(size_t i = 0, loops = dataLines.size(); i < loops; i++){
        workingString = dataLines.at(i);
        if(workingString.find(":") !=  std::string::npos){
            mem_addr extractedValue;
            std::string label = workingString.substr(0, workingString.find(":"));
            value = workingString.substr(workingString.find(":") + 1, string::npos);
            std::stringstream convert(value);
            if( !(convert >> extractedValue))
                extractedValue = 0;
            idValue extractedPair = std::make_pair(label, extractedValue);
            idValuePairs.push_back(extractedPair);
        }
    }
}

void FileParser::loadText(){
    std::string workingString;
    std::string opCode;
    std::string operandString;
    getSectionLines(textLabel, dataLabel);
    for(size_t i = 0, loops = textLines.size(); i < loops; i++){
        workingString = textLines.at(i);
        if(workingString.find(":") !=  std::string::npos){
            operandString = "";
            opCode = workingString.substr(0, workingString.find(":"));
            if (opCode !=   "END")
                operandString = workingString.substr(workingString.find(":") + 1, string::npos);
            operatorOperand linePair = make_pair(toOperator(opCode), operandString);
            operatorOperandPairs.push_back(linePair);
        }
    }
}

void FileParser::stripCommentsAndWhitespace(){
    std::string workingString;
    std::string value;
    for(size_t i = 0, loops = lines.size(); i < loops; i++){
        workingString = lines.at(i);
        workingString.erase( std::remove_if( workingString.begin(), workingString.end(), ::isspace ), workingString.end() );
        if(workingString.find(comment) != std::string::npos)
            workingString.resize(workingString.find(comment));
        lines[i] = workingString;
    }
}

void FileParser::readLines(char const * inpy){
    ifstream myfile (inpy);
    string line;
    if (myfile.is_open()){
        while ( getline (myfile,line) )
            lines.push_back(line);
        myfile.close();
    }
    else {
            cout << "Unable to open file" << endl;
    }
}

void FileParser::loadFile(const char * inputFilename){
    readLines(inputFilename);
    stripCommentsAndWhitespace();
    loadData();
    loadText();
}
// Utility function: Converts an ascii operator to it's encoded value;
reg_word FileParser::toOperator(std::string opString){
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

