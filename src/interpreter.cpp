//
// Created by moss on 2/11/22.
//

#include <fstream>
#include "base.h"
#include "tokenizer.h"
#include "parser.h"


int main(int argc, char** argv) {

	if (argc == 1) {
		mycelium::throw_error("No File Given", 1);
	}

	std::ifstream in_file(argv[1]);

	std::string line;

	std::vector<std::string> program;

	while (true) {
		getline(in_file, line);
		program.emplace_back(line);
		if (!in_file.good()) {
			break;
		}
	}

	mycelium::show_debug_lines = true;

	mycelium::initialize_static_values();

	mycelium::tokenizer tokenizer(program);

	tokenizer.tokenize();

	mycelium::parser parser(tokenizer);

	parser.parse();

	return 0;
}