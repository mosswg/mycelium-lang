//
// Created by moss on 3/30/22.
//

#include <algorithm>
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
            std::cout << func->to_string() << '\n';
//			std::cout << func->name.string << " : ";
//			print_the_thing(func->ret);
//			std::cout << " : ";
//			print_the_thing(func->args);
//			std::cout << "\n\n\n" << std::endl;
		}

		std::cout << "ops" << std::endl;

		for (auto &oper: operators) {
            std::cout << oper->to_string() << '\n';
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
		std::shared_ptr<mycelium::parsed_token> token = parse_token();
        // Don't save null tokens
        if (token) {
            parsed_tokens.push_back(token);
        }
	}
}

std::vector<std::shared_ptr<mycelium::operator_use>> mycelium::parser::find_ops() {
    std::vector<std::shared_ptr<operator_use>> out;
    int pushed_pos;
    for (auto &op: operators) {
        if (tokenizer.current_token_index + op->pattern.pattern.size() >= tokenizer.tokens.size()) {
            continue;
        }
        pattern_match pattern;
        token current_type;
        pushed_pos = tokenizer.current_token_index;
        tokenizer.current_token_index = tokenizer.current_token_index - op->operator_offset;
        while (pattern.pattern.size() < op->pattern.pattern.size()) {
            token tk = tokenizer.get_next_token();
            if (tk.type == ttype) {
                current_type = tk;
            } else if (tk.type == word) {
                if (current_type.type != token_type::invalid) {
                    variables.push_back(variable::make_variable(tk, type::types[type::validate_type(current_type)], current_scope));
                    pattern.pattern.emplace_back(variables.back());
                } else {
                    std::shared_ptr<variable> var = get_word_variable(tk);
                    if (var.get()) {
                        pattern.pattern.emplace_back(var);
                    } else {
                        pattern.pattern.emplace_back(tk.string);
                    }
                }
            } else if (tk.type == num) {
                pattern.pattern.emplace_back(constant::make_constant(std::stoi(tk.string)));
            } else if (tk.type == string_literal) {
                pattern.pattern.emplace_back(constant::make_constant(std::make_shared<std::string>(tk.string)));
            } else if (tk.type == grouping) {
                /// FIXME: Handle Parentheses In Operator
//                tokenizer.current_token_index--;
//                std::vector<std::shared_ptr<operator_use>> ops = find_ops();
//                for (auto& new_op : ops) {
//                    pattern.pattern.emplace_back(new_op);
//                }
                pattern.pattern.emplace_back(tk.string);
            } else {
                pattern.pattern.emplace_back(tk.string);
            }
        }
        if (op->pattern.is_match(pattern)) {
            out.push_back(std::make_shared<operator_use>(operator_use(op, pattern.get_expressions())));
        }
        tokenizer.current_token_index = pushed_pos;
    }
    return out;
}

/// TODO: Handle Unknown Operators
/// TODO: Handle Parentheses
std::shared_ptr<mycelium::expression> mycelium::parser::find_ops_in(int number_of_tokens) {
    std::vector<std::shared_ptr<mycelium::operator_use>> operator_uses;

    int start_index = tokenizer.current_token_index;
    for (; tokenizer.current_token_index < start_index + number_of_tokens; tokenizer.current_token_index++) {
        std::vector<std::shared_ptr<operator_use>> found_ops = find_ops();

        if (!found_ops.empty()) {
            operator_uses.insert(operator_uses.end(), found_ops.begin(), found_ops.end());

            std::sort(operator_uses.begin(), operator_uses.end(),
                      [](const std::shared_ptr<operator_use> &a, const std::shared_ptr<operator_use> &b) -> bool {
                          return std::static_pointer_cast<operatr>(a->op)->priority >
                                 std::static_pointer_cast<operatr>(b->op)->priority;
                      });
        }
    }

    for (int i = 1; i < operator_uses.size(); i++) {
        operator_uses[i - 1]->args.back() = operator_uses[i];
    }

    if (operator_uses.empty()) {
        return {};
    }

    // Return the first operator which will recursively call the others
    return operator_uses.front();
}

std::shared_ptr<mycelium::parsed_token> mycelium::parser::parse_token() {
	mycelium::token current_token = tokenizer.get_next_token();

	mycelium::token next_token;
    if (tokenizer.has_next_token()) {
        next_token = tokenizer.get_next_token_without_increment();
    }

	switch (current_token.type) {
        case op: {
            int pushed_index = tokenizer.current_token_index;
            int count;
            for (count = 0; tokenizer.get_next_token().type != newline; count++);
            tokenizer.current_token_index = pushed_index;
            tokenizer.current_token_index--;
            return find_ops_in(count);
        }
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
        case num:
        case string_literal:
            /// TODO: Handle operators inside of function calls
            return parse_expression();
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
            // Don't return the variable declaration token because we already created it.
			return {};
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
        std::shared_ptr<mycelium::parsed_token> token = parse_token();
        // Don't save null tokens
        if (token) {
            ptokens.push_back(token);
        }
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

    if (show_debug_lines) {
        std::cout << "Parsed function: " << out->to_string() << std::endl;
    }

    if (this->tokenizer.get_next_token_without_increment().string == "{") {
        if (get_body) {
            int starting_body_location = tokenizer.current_token_index;
            int ending_body_location = tokenizer.get_ending_grouping_token_index();

            out->body = parse_tokens(starting_body_location + 1, ending_body_location - 1);
            tokenizer.current_token_index = ending_body_location + 1;
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

    pattern_match pattern;

    name = operatr::generate_name_from_context(context);

    /// TODO: Get priority
    std::shared_ptr<operatr> out = std::make_shared<mycelium::operatr>(token(), context, pattern, name, type::convert_tokens_to_types(ret), 1, scope, parent_scope);

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
                std::shared_ptr<mycelium::function> out = parse_function(true);

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

	return variable::make_variable(tokenizer.get_next_token(), type::types[variable_type], current_scope);
}

std::shared_ptr<mycelium::expression> mycelium::parser::parse_expression() {
    tokenizer.current_token_index--;
    mycelium::token next_token = tokenizer.get_next_token();

    if (tokenizer.get_next_token_without_increment().string != "(") {
        auto var = get_word_variable(next_token);
        if (!var) {
            var = get_constant(next_token);
        }
        int pushed_index = tokenizer.current_token_index;
        int count;
        for (count = 0; tokenizer.get_next_token().type != newline; count++);
        tokenizer.current_token_index = pushed_index;
        std::shared_ptr<expression> op = find_ops_in(count);
        if (op) {
            return op;
        }
        else if (var) {
            return var;
        }
        else {
            throw_error("Unknown variable \"" + next_token.string + "\"", 70001);
            return {};
        }
    }

    /// TODO: Handle parentheses in parentheses
    int close_grouping_index = tokenizer.get_ending_grouping_token_index();
    std::vector<std::shared_ptr<parsed_token>> ptokens = this->parse_tokens(tokenizer.current_token_index, close_grouping_index);

    std::vector<std::shared_ptr<expression>> fn_arguments;

    for (const auto& token : ptokens) {
        if (token->type != parsed_token_type::expr) {
            throw_error("Non-expression passed to function call", 61001);
        }

        fn_arguments.push_back(std::static_pointer_cast<expression>(token));
    }
    tokenizer.current_token_index = close_grouping_index + 1;

    std::vector<type> fn_types;

    for (auto& arg : fn_arguments) {
        fn_types.push_back(arg->get_type());
    }

    auto fn = get_word_function(next_token, fn_types);

    if (fn.get()) {
        return std::make_shared<function_call>(function_call(fn, fn_arguments));
    }
    else {
        std::string types_string;
        for (auto& type : fn_types) {
            types_string += type.name;
            if (&type != &fn_types.back()) {
                types_string += ", ";
            }
        }
        throw_error("Unknown function \"" + next_token.string + "(" + types_string + ")\"", 70002);
        return {};
    }


}

std::shared_ptr<mycelium::function> mycelium::parser::get_word_function(const mycelium::token& word, const std::vector<type>& arg_types) {
    for (const auto& fn : functions) {
        if (word.string == fn->name.string) {
            if (fn->args.size() != arg_types.size()) {
                continue;
            }

            bool has_same_args = true;
            for (int i = 0; i < arg_types.size(); i++) {
                if (fn->args[i].code != arg_types[i].code) {
                    has_same_args = false;
                    break;
                }
            }
            if (has_same_args) {
                return fn;
            }
        }
    }

    return {};
}

std::shared_ptr<mycelium::variable> mycelium::parser::get_word_variable(const mycelium::token& word) {
    return this->get_variable(word.string);
}

std::shared_ptr<mycelium::constant> mycelium::parser::get_constant(const mycelium::token& word) {
    if (word.type == num) {
        return constant::make_constant(std::stoi(word.string));
    }
    if (word.type == string_literal) {
        return constant::make_constant(std::make_shared<std::string>(std::string(word.string)));
    }
    else {
        throw_error("Invalid Constant \"" + word.string + "\"", 70003);
        return {};
    }
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

std::shared_ptr<mycelium::variable> builtin_print(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    std::cout << args[0]->get_as_string();

    return mycelium::constant::make_constant(0);
}

std::shared_ptr<mycelium::variable> builtin_println(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    if (!args.empty()) {
        std::cout << args[0]->get_as_string();
    }
    std::cout << "\n";

    return mycelium::constant::make_constant(0);
}


std::vector<std::shared_ptr<mycelium::function>> mycelium::parser::create_base_functions() {
    std::vector<std::shared_ptr<function>> out;

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("print", {}, {type::integer}, builtin_print, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("print", {}, {type::string}, builtin_print, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("println", {}, {type::integer}, builtin_println, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("println", {}, {type::string}, builtin_println, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("println", {}, {}, builtin_println, generate_new_scope()))
    );

    return out;
}


std::shared_ptr<mycelium::variable> builtin_add_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    return mycelium::constant::make_constant(args[0]->value + args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_multiply_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    return mycelium::constant::make_constant(args[0]->value * args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_append_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    return mycelium::constant::make_constant(std::make_shared<std::string>(*args[0]->str_ptr + *args[1]->str_ptr));
}

std::shared_ptr<mycelium::variable> builtin_assign_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    args[0]->value = args[1]->value;
    return args[0];
}

std::shared_ptr<mycelium::variable> builtin_assign_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    *args[0]->str_ptr = *args[1]->str_ptr;
    return args[0];
}

std::shared_ptr<mycelium::variable> builtin_plus_equals_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    args[0]->value = args[0]->value + args[1]->value;
    return args[0];
}

std::shared_ptr<mycelium::variable> builtin_minus_equals_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
    args[0]->value = args[0]->value - args[1]->value;

    return args[0];
}

std::vector<std::shared_ptr<mycelium::operatr>> mycelium::parser::create_base_operators() {
    std::vector<std::shared_ptr<operatr>> out;

    // const std::string& op_token, const mycelium::pattern_match& pattern_match, const std::string& name, const std::vector<mycelium::type>& ret, std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int scope

    /// Math
    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("+", {token("int"), token("a"), token("+"), token("int"), token("b")}, "builtin_add_int", {type::integer}, builtin_add_int, 20, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("*", {token("int"), token("a"), token("*"), token("int"), token("b")}, "builtin_multiply_int", {type::integer}, builtin_multiply_int, 10, generate_new_scope()))
    );

    /// Strings
    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("+", {token("string"), token("a"), token("+"), token("string"), token("b")}, "builtin_append_string", {type::string}, builtin_append_string, 20, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("=", {token("string"), token("a"), token("="), token("string"), token("b")}, "builtin_assign_string", {type::string}, builtin_assign_string, 99, generate_new_scope()))
    );


    /// Ints

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("=", {token("int"), token("a"), token("="), token("int"), token("b")}, "builtin_assign_int", {type::integer}, builtin_assign_int, 99, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("+=", {token("int"), token("a"), token("+="), token("int"), token("b")}, "builtin_plus_eq_int", {type::integer}, builtin_plus_equals_int, 99, generate_new_scope()))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("-=", {token("int"), token("a"), token("-="), token("int"), token("b")}, "builtin_minus_eq_int", {type::integer}, builtin_minus_equals_int, 99, generate_new_scope()))
    );

    return out;
}

mycelium::pattern_match mycelium::parser::create_from_known_variables(int num_of_tokens) {
    pattern_match out;
    token current_type;
    /// TODO: Handle two types in a row
    for (int i = tokenizer.current_token_index - 1; out.pattern.size() < num_of_tokens && i < tokenizer.tokens.size(); i++) {
        token tk = tokenizer.tokens[i];
        if (tk.type == ttype) {
            if (current_type.type != token_type::invalid) {
                throw_error("Two type name in a row in operator definition", 30004);
            }
            current_type = tk;
        }
        else if (tk.type == word) {
            if (current_type.type != token_type::invalid) {
                out.pattern.emplace_back(mycelium::variable::make_variable(tk, type::types[type::validate_type(current_type)], 0));
            }
        }
        else {
            out.pattern.emplace_back(tk.string);
        }
    }
    return out;
}
