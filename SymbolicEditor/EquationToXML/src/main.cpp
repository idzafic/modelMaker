#include "./../inc/generateXML.h"
#include "./../inc/nodes.h"
#include <iostream>
#include <string>
#include <fstream>
#include "xml/Writer.h"
#include <iostream>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

void displayHelp() {
    std::cout << "--help -> this menu\n"
              "-i -> input file\n"
              "-o -> output file\n"
              "if output is not specified it will have the same name as input\n";
}

int main(int argc, char* argv[]) {
    std::string inputString, outputString;
	fs::path inputPath;

    for (int i = 1; i < argc; ++i) {
		break;
        if (*argv[i] == '-') {
            if (std::strcmp(argv[i], "--help") == 0) {
                displayHelp();
                return 0;
            }

            if (i == argc - 1)
                break;

            if (std::strcmp(argv[i], "-i") == 0) {
                inputPath = fs::absolute(argv[++i]).string();
				inputString = fs::absolute(argv[++i]).string();
                continue;
            }

            if (std::strcmp(argv[i], "-o") == 0) {
                outputString = fs::absolute(argv[++i]).string();
                continue;
            }
        }
    }

	inputPath = fs::absolute("./../../modelMaker/Debug/nlimport.xml").string();
	inputString =  "/home/bots/Projects/Out/EquationToXML/Debug/./../../modelMaker/Debug/nlimport.xml";

    if (inputPath.empty()) {
        std::cout << "Invalid use\n";
        displayHelp();
        return 1;
    }


    if (outputString.empty()) {
        outputString = inputString.substr(0, inputString.find_last_of('.')) + ".modl";
    }

    std::cout << "Input file: " << inputString << "\n";
    std::cout << "Output file: " << outputString << "\n";



	std::ifstream input;
	

	input.open(inputPath);

	if (!input.is_open()) {
		std::cout << "Cant open file " << inputPath << "\n";
		return 10;
	}



	ModelNode mod;

	mod.readFromFile(inputString.c_str());
	td::String modelString;
	mod.prettyPrint(modelString);

	std::ofstream outputFile(outputString);
    if (!outputFile) {
        std::cerr << "Error: Could not open output file: " << outputString << '\n';
        return 1;
    }

    
    outputFile << modelString;


	/*ovo je za output u xml

	if (outputString.empty()) {
        outputString = inputString.substr(0, inputString.find_last_of('.')) + ".xml";
    }

	xml::Writer output(outputString);
	output.startDocument();

	if (!output.isOk()) {
		std::cout << "Cant write to path " << outputString << "\n";
		return 11;
	}

	td::String commands;
	int cnt = 0;
	bool first = true;
	while (input.is_open() && !input.eof()) {
		for (int i = 0; i < 100; i++){
			if (!input.is_open() || input.eof())
				break;
			std::string line;
			std::getline(input, line);
			commands += line.c_str();
			commands += "\n";
		}

		if(first){
			int f = commands.find("Model");
			commands = commands.subStr(f, -1);
			first = false;
		}

		mod.processCommands(commands);

		commands.clean();
	}

	mod.printNode(output);

	output.endDocument();
	output.close();

	*/

	std::cout << "Output written to " << outputString << "\n";

}


