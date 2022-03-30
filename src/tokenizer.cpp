//
// Created by moss on 2/12/22.
//

#include "tokenizer.h"

const std::vector<std::string> mycelium::tokenizer::whitespace_token_strings = {" ", "	"};
const std::vector<std::string> mycelium::tokenizer::grouping_token_strings = {"(", ")", "{", "}", "[", "]"};
const std::vector<std::string> mycelium::tokenizer::keyword_token_strings = {"fn", "op", "cn"};
const std::vector<std::string> mycelium::tokenizer::oper_token_strings = {"+", "-", "*", "/", "%", "=", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "++", "--", "!", "<<", ">>", "&", "|", "~"};
const std::vector<std::vector<std::string>> mycelium::tokenizer::token_string_lists = {oper_token_strings, whitespace_token_strings, grouping_token_strings, keyword_token_strings};




void mycelium::tokenizer::tokenize() {

	std::vector<std::vector<std::string>> split_lines;

	for (auto& line : lines) {
		std::cout << "\"" << line << "\"" << "\n\n";

		split_lines.push_back(string_split(line, token_strings));
	}

	std::cout << split_lines << "\n\n";

}
