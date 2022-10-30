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
                /// TODO: This allows the creation of variables in operator call chains
                /// TODO: Should we allow this?
                if (current_type.type != token_type::invalid) {
                    std::shared_ptr<mycelium::variable> var = current_scope->make_variable(tk, type::types[type::validate_type(current_type)]);
                    pattern.pattern.emplace_back(var);
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


    // Offset the position by the size of the last operator minus the distance from the start of the operator call to the first operator token. For example:
    //           1 + 2
    // We're here  ^
    // We want to take the length (3) and subtract the operator offset (1).
    //           1 + 2
    // We end here     ^
    // This means that we ignore things we already parsed
    tokenizer.current_token_index += operator_uses.back()->op->pattern.pattern.size() - operator_uses.back()->op->operator_offset;
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
    std::shared_ptr<mycelium::scope> scope = generate_new_scope();

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

    std::shared_ptr<function> out = std::make_shared<mycelium::function>(token, name, type::convert_tokens_to_types(ret),
                                                                         mycelium::variable::convert_tokens_to_variables(args), scope);

    if (show_debug_lines) {
        std::cout << "Parsed function: " << out->to_string() << std::endl;
    }

    if (this->tokenizer.get_next_token_without_increment().string == "{") {
        int starting_body_location = tokenizer.current_token_index;
        int ending_body_location = tokenizer.get_ending_grouping_token_index();
        if (get_body) {
            out->body = parse_tokens(starting_body_location + 1, ending_body_location - 1);
        }
        out->body_start_index = starting_body_location;
        out->body_end_index = ending_body_location;
        tokenizer.current_token_index = ending_body_location + 1;
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

    std::shared_ptr<mycelium::scope> scope = generate_new_scope();
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
    std::shared_ptr<operatr> out = std::make_shared<mycelium::operatr>(token(), context, pattern, name, type::convert_tokens_to_types(ret), 1, scope);

    if (this->tokenizer.get_next_token_without_increment().string == "{") {
        int starting_body_location = tokenizer.current_token_index;
        int ending_body_location = tokenizer.get_ending_grouping_token_index();
        if (get_body) {
            std::shared_ptr<mycelium::scope> pushed_scope = current_scope;
            change_scope(out->scope);
            out->body = parse_tokens(starting_body_location + 1, ending_body_location - 1);
            change_scope(pushed_scope);
        }
        out->body_start_index = starting_body_location;
        out->body_end_index = ending_body_location;
        tokenizer.current_token_index = ending_body_location + 1;
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

    int number_of_builtin_functions = functions.size();
    int number_of_builtin_operators = operators.size();
	for (int i = 0; i < tokenizer.tokens.size(); i++) {
		mycelium::token current_token = tokenizer.tokens[i];
		if (current_token.type == keyword) {
			if (current_token.string == token::function_keyword) {
                tokenizer.current_token_index = i;
                std::shared_ptr<mycelium::function> out = parse_function(false);

                functions.push_back(out);
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

    // Push the current scope
    std::shared_ptr<mycelium::scope> pushed_scope = current_scope;

    // Loop over all non builtin functions
    for (int i = number_of_builtin_functions; i < functions.size(); i++) {
        // Change to the function scope so that arguments names will be known
        change_scope(functions[i]->scope);
        // Parse the body tokens
        functions[i]->body = parse_tokens(functions[i]->body_start_index + 1, functions[i]->body_end_index - 1);
    }

    // Loop over all non builtin operators
    for (int i = number_of_builtin_operators; i < operators.size(); i++) {
        // Change to the operator scope so that arguments names will be known
        change_scope(operators[i]->scope);
        // Parse the body tokens
        operators[i]->body = parse_tokens(operators[i]->body_start_index + 1, operators[i]->body_end_index - 1);
    }

    // Pop the scope back to the old current scope
    change_scope(pushed_scope);
}

std::shared_ptr<mycelium::variable> mycelium::parser::parse_variable(int variable_type) {

	if (show_debug_lines) {
		//std::cout << "parsing variable: type: " << tokenizer.get_next_token_without_increment().string << "\tname: " << tokenizer.tokens[tokenizer.current_token_index+1].string << std::endl;
	}

	return current_scope->make_variable(tokenizer.get_next_token(), type::types[variable_type]);
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
                if (fn->args[i]->type.code != arg_types[i].code) {
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

std::shared_ptr<mycelium::scope> mycelium::parser::generate_new_scope() {
    scopes.push_back(std::make_shared<scope>(current_scope));
    return scopes.back();
}

void mycelium::parser::change_scope(const std::shared_ptr<scope>& new_scope, bool delete_previous_scope) {
    if (delete_previous_scope) {
        for (int i = 0; i < scopes.size(); i++) {
            if (scopes[i] == current_scope) {
                scopes.erase(scopes.begin() + i);
                break;
            }
        }
    }
    current_scope = new_scope;
}

std::shared_ptr<mycelium::variable> mycelium::parser::get_variable(const std::string& name) {
    return current_scope->get_variable(name);
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

    std::shared_ptr<mycelium::scope> builtin_scope = generate_new_scope();

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("print", {}, {type::integer}, builtin_print, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("print", {}, {type::string}, builtin_print, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("println", {}, {type::integer}, builtin_println, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("println", {}, {type::string}, builtin_println, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_function>(builtin_function("println", {}, {}, builtin_println, builtin_scope))
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

    std::shared_ptr<mycelium::scope> builtin_scope = generate_new_scope();

    /// Math
    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("+", {token("int"), token("a"), token("+"), token("int"), token("b")}, "builtin_add_int", {type::integer}, builtin_add_int, 20, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("*", {token("int"), token("a"), token("*"), token("int"), token("b")}, "builtin_multiply_int", {type::integer}, builtin_multiply_int, 10, builtin_scope))
    );

    /// Strings
    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("+", {token("string"), token("a"), token("+"), token("string"), token("b")}, "builtin_append_string", {type::string}, builtin_append_string, 20, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("=", {token("string"), token("a"), token("="), token("string"), token("b")}, "builtin_assign_string", {type::string}, builtin_assign_string, 99, builtin_scope))
    );


    /// Ints

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("=", {token("int"), token("a"), token("="), token("int"), token("b")}, "builtin_assign_int", {type::integer}, builtin_assign_int, 99, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("+=", {token("int"), token("a"), token("+="), token("int"), token("b")}, "builtin_plus_eq_int", {type::integer}, builtin_plus_equals_int, 99, builtin_scope))
    );

    out.push_back(
            std::make_shared<builtin_operator>(builtin_operator("-=", {token("int"), token("a"), token("-="), token("int"), token("b")}, "builtin_minus_eq_int", {type::integer}, builtin_minus_equals_int, 99, builtin_scope))
    );

    return out;
}