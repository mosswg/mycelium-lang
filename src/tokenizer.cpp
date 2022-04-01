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
			token_type type = token::find_type(token_string);
			if (type != whitespace) {
				tokens.emplace_back(type, token_string);
			}
		}
		tokens.emplace_back(token_type::newline, "");
	}
}
