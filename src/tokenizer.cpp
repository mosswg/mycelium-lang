//
// Created by moss on 2/12/22.
//

#include "tokenizer.h"

void mycelium::tokenizer::tokenize() {

	std::cout << lines << std::endl;

	std::vector<std::vector<std::string>> split_lines;

	std::vector<std::string> split_groups = {};

	split_groups.insert(split_groups.end(), token::whitespace_strings.begin(), token::whitespace_strings.end());
	split_groups.insert(split_groups.end(), token::grouping_strings.begin(), token::grouping_strings.end());

	for (auto& line : lines) {
		split_lines.push_back(string_split(line, split_groups));
	}

	for (auto& line : split_lines) {
		for (int i = 0; i < line.size(); i++) {
			std::string token_string = line[i];
			token_type type = token::find_type(token_string);
			if (type != whitespace) {
				if (token_string == "<" && line[i + 1] == "<") {
					tokens.emplace_back(op, "<<");
					i++;
				}
				else if (token_string == ">" && line[i + 1] == ">") {
					tokens.emplace_back(op, ">>");
					i++;
				}
				else {
					tokens.emplace_back(type, token_string);
				}
			}
		}
		if (tokens.back().type != newline) {
			tokens.emplace_back(newline, "");
		}
	}
}
