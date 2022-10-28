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
            std::cout << func->to_string();
//			std::cout << func->name.string << " : ";
//			print_the_thing(func->ret);
//			std::cout << " : ";
//			print_the_thing(func->args);
//			std::cout << "\n\n\n" << std::endl;
		}

		std::cout << "ops" << std::endl;

		for (auto &oper: operators) {
            std::cout << oper->to_string();
//            std::cout << func->name.string << " : ";
//            print_the_thing(func->ret);
//            std::cout << " : ";
//            print_the_thing(func->context);
//            std::cout << "\n\n\n" << std::endl;
        }

		std::cout << token::oper_strings << std::endl;

	}

	// TODO: Verify that no functions have been declared more than once

//	for (auto& token : tokenizer.tokens) {
//		 std::cout << "(" << token::type_names[token.type] << ", \"" << token.string << "\")\n";
//	}

	while (tokenizer.has_next_token()) {
		if (show_debug_lines) {
			std::cout << "parsing token: " << tokenizer.current_token_index << ": " << tokenizer.get_next_token_without_increment().string << std::endl;
		}
		parsed_tokens.push_back(parse_token());
        if (current_finding_operator_index != -1) {
            for (auto& op : operators) {
                int check_index = parsed_tokens.size() - op->pattern.pattern.size();
                if (op->pattern.is_match_at_index(parsed_tokens, check_index)) {
                    parsed_tokens.push_back(operator_use::create_operator_call(*op, parsed_tokens, current_finding_operator_index, check_index));
                    current_finding_operator_index = -1;
                }
            }
        }
	}
}

std::shared_ptr<mycelium::parsed_token> mycelium::parser::parse_token() {

	mycelium::token current_token = tokenizer.get_next_token();

	mycelium::token next_token;
    if (tokenizer.has_next_token()) {
        next_token = tokenizer.get_next_token_without_increment();
    }

	switch (current_token.type) {
		case op:
            current_finding_operator_index = parsed_tokens.size();
		case keyword:
            if (next_token.type == token_type::invalid) {
                throw_error("Keyword with nothing following found", 62002);
            }

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
            return parse_word();
		case num:
            return std::make_shared<constant>(mycelium::constant(type::integer, std::stoi(current_token.string)));
		case ttype: {
			int type = type::validate_type(current_token);

            if (next_token.type == token_type::invalid) {
                throw_error("Type with no following variable name found", 62001);
            }

			std::shared_ptr<variable> var = parse_variable(type);
			variables.push_back(var);
			if (show_debug_lines) {
				std::cout << "Got variable: " << var->type.name << ": " << var->token.string << "\n";
			}
			return var;
		}
		case invalid:
            throw_error("invalid token: \"" + current_token.string + '"', 80000);
			break;
		default:
			break;
	}

	return {};
}


std::vector<std::shared_ptr<mycelium::parsed_token>> mycelium::parser::parse_tokens(int start, int end) {
    // find_function_declarations();
    int starting_token_index = tokenizer.current_token_index;

    tokenizer.current_token_index = start;

    std::vector<std::shared_ptr<parsed_token>> ptokens;

    while (tokenizer.current_token_index < end) {
        if (show_debug_lines) {
            std::cout << "parsing token: " << tokenizer.current_token_index << ": " << tokenizer.get_next_token_without_increment().string << std::endl;
        }
        ptokens.push_back(parse_token());
    }

    tokenizer.current_token_index = starting_token_index;

    return ptokens;
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
    int scope = generate_new_scope();
    int parent_scope;

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

    std::shared_ptr<function> out = std::make_shared<mycelium::function>(token, name, type::convert_tokens_to_types(ret), type::convert_tokens_to_types(args), scope, parent_scope);

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
    /// TODO: Handle '<' and '>' in operator definition
    if (tokenizer.tokens[tokenizer.current_token_index + 1].string != "<") {
        throw_error(token::operator_keyword + " keyword must be followed by \'<\'", 41001);
    }

    std::string name;
    std::vector<mycelium::token> ret = {};
    std::vector<mycelium::token> context = {};

    int scope = generate_new_scope();
    int parent_scope;

    parent_scope = this->current_scope;
    token name_token;

    token next_token;

    // Ignore whitespace
    for (; (next_token = tokenizer.get_next_token()).type == whitespace;);

    if (next_token.string == "<") {
        tokenizer.current_token_index++;
        for (auto& token : find_in_grouping(tokenizer.current_token_index, "<", ">")) {
            if (show_debug_lines) {
                std::cout << "ret: " << token.string << std::endl;
            }
            ret.push_back(token);
        }
    }
    else {
        mycelium::throw_error("operator definitions must contain a context", 41001);
    }

    // Ignore whitespace
    for (; (next_token = tokenizer.get_next_token()).type == whitespace;);

    if (next_token.string == "<") {
        tokenizer.current_token_index++;
        for (auto& token : find_in_grouping(tokenizer.current_token_index, "<", ">")) {
            if (show_debug_lines) {
                std::cout << "context: " << token.string << std::endl;
            }
            context.push_back(token);
        }
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

    // Ignore whitespace
    for (; (next_token = tokenizer.get_next_token()).type == whitespace;);

    parser pars((mycelium::tokenizer(context)));

    pars.parse();

    name = operatr::generate_name_from_context(context);

    std::shared_ptr<operatr> out = std::make_shared<mycelium::operatr>(token(), context, pattern_match(pars.parsed_tokens), name, type::convert_tokens_to_types(ret), scope, parent_scope);

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

    if (show_debug_lines) {
        std::cout << "\n";
    }

    return out;
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
                tokenizer.current_token_index = i;
                std::shared_ptr<operatr> out = parse_operator(false);

                operators.push_back(out);
			}
			else if (current_token.string == token::conditional_keyword) {

			}
		}
	}
}

std::shared_ptr<mycelium::variable> mycelium::parser::parse_variable(int variable_type) {

	if (show_debug_lines) {
		//std::cout << "parsing variable: type: " << tokenizer.get_next_token_without_increment().string << "\tname: " << tokenizer.tokens[tokenizer.current_token_index+1].string << std::endl;
	}

	std::shared_ptr<variable> out(new variable(tokenizer.get_next_token(), type::types[variable_type], current_scope));

	return out;
}

std::shared_ptr<mycelium::parsed_token> mycelium::parser::parse_word() {
    tokenizer.current_token_index--;
    mycelium::token next_token = tokenizer.get_next_token();

    auto fn = get_word_function(next_token);
     auto var = get_word_variable(next_token);



    if (fn.get()) {
        if (tokenizer.get_next_token_without_increment().string != "(") {
            return std::make_shared<variable>(variable(next_token, type::func, current_scope, (long)fn.get()));
        }

        int close_grouping_index = tokenizer.get_ending_grouping_token_index();

        std::vector<std::shared_ptr<parsed_token>> ptokens = this->parse_tokens(tokenizer.current_token_index, close_grouping_index);

        std::vector<variable> args;

        for (const auto& token : ptokens) {
            if (token->type != parsed_token_type::var) {
                throw_error("Non-value passed to function call", 61001);
            }

            args.push_back(*(variable *) token.get());
        }
        tokenizer.current_token_index = close_grouping_index + 1;

        return std::make_shared<function_call>(function_call(fn, args));
    }
    else if (var.get()) {
        return var;
    }
    else {
        throw_error("Unknown word \"" + next_token.string + "\"", 70001);
        return {};
    }

}

std::shared_ptr<mycelium::function> mycelium::parser::get_word_function(const mycelium::token& word) {
    for (const auto& fn : functions) {
        if (word.string == fn->name.string) {
            return fn;
        }
    }

    return {};
}

std::shared_ptr<mycelium::variable> mycelium::parser::get_word_variable(const mycelium::token& word) {
    return this->get_variable(word.string);
}

int mycelium::parser::generate_new_scope() {
    for (int i = 1; i < INT32_MAX; i++) {
        if (!scopes[i]) {
            scopes[i] = true;
            return i;
        }
    }
    mycelium::throw_error("Could not create scope", 11001);
    return -1;
}

void mycelium::parser::change_scope(int new_scope, bool delete_previous_scope) {
    if (delete_previous_scope) {
        scopes[current_scope] = false;
        for (int i = 0; i < variables.size(); i++) {
            if (variables[i]->parent_scope == current_scope) {
                variables.erase(variables.begin() + i);
                i--;
            }
        }
    }
}

std::shared_ptr<mycelium::variable> mycelium::parser::get_variable(const std::string& name) {
    /// TODO: Make this handle scopes.
    for (const auto& var : variables) {
        if (var->token.string == name) {
            if (var->parent_scope != -1) {
                return var;
            }
        }
    }
    return {};
}

void mycelium::parser::execute() {
    for (auto& pt : this->parsed_tokens) {
        if (pt) {
            pt->execute();
        }
    }
}

void builtin_print(std::vector<mycelium::variable>& args) {
    std::cout << args[0].get_as_string();
}

void builtin_println(std::vector<mycelium::variable>& args) {
    if (args.empty()) {
        std::cout << "\n";
    }
    else {
        std::cout << args[0].get_as_string();
    }
}


std::vector<std::shared_ptr<mycelium::function>> mycelium::parser::create_base_functions() {
    std::vector<std::shared_ptr<function>> out;

    int new_scope = generate_new_scope();

    std::shared_ptr<builtin_function> print = std::make_shared<builtin_function>(builtin_function("print", {}, {type::integer}, builtin_print, new_scope, 0));

    std::shared_ptr<builtin_function> println_int = std::make_shared<builtin_function>(builtin_function("println", {}, {type::integer}, builtin_println, new_scope, 0));

    std::shared_ptr<builtin_function> println_void = std::make_shared<builtin_function>(builtin_function("println", {}, {}, builtin_println, new_scope, 0));


    out.push_back(print);

    out.push_back(println_int);

    out.push_back(println_void);

    return out;
}

std::vector<std::shared_ptr<mycelium::operatr>> mycelium::parser::create_base_operators() {
    return std::vector<std::shared_ptr<mycelium::operatr>>();
}
