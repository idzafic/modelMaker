#include "./../inc/generateXML.h"
#include "./../inc/nodes.h"
#include <iostream>
#include <string>
#include <fstream>
#include "xml/Writer.h"

void displayHelp() {
	std::cout << "--help -> this menu\n"
		"-i -> input file\n"
		"-o -> output file\n"
		"if output is not specified it has the same name as input";
}

int main(int argc, char* argv[]) {

	td::String inputPath, outputPath;

	for (int i = 1; i < argc; ++i){

		if (*argv[i] == '-') {

			if (std::strcmp(argv[i], "--help") == 0) {
				displayHelp();
				return 0;
			}

			if (i == argc - 1)
				break;

			if (std::strcmp(argv[i], "-i") == 0) {
				inputPath = argv[++i];
				continue;
			}

			if (std::strcmp(argv[i], "-o") == 0) {
				outputPath = argv[++i];
				continue;
			}
		}
	}

	if (inputPath.isNull()) {
		std::cout << "Invalid use\n";
		displayHelp();
		return 1;
	}

	if (outputPath.isNull()) {
		int poz = inputPath.findFromRight('.');
		outputPath = inputPath.subStr(0, poz-1);
		outputPath += ".xml";
	}

	std::ifstream input;
	xml::Writer output(outputPath);
	output.startDocument();

	input.open(inputPath.c_str());

	if (!input.is_open()) {
		std::cout << "Cant open file " << inputPath;
		return 10;
	}

	if (!output.isOk()) {
		std::cout << "Cant write to path " << inputPath;
		return 11;
	}

	ModelNode mod;

	td::String commands;
	int cnt = 0;
	while (input.is_open() && !input.eof()) {
		for (int i = 0; i < 100; i++){
			if (!input.is_open() || input.eof())
				break;
			std::string line;
			std::getline(input, line);
			commands += line.c_str();
			commands += "\n";
		}

		mod.processCommands(commands);

		commands.clean();
	}

	mod.printNode(output);

	output.endDocument();
	output.close();

	std::cout << "Output written to " << outputPath;

}


