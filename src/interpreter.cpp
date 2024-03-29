//
// Created by moss on 2/11/22.
//

#include <fstream>
#include "base.h"
#include "tokenizer.h"
#include "parser.h"


int main(int argc, char** argv) {

	if (argc == 1) {
		mycelium::throw_error("No File Given");
	}

	std::ifstream in_file(argv[1]);

	if (!in_file.is_open()) {
		mycelium::throw_error("Could not open " + std::string(argv[1]));
	}

	std::vector<std::string> args;
	for (int i = 2; i < argc; i++) {
		args.push_back(argv[i]);
	}

	std::string line;

	std::vector<std::string> program;

	while (true) {
		std::getline(in_file, line);
		program.emplace_back(line);
		if (!in_file.good()) {
			break;
		}
	}

	mycelium::initialize_static_values();

	mycelium::tokenizer tokenizer(program);

	tokenizer.tokenize();

	if (mycelium::show_debug_lines) {
		std::cout << "{";
		for (auto &t: tokenizer.tokens) {
			std::cout << t.string << ", ";
		}
		std::cout << "}\n";
	}

	mycelium::parser parser(tokenizer, args);

	if (mycelium::show_debug_lines) {
		std::cout << "Parsing Tokens:\n";
	}

	parser.parse();

	if (mycelium::show_debug_lines) {
		std::cout << "Parsed Tokens:\n";
		for (auto &pt: parser.parsed_tokens) {
			if (pt.get()) {
				std::cout << pt->to_string() << "\n";
			}
		}
	}

	if (mycelium::show_debug_lines) {
		std::cout << "Executing Code:\n\n";
	}

	parser.execute();

	return 0;
}
