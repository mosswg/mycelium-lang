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

void print_the_thing(const std::vector<mycelium::token>& tks) {
	for (auto& token : tks) {
		std::cout << mycelium::token::type_names[token.type] << " | " << token.string << " , ";
	}
}

void mycelium::parser::parse() {
	find_function_declarations();

	if (show_debug_lines) {
		std::cout << "funcs" << std::endl;

		for (auto &func: functions) {
			std::cout << func->name.string << " : ";
			print_the_thing(func->ret);
			std::cout << " : ";
			print_the_thing(func->args);
			std::cout << "\n\n\n" << std::endl;
		}

		std::cout << "ops" << std::endl;

		for (auto &func: operators) {
			std::cout << func.name.string << " : ";
			print_the_thing(func.ret);
			std::cout << " : ";
			print_the_thing(func.context);
			std::cout << "\n\n\n" << std::endl;
		}

		std::cout << token::oper_strings << std::endl;

	}

	// TODO: Verify that no functions have been declared more than once

//	for (auto& token : tokenizer.tokens) {
//		 std::cout << "(" << token::type_names[token.type] << ", \"" << token.string << "\")\n";
//	}

	for (int i = 0; i < tokenizer.tokens.size()-1; i++) {
		if (show_debug_lines) {
			std::cout << "parsing token: " << tokenizer.current_token_index << ": " << tokenizer.get_next_token_without_increment().string << std::endl;
		}
		parsed_tokens.push_back(parse_token());
	}
}

std::shared_ptr<mycelium::parsed_token> mycelium::parser::parse_token() {

	mycelium::token current_token = tokenizer.get_next_token();
	mycelium::token next_token = tokenizer.get_next_token_without_increment();
	switch (current_token.type) {
		case op:
			return std::shared_ptr<parsed_token>(new operator_use(current_token));
		case keyword:
//			std::cerr << "Keyword parsing not implemented\n";
			if (current_token.string == token::function_keyword) {
				return parse_function();
			}
			else if (current_token.string == token::operator_keyword) {
				return parse_operator();
			}
			else if (current_token.string == token::conditional_keyword) {
				return parse_cond();
			}
			break;
		case word:
			std::cerr << "Word parsing not implemented\n";
			break;
		case num:
			std::cerr << "Num parsing not implemented\n";
			break;
		case ttype: {
			int type = validate_type(current_token);

			std::shared_ptr<variable> var = parse_variable(type);
			current_scope->add_variable(var.get());
			if (show_debug_lines) {
				std::cout << var->type.name << ": " << var->token.string << "\n";
			}
			return var;
		}
		case invalid:
			throw_error("invalid token: " + current_token.string, 80000);
			break;
		default:
			break;
	}

	return {};
}


std::vector<std::shared_ptr<mycelium::parsed_token>> mycelium::parser::parse_tokens(int start, int end) {
    find_function_declarations();

    if (show_debug_lines) {
        std::cout << "funcs" << std::endl;

        for (auto &func: functions) {
            std::cout << func->name.string << " : ";
            print_the_thing(func->ret);
            std::cout << " : ";
            print_the_thing(func->args);
            std::cout << "\n\n\n" << std::endl;
        }

        std::cout << "ops" << std::endl;

        for (auto &func: operators) {
            std::cout << func.name.string << " : ";
            print_the_thing(func.ret);
            std::cout << " : ";
            print_the_thing(func.context);
            std::cout << "\n\n\n" << std::endl;
        }

        std::cout << token::oper_strings << std::endl;

    }

    // TODO: Verify that no functions have been declared more than once

    int starting_token_index = tokenizer.current_token_index;

    tokenizer.current_token_index = start;
    while (tokenizer.current_token_index < end) {
        if (show_debug_lines) {
            std::cout << "parsing token: " << tokenizer.current_token_index << ": " << tokenizer.get_next_token_without_increment().string << std::endl;
        }
        parsed_tokens.push_back(parse_token());
    }
}

//std::vector<std::shared_ptr<mycelium::parsed_token>> mycelium::parser::parse_func_body(int& index) {
//	temp.clear();
//
//	auto search_depth = new int[token::grouping_strings.size() % 2];
//
//	const int curly_brace_index = vector_find(token::grouping_strings,  (std::string)"{") / 2;
//
//	for (int i = index; i < tokenizer.tokens.size() - 1; i++) {
//		mycelium::token current_token = tokenizer.tokens[i];
//		mycelium::token next_token = tokenizer.tokens[i + 1];
//		if (vector_contains(token::grouping_strings, searching_for)) {
//			if (current_token.type == grouping) {
//				int group = vector_find(token::grouping_strings, current_token.string);
//				search_depth[group / 2] += group % 2 == 0 ? 1 : -1;
//		}
//	}
//
//		temp.push_back(parse_token(i));
//
//		if (show_debug_lines) {
//			std::cout << search_depth << std::endl;
//		}
//
//		if (search_depth[curly_brace_index] == 0 && next_token.string == "}") {
//			break;
//		}
//	}
//	return temp;
//}

std::shared_ptr<mycelium::function> mycelium::parser::parse_function(bool get_body) {
    mycelium::token token;
    mycelium::token name;
    std::vector<mycelium::token> ret;
    std::vector<mycelium::token> args;
    mycelium::scope* parent_scope;

    parent_scope = this->current_scope;
    token = this->tokenizer.get_next_token();

    mycelium::token next_token = this->tokenizer.get_next_non_whitespace_token();

    if (next_token.string == "<") {
        ret = this->tokenizer.get_next_non_whitespace_tokens_until(">");
        name = this->tokenizer.get_next_non_whitespace_token();
    }
    else {
        ret = {};
        name = next_token;
    }

    next_token = this->tokenizer.get_next_non_whitespace_token();

    if (next_token.string == "(") {
        args = this->tokenizer.get_next_non_whitespace_tokens_until(")");
    }
    else {
        args = {};
        // This is a bit of a hack.
        // When we have no return type and no arguments it cannot find the body without this.
        if (next_token.string == "{") {
            tokenizer.current_token_index--;
        }
    }

    std::shared_ptr<function> out = std::make_shared<mycelium::function>(token, name, ret, args, parent_scope);

    std::cout << "Parsed function: " << out->to_string() << std::endl;

    if (this->tokenizer.get_next_token_without_increment().string == "{") {
        if (get_body) {
            int starting_body_location = tokenizer.current_token_index;
            int ending_body_location = tokenizer.get_ending_grouping_token_index();

            out->body = parse_tokens(starting_body_location, ending_body_location);
        }
        else {
            tokenizer.skip_tokens_inside_grouping();
        }
    }
    else {
        mycelium::throw_error("functions must have a body", 40001);
    }

    return out;
}

std::shared_ptr<mycelium::operatr> mycelium::parser::parse_operator(bool get_body) {
    if (tokenizer.tokens[tokenizer.current_token_index + 1].string != "<") {
        throw_error(token::operator_keyword + " keyword must be followed by \'<\'", 41001);
    }

}

std::shared_ptr<mycelium::conditional> mycelium::parser::parse_cond(bool get_body) {

}

std::vector<mycelium::token> mycelium::parser::find_in_grouping(int& index, const std::string& open, const std::string& close) {
	std::vector<mycelium::token> out = {};
	int search_depth = 0;
	while (!(search_depth == 0 && tokenizer.tokens[index].string == close)) {
		if (tokenizer.tokens[index].string == open) {
			search_depth++;
		}
		else if (tokenizer.tokens[index].string == close) {
			search_depth--;
		}
		else {
			out.push_back(tokenizer.tokens[index]);
		}
		index++;
	}
	return out;
}

void mycelium::parser::find_function_declarations() {
	std::vector<std::shared_ptr<mycelium::token>> tmp = {};

	for (int i = 0; i < tokenizer.tokens.size(); i++) {
		mycelium::token current_token = tokenizer.tokens[i];
		if (current_token.type == keyword) {
			if (current_token.string == token::function_keyword) {
                tokenizer.current_token_index = i;
                std::shared_ptr<mycelium::function> out = parse_function(false);

                functions.push_back(out);

                continue;

				mycelium::token name = {};
				std::vector<mycelium::token> ret = {};
				std::vector<mycelium::token> args = {};

				int next_token_index = 1;

				for (; tokenizer.tokens[i + next_token_index].type == newline; next_token_index++);

				if (tokenizer.tokens[i + next_token_index].string == "<") {
					int search_index = i + next_token_index + 1;
					for (auto& token : find_in_grouping(search_index, "<", ">")) {
						if (show_debug_lines) {
							std::cout << "ret: " << token.string << std::endl;
						}
						ret.push_back(token);
					}
					tmp.clear();
					next_token_index = (search_index - i) + 1;
				}
				else {
					if (show_debug_lines) {
						std::cout << ":(" << std::endl;
					}
				}

				for (; tokenizer.tokens[i + next_token_index].type == newline; next_token_index++);

				if (tokenizer.tokens[i + next_token_index].type == word) {
					if (show_debug_lines) {
						std::cout << "name: " << tokenizer.tokens[i + next_token_index].string << std::endl;
					}
					name = tokenizer.tokens[i + next_token_index];
					next_token_index++;
				}
				else {
					if (show_debug_lines) {
						std::cout << tokenizer.tokens[i + next_token_index].string << std::endl;
					}
					mycelium::throw_error("functions must have a name", 40001);
				}

				for (; tokenizer.tokens[i + next_token_index].type == newline; next_token_index++);

				if (tokenizer.tokens[i + next_token_index].string == "(") {
					int search_index = i + next_token_index + 1;
					for (auto& token : find_in_grouping(search_index, "(", ")")) {
						if (show_debug_lines) {
							std::cout << "arg: " << token.string << std::endl;
						}
						args.push_back(token);
					}
					tmp.clear();
					next_token_index = (search_index - i) + 1;
				}

				for (; tokenizer.tokens[i + next_token_index].type == newline; next_token_index++);

				if (tokenizer.tokens[i + next_token_index].string == "{") {
					int search_depth = 0;
					int search_index = next_token_index + 1;
					while (!(search_depth == 0 && tokenizer.tokens[i + search_index].string == "}")) {
						if (tokenizer.tokens[i + search_index].string == "{") {
							search_depth++;
						}
						else if (tokenizer.tokens[i + search_index].string == "}") {
							search_depth--;
						}
						search_index++;
					}
					next_token_index = search_index;
				}
				else {
					mycelium::throw_error("functions must have a body", 40001);
				}

				// functions.emplace_back(current_token, name, ret, args, current_scope);

				if (show_debug_lines) {
					std::cout << std::endl;
				}

				i += next_token_index;
			}
			else if (current_token.string == token::operator_keyword) {
				std::string name;
				std::vector<mycelium::token> ret = {};
				std::vector<mycelium::token> context = {};

				int next_token_index = 1;

				for (; tokenizer.tokens[i + next_token_index].type == newline; next_token_index++);

				if (tokenizer.tokens[i + next_token_index].string == "<") {
					int search_index = i + next_token_index + 1;
					for (auto& token : find_in_grouping(search_index, "<", ">")) {
						if (show_debug_lines) {
							std::cout << "ret: " << token.string << std::endl;
						}
						ret.push_back(token);
					}
					tmp.clear();
					next_token_index = (search_index - i) + 1;
				}
				else {
					mycelium::throw_error("operator definitions must contain a context", 41001);
				}

				for (; tokenizer.tokens[i + next_token_index].type == newline; next_token_index++);

				if (tokenizer.tokens[i + next_token_index].string == "<") {
					int search_index = i + next_token_index + 1;
					for (auto& token : find_in_grouping(search_index, "<", ">")) {
						if (show_debug_lines) {
							std::cout << "context: " << token.string << std::endl;
						}
						context.push_back(token);
					}
					tmp.clear();
					next_token_index = (search_index - i) + 1;
				}
				else {
					if (show_debug_lines) {
						std::cout << "moving from ret to context" << std::endl;
					}
					for (auto& token : ret) {
						context.push_back(token);
					}
					ret.clear();
				}

				if (tokenizer.tokens[i + next_token_index].string == "{") {
					int search_depth = 0;
					int search_index = next_token_index + 1;
					while (!(search_depth == 0 && tokenizer.tokens[i + search_index].string == "}")) {
						if (tokenizer.tokens[i + search_index].string == "{") {
							search_depth++;
						}
						else if (tokenizer.tokens[i + search_index].string == "}") {
							search_depth--;
						}
						search_index++;
					}
					next_token_index = search_index;
				}

				name = operatr::generate_name_from_context(context);

				//operators.emplace_back(current_token, context, name, ret, current_scope);
				
				i += next_token_index;

				if (show_debug_lines) {
					std::cout << "\n";
				}
			}
			else if (current_token.string == token::conditional_keyword) {

			}
		}
	}
}

int mycelium::parser::validate_type(const mycelium::token& type) {
	for (int i = 0; i < type::strings.size(); i++) {
		if (type::strings[i] == type.string) {
			if (show_debug_lines) {
				std::cout << "type \"" << type.string << "\" validated" << std::endl;
			}
			return i;
		}
	}
	mycelium::throw_error("unknown type \"" + type.string + "\"", 50001);
	return -1;
}

std::shared_ptr<mycelium::variable> mycelium::parser::parse_variable(int variable_type) {

	if (show_debug_lines) {
		std::cout << "parsing variable: type: " << tokenizer.get_next_token_without_increment().string << "\tname: " << tokenizer.tokens[tokenizer.current_token_index+1].string << std::endl;
	}

	std::shared_ptr<variable> out(new variable(tokenizer.get_next_token(), type::types[variable_type], current_scope));

	return out;
}