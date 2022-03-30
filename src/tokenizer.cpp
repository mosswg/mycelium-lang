//
// Created by moss on 2/12/22.
//

#include "tokenizer.h"

void mycelium::tokenizer::tokenize() {

	std::vector<std::vector<std::string>> split_lines;

	for (auto& line : lines) {
		std::cout << "\"" << line << "\"" << "\n\n";

		split_lines.push_back(string_split(line, token_strings));
	}

	std::cout << split_lines << "\n\n";

}
