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
	for (int i = 0; i < tokenizer.tokens.size()-1; i++) {
		mycelium::token current_token = tokenizer.tokens[i];
		std::cout << "Current token: " << current_token.string << std::endl;
		mycelium::token next_token = tokenizer.tokens[i+1];
		if (state == idle) {
			switch (current_token.type) {
				case op:
					break;
				case whitespace:
					break;
				case grouping:
					break;
				case keyword:
					if (current_token.string == "fn") {
						if (!(next_token.string == "{" || next_token.string == "(" ||next_token.string == "<" )) {
							throw_error("op keyword must be followed by '<', '(', or '{'", 3);
						}

						if (next_token.string == "<") {
							int depth = 0;

							while ((++i < tokenizer.tokens.size())) {
								std::cout << "token: \"" << tokenizer.tokens[i].string << "\" depth: " << depth << std::endl;

								if (tokenizer.tokens[i].string == "<") {
									depth++;
								}
								else if (tokenizer.tokens[i].string == ">") {
									depth--;
									if (depth == 0) {
										break;
									}
								}

								temp.push_back(tokenizer.tokens[i]);


								std::cout << temp.size() << std::endl;

								for (auto& token : temp) {
									std::cout << "(" << token.string << ")" << ", ";
								}
								std::cout << std::endl;
							}

							temp.push_back(tokenizer.tokens[i]);

							parsed_token token(current_token, temp);

							while (tokenizer.tokens[++i].type != word);

							token.context.push_back( {tokenizer.tokens[i]} );

							std::cout << "Token: " << token.token.string << "\tContext: ";
							for (int j = 0; j < token.context.size(); j++) {
								for (auto &ctx_token: token.context[j]) {
									std::cout << "\"" << ctx_token.string << "\" ";
								}
								if (j != token.context.size() - 1) {
									std::cout << "\tNext: ";
								}
							}

							i = parse_keyword_token(token, i + 1);
						}
						else {
							i = parse_keyword_token(mycelium::parsed_token(current_token), i + 1);
						}


					}
					else if (current_token.string == "op") {
						if (next_token.string != "<") {
							throw_error("op keyword must be followed by \'<\'", 3);
						}
						i = parse_keyword_token(mycelium::parsed_token(current_token), i + 1);
					}
					else if (current_token.string == "cn") {
						while (tokenizer.tokens[++i].type == whitespace);

						if (tokenizer.tokens[i].type != word) {
							throw_error("cn keyword must be followed by a word", 3);
						}

						i = parse_keyword_token(mycelium::parsed_token(current_token, { tokenizer.tokens[i] } ), i + 1);
					}
					break;
				case word:
					break;
				case num:
					break;
				case invalid:
					throw_error("invalid token: " + current_token.string, 2);
					break;
				case newline:
					break;
			}
		}
		else if (state == searching) {
		}
	}
}

int mycelium::parser::parse_keyword_token(const mycelium::parsed_token& ptoken, int index) {
	int group_string_index = vector_find(token::grouping_strings, tokenizer.tokens[index].string) + 1;

	searching_for = token::grouping_strings[group_string_index];

	temp.clear();

	parsed_tokens.push_back(ptoken);

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