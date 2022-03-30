//
// Created by moss on 2/12/22.
//

#include "tokenizer.h"

void mycelium::tokenizer::tokenize() {

	std::vector<std::vector<std::string>> split_lines;

	for (auto& line : lines) {
		split_lines.push_back(string_split(line, mycelium::token::strings));
	}

	for (auto& line : split_lines) {
		for (auto& token_string : line) {
			tokens.emplace_back(token::find_type(token_string), token_string);
		}
		tokens.emplace_back(token_type::newline, "");
	}

	for (auto& token : tokens) {
		std::cout << "(" << token::type_names[token.type] << ", \"" << token.string << "\")\n";
	}

}
