//
// Created by moss on 3/30/22.
//

#include "parser.h"

template <typename T>
int mycelium::vector_find(const std::vector<T>& vec, const T& search) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i] == search) {
			return i;
		}
	}
	return -1;
}

void mycelium::parser::parse() {
	for (auto& token : tokenizer.tokens) {
		std::cout << "(" << token::type_names[token.type] << ", \"" << token.string << "\")\n";
	}
	for (int i = 0; i < tokenizer.tokens.size()-1; i++) {}
}

mycelium::parsed_token mycelium::parser::parse_token(int& index) {

	mycelium::token current_token = tokenizer.tokens[index];
	std::cout << "Current token: " << current_token.string << std::endl;
	mycelium::token next_token = tokenizer.tokens[index+1];
	if (state == idle) {
		switch (current_token.type) {
			case op:
				break;
			case keyword:
				if (current_token.string == "fn") {
					return parse_function(index);
				}
				else if (current_token.string == "op") {
					if (next_token.string != "<") {
						throw_error("op keyword must be followed by \'<\'", 3);
					}
					return parse_operator(index);
				}
				else if (current_token.string == "cn") {
					return parse_cond(index);
				}
				break;
			case word:
				break;
			case num:
				break;
			case ttype:

				break;
			case invalid:
				throw_error("invalid token: " + current_token.string, 2);
				break;
			default:
				break;
		}
	}
}

std::vector<mycelium::parsed_token> mycelium::parser::parse_func_body(int& index) {
	temp.clear();

	auto search_depth = new int[token::grouping_strings.size() % 2];

	const int curly_brace_index = vector_find(token::grouping_strings,  (std::string)"{") / 2;

	for (int i = index; i < tokenizer.tokens.size() - 1; i++) {
		mycelium::token current_token = tokenizer.tokens[i];
		mycelium::token next_token = tokenizer.tokens[i + 1];
		if (vector_contains(token::grouping_strings, searching_for)) {
			if (current_token.type == grouping) {
				int group = vector_find(token::grouping_strings, current_token.string);
				search_depth[group / 2] += group % 2 == 0 ? 1 : -1;
		}
	}

		temp.push_back(parse_token(i));

		std::cout << search_depth << std::endl;

		if (search_depth[curly_brace_index] == 0 && next_token.string == "}") {
			return temp;
		}
	}
}

mycelium::function mycelium::parser::parse_function(int& index) {
	temp = {};
	int idx = index;
	int search_depth = 0;
	if (tokenizer.tokens[idx + 1].type == newline) {
		while (tokenizer.tokens[idx++].type == newline);
	}
	else {
		idx++;
	}

	temp.push_back(parse_token(idx));

	if (tokenizer.tokens[idx].string == "{") {
		search_depth++;
		searching_for = "}";
	} else {
		throw_error("keyword definitions should be followed by '(' or '{'", 3);
	}



}

mycelium::function mycelium::parser::parse_operator(int& index) {

}

mycelium::function mycelium::parser::parse_cond(int& index) {

}

int mycelium::parser::parse_keyword_token(const mycelium::parsed_token& ptoken, int index) {
	int group_string_index = vector_find(token::grouping_strings, tokenizer.tokens[index].string) + 1;

	searching_for = token::grouping_strings[group_string_index];

	temp.clear();

	int search_depth = 0;

	for (int i = index; i < tokenizer.tokens.size() - 1; i++) {
		mycelium::token current_token = tokenizer.tokens[i];
		mycelium::token next_token = tokenizer.tokens[i + 1];
		std::cout << "\ns: " << searching_for << std::endl;
		std::cout << "c: " << current_token.string << std::endl;
		std::cout << "d: " << search_depth << std::endl;
		if (vector_contains(token::grouping_strings, searching_for)) {
			if (current_token.type == grouping) {
				if (!(vector_find(token::grouping_strings, current_token.string) % 2)) {
					search_depth++;
				} else {
					search_depth--;
				}
			}
		}

		if (current_token.type != newline) {
			temp.push_back(current_token);
		}

		if (search_depth == 0 && current_token.string == searching_for) {
			parsed_tokens.back().context.emplace_back(temp);

			for (auto &token: parsed_tokens) {
				std::cout << "Token: " << token.token.string << "\tContext: ";
				for (int j = 0; j < token.context.size(); j++) {
					for (auto &ctx_token: token.context[j]) {
						std::cout << "\"" << ctx_token.string << "\" ";
					}
					if (j != token.context.size() - 1) {
						std::cout << "\tNext: ";
					}
				}
				std::cout << std::endl;
			}

			if (searching_for == "}") {
				return i;
			}
			state = searching;
			search_type = next_token;
			temp = {};
			int idx = i;
			if (next_token.type == newline) {
				while (tokenizer.tokens[idx++].type == newline);
			}
			else {
				idx++;
			}

			temp.push_back(tokenizer.tokens[idx]);

			if (tokenizer.tokens[idx].string == "(") {
				search_depth++;
				searching_for = ")";
			} else if (tokenizer.tokens[idx].string == "{") {
				search_depth++;
				searching_for = "}";
			} else {
				throw_error("keyword definitions should be followed by '(' or '{'", 3);
			}
			i = idx;
		}
	}

	return (int)tokenizer.tokens.size();
}