//
// Created by moss on 3/30/22.
//

#include <algorithm>
#include "parser.h"


bool spam_debug_output = mycelium::show_debug_lines && true;

std::vector<std::string> mycelium::parser::program_args = {};

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

std::string tokens_to_string(const std::vector<mycelium::token>& tks) {

	std::string out = "{";
	for (const auto& tk : tks) {
		out += tk.string;
		if (&tk != &tks.back()) {
			out += ", ";
		}
	}
	out += "}";
	return out;
}

void print_tokens(const std::vector<mycelium::token>& tks) {
	std::cout << "{";
	for (const auto& tk : tks) {
		std::cout << tk.string;
		if (&tk != &tks.back()) {
			std::cout << ", ";
		}
	}
	std::cout << "}";
}


void mycelium::parser::throw_error(const std::string &error, const token& tk) {
	if (current_parsing_function) {
		std::cerr << "ERROR in " << current_parsing_function->to_string() << " on line " << tk.line << ": "  << error << std::endl;
	}
	else {
		std::cerr << "ERROR on line " << tk.line << ": "  << error << std::endl;
	}
	exit(1);
}

void mycelium::parser::warn(const std::string& warning, const token& tk) {
	std::cout << "WARNING on line " << tk.line << ": " << warning << std::endl;
}


void mycelium::parser::parse() {
	find_function_declarations();

	if (show_debug_lines) {
		std::cout << "funcs" << std::endl;

		for (auto &func: functions) {
			std::cout << func->to_string() << '\n';
		}

		std::cout << "opers" << std::endl;

		for (auto &oper: operators) {
			std::cout << oper->to_string() << '\n';
		}

		std::cout << "conds" << std::endl;

		for (auto &cond: conditionals) {
			std::cout << cond->to_string() << '\n';
		}


		std::cout << token::oper_strings << std::endl;
	}

	// TODO: Verify that no functions have been declared more than once

	while (tokenizer.has_next_token()) {
		if (show_debug_lines) {
			std::cout << "parsing token: " << tokenizer.current_token_index << ": " << tokenizer.get_next_token_without_increment().string << std::endl;
		}
		std::shared_ptr<mycelium::parsed_token> token = parse_token(parsed_tokens, tokenizer.tokens, tokenizer.current_token_index);
		// Don't save null tokens
		if (token) {
			parsed_tokens.push_back(token);
		}
	}

	// Push the current scope
	std::shared_ptr<mycelium::scope> pushed_scope = current_scope;

	// Loop over all the new functions and generate their body code. Doing things in this order allows for function
	// definitions to be in any order without forward declarations in the file
	for (size_t i = number_of_builtin_functions; i < functions.size(); i++) {
		// Change to the function scope so that arguments names will be known
		change_scope(functions[i]->scope);

		// Change the current parsing function for returns
		current_parsing_function = functions[i];

		// Parse the body tokens
		functions[i]->body = parse_tokens(functions[i]->body_start_index + 1, functions[i]->body_end_index - 1);
	}

	// Loop over all non builtin operators
	for (size_t i = number_of_builtin_operators; i < operators.size(); i++) {
		// Change to the operator scope so that arguments names will be known
		change_scope(operators[i]->scope);

		// Change the current parsing function for returns
		current_parsing_function = operators[i];

		// Parse the body tokens
		operators[i]->body = parse_tokens(operators[i]->body_start_index + 1, operators[i]->body_end_index - 1);
	}

	current_parsing_function = {};

	// Pop the scope back to the old current scope
	change_scope(pushed_scope);
}

std::shared_ptr<mycelium::pattern_token> mycelium::parser::get_pattern_token(const std::vector<mycelium::token>& tks, size_t& index) {
	token tk = tks[index++];
	if (tk.type == ttype) {
			warn("Using type as operator: " + tk.string, tk);
			return make_pattern_token(tk.string);
	} else if (tk.type == word) {
		std::shared_ptr<mycelium::variable> var = get_word_variable(tk);
		if (var.get()) {
			return make_pattern_token(var);
		} else {
			return make_pattern_token(tk.string);
		}
	} else if (tk.type == num) {
		return make_pattern_token(constant::make_constant(std::stoi(tk.string)));
	} else if (tk.type == string_literal) {
		return make_pattern_token(constant::make_constant(tk.string));
	} else if (tk.type == boolean_literal) {
		if (tk.string == token::boolean_true) {
			return make_pattern_token(constant::make_bool_constant(true));
		}
		else if (tk.string == token::boolean_false) {
			return make_pattern_token(constant::make_bool_constant(false));
		}
		else {
			throw_error("Unknown boolean " + tk.string, tk);
		}
	} else if (tk.type == grouping) {
		/// FIXME: Handle Parentheses In Operator
		return make_pattern_token(tk.string);
	} else {
		return make_pattern_token(tk.string);
	}
}

mycelium::pattern_match mycelium::parser::get_pattern_from_tokens(size_t number_of_tokens) {
	pattern_match pattern;
	token current_type;
	while (pattern.pattern.size() < number_of_tokens) {
		auto new_token = get_pattern_token(tokenizer.tokens, tokenizer.current_token_index);
		/*
		if (!new_token.is_expression && new_token.oper.empty()) {
			// Return when we have an empty token because that means we have an invalid token e.g. a type
			return {};
		}
		*/
		pattern.pattern.emplace_back(new_token);
	}

	return pattern;
}

mycelium::pattern_match mycelium::parser::get_pattern_from_tokens(size_t start_index, size_t end_index) {
	pattern_match pattern;
	for (size_t current_token_index = start_index; current_token_index < end_index; current_token_index++) {
		pattern.pattern.push_back(get_pattern_token(tokenizer.tokens, current_token_index));
	}

	return pattern;
}

mycelium::pattern_match mycelium::parser::get_pattern_from_tokens(const std::vector<token>& tks) {
	pattern_match pattern;
	for (size_t i = 0; i < tks.size();) {
		std::cout << "getting pt from " << tks[i].string << "\n";
		auto pt = get_pattern_token(tks, i);
		std::cout << "got pt: " << pt->to_string() << "\n";
		pattern.pattern.push_back(pt);
	}

	return pattern;
}

std::shared_ptr<mycelium::expression> mycelium::parser::get_expression_from_single_token(const token& tk) {
	for (auto& op : operators) {
		pattern_match op_use_pattern = this->generate_pattern_from_function(op, tk);
		if (op->args.is_match(op_use_pattern)) {
			return std::make_shared<operator_use>(op, op_use_pattern.get_expressions());
		}
	}

	/// If no matching operators are found we return an empty expression.
	return {};
}


std::shared_ptr<mycelium::expression> mycelium::parser::get_expression_from_tokens(const std::vector<token>& tks) {

	if (spam_debug_output) {
		std::cout << "Getting Expression From: " << tokens_to_string(tks) << "\n";
	}

	if (tks.empty()) {
		/// If we have no token then warn and return nothing indicating the expression is invalid
		warn("No tokens passed to get_expression_from_tokens", token());
		return {};
	}

	if (tks.size() == 1) {
		/// If all we have is a single token we want to try to match that to a pattern or return that as either a variable or nothing
		/// or if we have a type we want to still treat it as a variable, just a new one.
		size_t tmp = 0;
		auto pt = get_pattern_token(tks, tmp);
		if (pt->type == pattern_token_type::pt_expr) {
			return (std::static_pointer_cast<pattern_tokens::variable>(pt))->value;
		}
		return get_expression_from_single_token(tks[0]);
	}

	if (tks.front().string == "(" && tks.back().string == ")") {
		int search_depth = 0;
		std::string opening_grouping = "(";
		std::string closing_grouping = ")";

		std::vector<token> out;
		size_t current_token_index = 1;
		while (!(search_depth == 0 && tks[current_token_index].string == closing_grouping) && current_token_index < tks.size()) {
			if (tks[current_token_index].string == opening_grouping) {
				search_depth++;
			}
			else if (tks[current_token_index].string == closing_grouping) {
				search_depth--;
			}
			out.push_back(tks[current_token_index]);
			current_token_index++;
		}

		return get_expression_from_tokens(out);
	}

	/// TODO: This should support more than just the second token. E.G. 'list_of_functions[0]()' should be supported. Although we need to do a lot of work before we can have lists of functions
	if (tks[1].string == "(" && tks.back().string == ")") {
		size_t tmp = 0;
		return get_function(tks, tmp);
	}

	std::vector<std::shared_ptr<operator_use>> matched_operators;
	for (auto& op : operators) {
		pattern_match op_use_pattern = this->generate_pattern_from_function(op, tks);
		if (!op_use_pattern.pattern.empty()) {
			if (mycelium::show_debug_lines) {
				std::cout << "oup: " << op_use_pattern.to_string() << " from " << op->to_string() << "\n";
			}
		}
		if (op->args.is_match(op_use_pattern)) {
			auto gexp = op_use_pattern.get_expressions();
			matched_operators.push_back(std::make_shared<operator_use>(op, gexp));
		}
	}

	if (!matched_operators.empty()) {
		/// Note that we find the highest proirity here althogh the smallest priority is computed first. E.G. int + int has priority of 50 while int = int has priority of 99.
		/// This is because we do a reverse search. We find the highest priority as the first search element then the next highest and so on. But we execute the last found operator first.
		/// For example, 'a = b + c' we find "int = int" and then we compress "b + c" into int and so the result is "a = (b + c)". Hope this makes sense.
		auto& highest_priority = matched_operators.front();

		/// Find the matched operator with the highest priority. This ensures we dont do silly things like treat "a = b + c" as "(a = b) + c" which was happening before.
		/// TODO: Allow users to define proirity
		for (auto& oper : matched_operators) {
			if (oper->op->priority > highest_priority->op->priority) {
				highest_priority = oper;
			}
		}

		if (spam_debug_output) {
			std::cout << "Using: " << highest_priority->to_string() << "\n";
			auto oexp = highest_priority->args;
			for (const auto& exp : oexp) {
				std::cout << "\texp: " << exp->to_string() << "\n";
			}
		}

		return highest_priority;
	}

	auto obj_func = get_object_function(tks, 0);
	if (obj_func.get()) {
		return obj_func;
	}

	if (tks.size() == 2 && tks[0].type == ttype) {
		return std::make_shared<variable_creation>(tks[1], type::types[type::validate_type(tks[0])], current_scope);
	}

	return {};
}

std::shared_ptr<mycelium::parsed_token> mycelium::parser::parse_token(const std::vector<std::shared_ptr<parsed_token>>& previous_tokens, const std::vector<token>& tokens, size_t& index) {
	mycelium::token current_token = tokens[index++];

	mycelium::token next_token;
	if (index < tokens.size()) {
		next_token = tokens[index];
	}


	/// If the last thing we parsed was a conditional call ensure that we have a body
	if (!previous_tokens.empty()) {
		if (previous_tokens.back()->type == parsed_token_type::cond_call && (current_token.type != token_type::grouping || current_token.string != "{")) {
			std::shared_ptr<conditional_call> cn = std::static_pointer_cast<conditional_call>(previous_tokens.back());
			if (!cn->has_body) {
				throw_error("Conditional calls must have a body", previous_tokens.back()->token);
			}
		}
	}


	// Check for operator on every token.
	std::vector<token> remaining_line_tokens = get_tokens_until_newline(tokens, index);

	// Removing open curly because we want it to still be with the main tokens when we find the body of the conditional. (we also don't want to confuse the function that gets expressions (it's a bit finicky but don't tell it I said that))
	if (!remaining_line_tokens.empty() && remaining_line_tokens.back().string == "{") {
		remaining_line_tokens.pop_back();
	}

	// Don't check if there's nothing to check!
	if (!remaining_line_tokens.empty()) {
		if (spam_debug_output) {
			std::cout << "checking for ops: " << token_vec_to_string(remaining_line_tokens) << "\n";
		}
		std::shared_ptr<expression> op = get_expression_from_tokens(remaining_line_tokens);

		if (op) {
			index += remaining_line_tokens.size() - 1;
			return op;
		}
	}


	switch (current_token.type) {
		case token_type::op:
			throw_error("Unknown Operator: " + current_token.string, current_token);
		case keyword:
			if (current_token.string == token::return_keyword) {
				return parse_return(tokens, index);
			}

			if (next_token.type == token_type::invalid) {
				throw_error("Keyword with nothing following found", current_token);
			}

			skip_function_definition(tokens, index);
			break;
		case word:
		case num:
		case boolean_literal:
		case string_literal:
			return parse_expression(tokens, index);
		case ttype: {
			warn("Misplaced Type Token", current_token);
			std::shared_ptr<expression> op = get_expression_from_tokens(remaining_line_tokens);

			if (op) {
				skip_to_newline(tokens, index);
				return op;
			}

			/// If we don't find an operator after the variable declaration skip checking the name of the variable
			index++;

			/// Don't return the variable declaration token because we already created the variable.
			return {};
		}
		case grouping: {
			if (current_token.string == "(") {
				index--;
				std::vector<token> tokens_inside_grouping = get_tokens_in_parentheses(tokens, index);
				if (spam_debug_output) {
					std::cout << "checking for ops in grouping: " << token_vec_to_string(tokens_inside_grouping) << "\n";
				}
				std::shared_ptr<expression> op = get_expression_from_tokens(tokens_inside_grouping);

				for (auto& tk : tokens_inside_grouping) {
					std::cout << "\"" << tk.string << "\" ";
				}

				if (op) {
					skip_to_newline(tokens, index);
					return op;
				}
				else {
					throw_error("Unknown Value Inside Grouping", current_token);
				}
			}
			else if (current_token.string == "{") {
				if (previous_tokens.back()->type == parsed_token_type::cond_call) {
					index--;
					std::shared_ptr<conditional_call> cn = std::static_pointer_cast<conditional_call>(previous_tokens.back());
					size_t starting_body_index, ending_body_index;

					auto body = get_tokens_in_curlies(tokens, index, starting_body_index, ending_body_index);

					if (show_debug_lines) {
						std::cout << "cond body: " << tokens_to_string(body) << "\n";
					}

					std::shared_ptr<mycelium::scope> pushed_scope = current_scope;
					change_scope(cn->body_scope);
					std::vector<std::shared_ptr<parsed_token>> call_body = parse_tokens(body);
					change_scope(pushed_scope);
					index = ending_body_index + 1;

					/// Create an anonymous function with body of the conditional statement and pass that as the first argument
					std::shared_ptr<expression> cn_call_expressions(function::make_anonymous_function(call_body, {}, {}, cn->body_scope));
					cn->args.insert(cn->args.begin(), cn_call_expressions);
					cn->has_body = true;

					index = ending_body_index + 1;
					return {};
				}
			}
			else {
				throw_error("Why the hell is there a '" + current_token.string + "' there?", current_token);
			}
		}
	case invalid:
			throw_error("invalid token: \"" + current_token.string + '"', current_token);
			break;
		default:
			break;
	}

	return {};
}


std::vector<std::shared_ptr<mycelium::parsed_token>> mycelium::parser::parse_tokens(const std::vector<token>& tokens) {
	// find_function_declarations();
	std::vector<std::shared_ptr<parsed_token>> ptokens;

	for (size_t index = 0; index < tokens.size();) {
		if (show_debug_lines) {
			std::cout << "parsing token: " << index << ": " << tokens[index].string << std::endl;
		}
		std::shared_ptr<mycelium::parsed_token> parsed_token = parse_token(ptokens, tokens, index);
		// Don't save null tokens
		if (parsed_token) {
			ptokens.push_back(parsed_token);
		}
	}

	return ptokens;
}

std::vector<std::shared_ptr<mycelium::parsed_token>> mycelium::parser::parse_tokens(size_t start, size_t end) {
	// find_function_declarations();
	size_t starting_token_index = tokenizer.current_token_index;

	tokenizer.current_token_index = start;

	std::vector<std::shared_ptr<parsed_token>> ptokens;

	while (tokenizer.current_token_index < end) {
		if (show_debug_lines) {
			std::cout << "parsing token: " << tokenizer.current_token_index << ": " << tokenizer.get_next_token_without_increment().string << std::endl;
		}
		std::shared_ptr<mycelium::parsed_token> token = parse_token(ptokens, tokenizer.tokens, tokenizer.current_token_index);
		// Don't save null tokens
		if (token) {
			ptokens.push_back(token);
		}
	}

	tokenizer.current_token_index = starting_token_index;

	return ptokens;
}

std::shared_ptr<mycelium::function> mycelium::parser::parse_function(bool get_body) {
	mycelium::token token;
	mycelium::token name;
	std::vector<mycelium::token> ret;
	std::vector<mycelium::token> args;
	std::shared_ptr<mycelium::scope> scope = generate_new_scope();

	token = this->tokenizer.get_next_token();

	mycelium::token next_token = this->tokenizer.get_next_token();

	if (next_token.string == "<") {
		ret = this->tokenizer.get_next_non_whitespace_tokens_until(">");
		name = this->tokenizer.get_next_token();
	}
	else {
		ret = {};
		name = next_token;
	}

	next_token = this->tokenizer.get_next_token();

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
												   mycelium::pattern_match::create_from_tokens(args), scope);

	if (show_debug_lines) {
		std::cout << "Parsed function: " << out->to_string() << std::endl;
	}

	if (this->tokenizer.get_next_token_without_increment().string == "{") {
		size_t starting_body_location = tokenizer.current_token_index;
		size_t ending_body_location = tokenizer.get_ending_grouping_token_index();
		if (get_body) {
			out->body = parse_tokens(starting_body_location + 1, ending_body_location - 1);
		}
		out->body_start_index = starting_body_location;
		out->body_end_index = ending_body_location;
		tokenizer.current_token_index = ending_body_location + 1;
	}
	else {
		throw_error("functions must have a body", token);
	}

	return out;
}

std::shared_ptr<mycelium::operatr> mycelium::parser::parse_operator(bool get_body) {
	/// TODO: Handle '<' and '>' in operator definition
	if (tokenizer.tokens[tokenizer.current_token_index + 1].string != "<") {
		throw_error(token::operator_keyword + " keyword must be followed by \'<\'", tokenizer.tokens[tokenizer.current_token_index]);
	}

	// Ignore the oper keyword
	token keyword_token = tokenizer.tokens[tokenizer.current_token_index + 1];
	tokenizer.current_token_index++;
	std::string name;
	std::vector<mycelium::token> ret = {};
	std::vector<mycelium::token> context = {};

	std::shared_ptr<mycelium::scope> scope = generate_new_scope();
	token name_token;

	token next_token = tokenizer.get_next_token_without_increment();

	size_t tmp, angle_bracket_ending_index = 0;

	if (next_token.string == "<") {
		for (auto& token : get_tokens_in_angle_brackets(tokenizer.tokens, tokenizer.current_token_index, tmp, angle_bracket_ending_index)) {
			if (show_debug_lines) {
				std::cout << "ret: " << token.string << std::endl;
			}
			ret.push_back(token);
		}
		tokenizer.skip_to_index(angle_bracket_ending_index + 1);
	}
	else {
		throw_error("operator definitions must contain a context", keyword_token);
	}

	next_token = tokenizer.get_next_token_without_increment();

	if (next_token.string == "<") {
		for (auto& token : get_tokens_in_angle_brackets(tokenizer.tokens, tokenizer.current_token_index, tmp, angle_bracket_ending_index)) {
			if (show_debug_lines) {
				std::cout << "context: " << token.string << std::endl;
			}
			context.push_back(token);
		}
		tokenizer.skip_to_index(angle_bracket_ending_index + 1);
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

	pattern_match context_pattern = pattern_match::create_from_tokens(context);

	/// TODO: Get priority
	std::shared_ptr<operatr> out = std::make_shared<mycelium::operatr>(token(), context, context_pattern, name, type::convert_tokens_to_types(ret), 1, scope);

	if (this->tokenizer.get_next_token_without_increment().string == "{") {
		size_t starting_body_location = tokenizer.current_token_index;
		size_t ending_body_location = tokenizer.get_ending_grouping_token_index();
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
		throw_error("operators must have a body", keyword_token);
	}

	if (show_debug_lines) {
		std::cout << "\n";
		std::cout << "Parsed Operator: " << out->to_string() << std::endl;
	}

	return out;
}

std::shared_ptr<mycelium::conditional> mycelium::parser::parse_cond(bool get_body) {

}

void mycelium::parser::find_function_declarations() {
	size_t pushed_index = tokenizer.current_token_index;

	// Find all the declarations for functions
	for (size_t i = 0; i < tokenizer.tokens.size(); i++) {
		mycelium::token current_token = tokenizer.tokens[i];
		if (current_token.type == keyword) {
			if (current_token.string == token::function_keyword) {
				tokenizer.current_token_index = i;

				functions.push_back(parse_function(false));
			}
			else if (current_token.string == token::operator_keyword) {
				tokenizer.current_token_index = i;

				operators.push_back(parse_operator(false));
			}
			else if (current_token.string == token::conditional_keyword) {

			}
		}
	}

	tokenizer.current_token_index = pushed_index;
}


std::shared_ptr<mycelium::return_from_function> mycelium::parser::parse_return(const std::vector<token>& tokens, size_t& index) {
	if (index > tokens.size()) {
		throw_error("trying to get parse return out of bounds", tokens.back());
	}
	// check if the return is the last token
	if (index == tokens.size()) {
		if (current_parsing_function->ret.empty()) {
			return std::make_shared<return_from_function>(current_parsing_function);
		}
		throw_error("Mismatch in return types", current_parsing_function->name);
	}

	// Skip 'return' token
	index++;
	std::vector<token> remaining_line_tokens = get_tokens_until_newline(tokens, index);
	skip_to_newline(tokens, index);

	if (remaining_line_tokens.empty()) {
		if (current_parsing_function->ret.empty()) {
			return std::make_shared<return_from_function>(current_parsing_function);
		}
		throw_error("Mismatch in return types", current_parsing_function->name);
	}
	else {
		bool contains_comma = false;

		for (const auto& tk : remaining_line_tokens) {
			if (tk.string == ",") {
				contains_comma = true;
				break;
			}
		}

		if (contains_comma) {
			throw_error("Returning Multiple Values is Not Yet Supported", remaining_line_tokens[0]);
		}

		if (spam_debug_output) {
			std::cout << "parsing return expression: " << token_vec_to_string(remaining_line_tokens) << "\n";
		}
		std::shared_ptr<expression> value = get_expression_from_tokens(remaining_line_tokens);

		if (!value.get()) {
			throw_error("Unknown return value " + tokens_to_string(remaining_line_tokens), remaining_line_tokens[0]);
		}

		if (current_parsing_function->ret.size() == 1 && current_parsing_function->ret[0] == value->get_type()) {
			return std::make_shared<return_from_function>(current_parsing_function, value);
		}

		throw_error("Mismatch in return types", current_parsing_function->name);
	}
}

std::shared_ptr<mycelium::variable> mycelium::parser::parse_variable(const std::vector<token>& tokens, size_t& index, int variable_type) {

	if (show_debug_lines) {
		std::cout << "parsing variable: type: " << type::types[variable_type].name << "\tname: " << tokens[index].string << std::endl;
	}

	return current_scope->make_variable(tokens[index++], type::types[variable_type]);
}



mycelium::pattern_match mycelium::parser::generate_pattern_from_function(const std::shared_ptr<mycelium::function_base>& fn, const mycelium::token& tk) {

	// We convert the single token (e.g. "1+2") into multiple if possible
	// For example with the operator int + int we would convert it into "1", "+", "2" and then pass it on to the multiple token function

	std::vector<size_t> landmark_positions = {0};
	std::vector<token> tokens;

	/// Convert the single token 'tk' into multiple tokens in 'tokens'.
	/// This is done going over all function arguments and finding "landmarks". These are only operators for now but this might change in the future.
	for (auto & arg : fn->args.pattern) {
		if (arg->type == pattern_token_type::pt_oper) {
			unsigned long pos;
			std::string arg_string = std::static_pointer_cast<pattern_tokens::oper>(arg)->value;
			if (!landmark_positions.empty()) {
				if ((pos = tk.string.find(arg_string, landmark_positions.back())) != std::string::npos) {
					if (pos != 0) {
						tokens.emplace_back(tk.string.substr(landmark_positions.back(), pos - landmark_positions.back()));
					}
					tokens.emplace_back(tk.string.substr(pos, arg_string.length()));

					landmark_positions.push_back(pos + arg_string.length());
				}
			}
			else {
				if ((pos = tk.string.find(arg_string, 0) != std::string::npos)) {
					if (pos != 0) {
						tokens.emplace_back(tk.string.substr(0, pos));
					}
					tokens.emplace_back(tk.string.substr(pos, arg_string.length()));

					landmark_positions.push_back(pos + arg_string.length());
				}
			}
		}
	}



	if (tokens.empty()) {
		return {};
	}

	if (landmark_positions.back() != tk.string.length()) {
		tokens.emplace_back(tk.string.substr(landmark_positions.back()));
	}


	// We pass the vector of tokens to the multiple token function.
	// This way we avoid repeating ourselves. Its possible that it would be easier to do things separately
	// but I don't feel like doing that.
	return generate_pattern_from_function(fn, tokens);
}


mycelium::pattern_match mycelium::parser::generate_pattern_from_function(const std::shared_ptr<mycelium::function_base>& fn, const std::vector<mycelium::token>& tks) {
	if (fn->args.pattern.empty()) {
		if (tks.empty()) {
			return {};
		}
		else {
			/// Create a pattern with something in it so that when we try to match against nothing it fails
			pattern_match out;
			out.pattern.push_back(make_pattern_token("Invalid Pattern"));
			return out;
		}
	}
//	std::cout << "getpatfrm: " << fn->to_string() << ":\n";
//	for (auto& tk : tks) {
//		std::cout << "\t" << tk.string << "\n";
//	}

	std::vector<std::vector<token>> landmark_chunks;
	std::vector<size_t> desired_chunk_sizes = {0};
	size_t number_of_chunks = 0;
	bool last_was_oper = true;
	std::vector<token> landmarks;

	size_t landmark_chunk_index = 0;
	for (auto & arg : fn->args.pattern) {
		if (arg->type == pattern_token_type::pt_oper) {
			std::string arg_string = std::static_pointer_cast<pattern_tokens::oper>(arg)->value;
			desired_chunk_sizes.push_back(0);
			landmarks.emplace_back(arg_string);
			landmark_chunks.emplace_back();
			for (; landmark_chunk_index < tks.size() && tks[landmark_chunk_index].string != arg_string; landmark_chunk_index++) {
				landmark_chunks.back().push_back(tks[landmark_chunk_index]);
			}

			/// If we don't find the landmark this pattern is invalid.
			if (landmark_chunk_index >= tks.size() || tks[landmark_chunk_index].string != arg_string) {
				return {};
			}

			// Skip the operator after we check if it exists
			landmark_chunk_index++;

			last_was_oper = true;
		}
		else {
			// Increas the number of chunks every time we switch between operator and value.
			// This ensures that we only increase this counter when we actuall need a new chunk
			if (last_was_oper) {
				number_of_chunks++;
			}
			desired_chunk_sizes.back()++;
			last_was_oper = false;
		}
	}

	/// If there are no landmark use all the tokens to create one expression
	if (landmarks.empty()) {
		landmark_chunks.emplace_back();
		pattern_match out;
		if (spam_debug_output) {
			std::cout << "creating expression from tokens in getpat: " << token_vec_to_string(tks) << "\n";
		}
		std::shared_ptr<expression> expr = get_expression_from_tokens(tks);
		if (expr) {
			out.pattern.push_back(make_pattern_token(expr));
		}
		return out;
	}

	/// If the last pattern token is a land mark then we don't want get the last chunk as there isn't one
	if (landmark_chunk_index < tks.size()) {
		/// Add a new landmark chunk only if the current one is not empty
		/// This avoids having an empty landmark_chunk for operators that start with a landmark e.g. print <value>.
		if (!landmark_chunks.back().empty()) {
			landmark_chunks.emplace_back();
		}
		/// Get the last tokens into a chunk if there is one
		for (size_t i = landmark_chunk_index; i < tks.size(); i++) {
			landmark_chunks.back().push_back(tks[i]);
		}
	}

	if (number_of_chunks != landmark_chunks.size()) {
		return {};
	}

	/*
	std::cout << "lmc:\n";
	for (auto& lmc : landmark_chunks) {
		std::cout << "\t{";
		for (auto& chunk : lmc) {
			std::cout << chunk.string;
			if (&chunk != &lmc.back()) {
				std::cout << ", ";
			}
		}
		std::cout << "}\n";
	}
	*/

	std::vector<std::shared_ptr<expression>> landmark_chunk_expressions;

	for (size_t i = 0; i < landmark_chunks.size(); i++) {
		if (spam_debug_output) {
			std::cout << "creating expression from chunk: " << token_vec_to_string(landmark_chunks[i]) << "\n";
		}
		std::shared_ptr<expression> expr = get_expression_from_tokens(landmark_chunks[i]);
		if (!expr) {
			/// If we get an invalid expression then the pattern is not valid
			return {};
		}

		landmark_chunk_expressions.push_back(expr);
	}

	pattern_match out;

	size_t expr_index = 0;
	for (size_t i = 0; out.pattern.size() < landmark_chunk_expressions.size() + landmarks.size() && i < desired_chunk_sizes.size(); i++) {
		for (size_t j = 0; j < desired_chunk_sizes[i]; j++) {
			out.pattern.push_back(make_pattern_token(landmark_chunk_expressions[expr_index++]));
		}
		if (i < landmarks.size()) {
			out.pattern.push_back(make_pattern_token(landmarks[i].string));
		}
	}

	if (show_debug_lines) {
		// std::cout << "got pattern: " << out.to_string() << "\n";
	}

	return out;
}

void mycelium::parser::skip_to_newline(const std::vector<token>& tokens, size_t& index) {
	int current_line = tokens[index - 1].line;

	if (current_line == tokens.back().line) {
		index = tokens.size();
		return;
	}

	for (; tokens[index++].line == current_line;) {
		if (tokens[index - 1].type != string_literal && tokens[index - 1].string == ";") {
			break;
		}
	}
	index--;
}


void mycelium::parser::skip_function_definition(const std::vector<token>& tokens, size_t& index) {
	int depth = 0;
	token first_curly;
	for (; index < tokens.size(); index++) {
		mycelium::token token = tokens[index];

		if (token.string == "{") {
			if (first_curly.type == token_type::invalid) {
				first_curly = token;
			}
			depth++;
		}

		else if (token.string == "}") {
			depth--;

			if (first_curly.type == token_type::invalid) {
				first_curly = token;
			}

			if (depth == 0) {
				// Skip the closing curly brace
				index++;
				return;
			}
		}
	}

	if (depth > 0) {
		throw_error("unclosed curly", first_curly);
	}
	else {
		throw_error("stray curly", first_curly);
	}
}

std::vector<mycelium::token> mycelium::parser::get_tokens_until_newline(const std::vector<token>& tokens, size_t index) {
	std::vector<token> out;
	/// Decrementing so that we get the current token
	int current_line = tokens[--index].line;
	/// If we're on the last line we just want to return the remaining tokens
	if (current_line == tokens.back().line) {
		for (size_t i = index; i < tokens.size(); i++) {
			if (tokens[i].type == separator && tokens[i].string == ";") {
				break;
			}
			out.push_back(tokens[i]);
		}
		return out;
	}


	token next;
	for (; (next = tokens[index++]).line == current_line;) {
		if (next.type != string_literal && next.string == ";") {
			break;
		}
		out.push_back(next);
	}

	return out;
}

std::vector<mycelium::token> mycelium::parser::get_tokens_in_grouping(const std::vector<token>& tks, size_t search_index, size_t& open_token_index, size_t& close_token_index, const std::string& open_token_string, const std::string& close_token_string) {
	std::vector<token> out;
	int search_depth = 0;

	open_token_index = 0xffffffffffffffff;
	close_token_index = 0xffffffffffffffff;

	for (size_t i = search_index; i < tks.size(); i++) {
		const auto& token = tks[i];
		if (token.type == token_type::grouping && token.string == open_token_string) {
			search_depth++;
			if (search_depth == 1) {
				open_token_index = i;
				continue;
			}
		}
		else if (token.type == token_type::grouping && token.string == close_token_string) {
			search_depth--;
			if (search_depth == 0) {
				close_token_index = i;
				break;
			}
		}

		if (search_depth != 0) {
			out.push_back(token);
		}
	}

	return out;
}


inline std::vector<mycelium::token> mycelium::parser::get_tokens_in_angle_brackets(const std::vector<token>& tks, size_t search_index) {
	size_t tmp1,tmp2;

	return get_tokens_in_angle_brackets(tks, search_index, tmp1, tmp2);
}


inline std::vector<mycelium::token> mycelium::parser::get_tokens_in_angle_brackets(const std::vector<token>& tks, size_t search_index, size_t& open_angle_index, size_t& close_angle_index) {
	return get_tokens_in_grouping(tks, search_index, open_angle_index, close_angle_index, "<", ">");
}

inline std::vector<mycelium::token> mycelium::parser::get_tokens_in_curlies(const std::vector<token>& tks, size_t search_index) {
	size_t tmp1,tmp2;

	return get_tokens_in_curlies(tks, search_index, tmp1, tmp2);
}


inline std::vector<mycelium::token> mycelium::parser::get_tokens_in_curlies(const std::vector<token>& tks, size_t search_index, size_t& open_curly_index, size_t& close_curly_index) {
	return get_tokens_in_grouping(tks, search_index, open_curly_index, close_curly_index, "{", "}");
}

inline std::vector<mycelium::token> mycelium::parser::get_tokens_in_parentheses(const std::vector<token>& tks, size_t search_index) {
	size_t tmp1,tmp2;

	return get_tokens_in_parentheses(tks, search_index, tmp1, tmp2);
}


inline std::vector<mycelium::token> mycelium::parser::get_tokens_in_parentheses(const std::vector<token>& tks, size_t search_index, size_t& open_parentheses_index, size_t& close_parentheses_index) {
	return get_tokens_in_grouping(tks, search_index, open_parentheses_index, close_parentheses_index, "(", ")");
}


std::shared_ptr<mycelium::expression> mycelium::parser::get_function(const std::vector<token>& tks, size_t& index) {
	// This is needed because of cpp type shenanigans
	size_t index_copy = index;
	size_t start_index = index;
	token name = tks[index];
	std::vector<std::shared_ptr<function_base>> search_functions;
	for (const auto& func : this->functions) {
		search_functions.push_back(func);
	}
	auto fn = get_function(tks, index_copy, search_functions);

	search_functions.clear();
	for (const auto& cond : this->conditionals) {
		search_functions.push_back(cond);
	}

	auto cn = get_function(tks, index, search_functions);

	if (fn.get()) {
		index = index_copy;
		return fn;
	}

	if (cn.get()) {
		return cn;
	}


	pattern_match fn_call_pattern = get_pattern_from_tokens(get_tokens_in_parentheses(tks, start_index));
	std::string pattern_string = fn_call_pattern.to_string();
	throw_error("Unknown function or conditional \"" + name.string + "(" + pattern_string + ")\"", name);
	return {};
}

std::shared_ptr<mycelium::expression> mycelium::parser::get_function(const std::vector<token>& tks, size_t& index, const std::vector<std::shared_ptr<mycelium::function_base>>& search_functions) {
	size_t start_parentheses_index, end_parentheses_index;
	std::vector<token> inside_grouping = get_tokens_in_parentheses(tks, index, start_parentheses_index, end_parentheses_index);



	token name;
	if (index + 1 == start_parentheses_index) {
		name = tks[index];
	}

	if (show_debug_lines) {
		std::cout << "Getting Function " << name.string << " with " << tokens_to_string(inside_grouping) << "\n";
	}

	std::vector<std::shared_ptr<function_base>> funcs;
	std::vector<pattern_match> func_call_patterns;

	for (auto& fn : search_functions) {
		if (fn->name == name) {
			pattern_match fn_call_pattern = this->generate_pattern_from_function(fn, inside_grouping);
			if (fn->args.is_match(fn_call_pattern)) {
				funcs.push_back(fn);
				func_call_patterns.push_back(fn_call_pattern);
			}
		}
	}


	index = end_parentheses_index + 1;
	if (funcs.empty()) {
		return {};
	}
	else if (funcs.size() == 1) {
		if (funcs[0]->type == parsed_token_type::func) {
			std::shared_ptr<function> fn = std::static_pointer_cast<function>(funcs[0]);
			return std::make_shared<function_call>(fn, func_call_patterns[0].get_expressions());
		}
		else if (funcs[0]->type == parsed_token_type::cond) {
			std::shared_ptr<conditional> cn = std::static_pointer_cast<conditional>(funcs[0]);

			std::shared_ptr<mycelium::scope> body_scope = generate_new_scope();

			return std::make_shared<conditional_call>(cn, func_call_patterns[0].get_expressions(), body_scope, false);
		}
	}
	else {
		/// TODO: Decide if this should be an error or a warning
		throw_error("Ambiguous Function Call Between " + funcs[0]->to_string() + " and " + funcs[1]->to_string(), name);
	}
}


std::shared_ptr<mycelium::expression> mycelium::parser::get_object_function(const std::shared_ptr<expression>& object, const std::vector<token>& function_tokens, const std::vector<token>& other_tokens) {
	size_t tmp = 0;
	std::shared_ptr<expression> function_call_as_expression = get_function(function_tokens, tmp, object->get_type().get_member_functions());

	std::shared_ptr<function_call> func = std::static_pointer_cast<function_call>(function_call_as_expression);

	if (func.get()) {
		func->args.insert(func->args.begin(), object);

		if (!other_tokens.empty()) {
			std::vector<token> new_function_tokens;
			std::vector<token> new_other_tokens;
			bool found_seperator = false;

			for (const auto& tk : other_tokens) {
				if (tk.string == ".") {
					if (found_seperator) {
						break;
					}

					found_seperator = true;
					continue;
				}

				if (found_seperator) {
					new_other_tokens.push_back(tk);
				}
				else {
					new_function_tokens.push_back(tk);
				}
			}
			if (!found_seperator) {
				return {};
			}
			return get_object_function(func, new_function_tokens, new_other_tokens);
		}

		return func;
	}
	else {
		std::string function_as_string;
		for (auto& tk : function_tokens) {
			function_as_string += tk.string;
		}
		throw_error("Unknown member function \"" + function_as_string + "\" of " + object->to_string(), function_tokens[0]);
		return {};
	}
}

std::shared_ptr<mycelium::expression> mycelium::parser::get_object_function(const std::vector<token>& tks, size_t index) {
	std::vector<token> object_tokens;
	std::vector<token> function_call_tokens;
	std::vector<token> other_tokens;
	bool found_seperator = false;
	bool multiple_seperators = false;

	for (const auto& tk : tks) {
		if (tk.string == ".") {
			if (found_seperator) {
				multiple_seperators = true;
			}

			found_seperator = true;
			continue;
		}


		if (multiple_seperators) {
			other_tokens.push_back(tk);
		}
		else if (found_seperator) {
			function_call_tokens.push_back(tk);
		}
		else {
			object_tokens.push_back(tk);
		}
	}

	if (!found_seperator) {
		return {};
	}

	if (object_tokens.empty()) {
		/// TODO: Invalid object function. (Should we treat this as an error and do some more checks before calling this function
		return {};
	}

	if (function_call_tokens.empty()) {
		/// TODO: Invalid object function. (Should we treat this as an error and do some more checks before calling this function
		return {};
	}

	if (multiple_seperators && other_tokens.empty()) {
		/// TODO: Invalid object function. (Should we treat this as an error and do some more checks before calling this function
		return {};
	}

	if (spam_debug_output) {
		std::cout << "getting object in object function: " << token_vec_to_string(object_tokens) << "\n";
	}
	std::shared_ptr<expression> object = get_expression_from_tokens(object_tokens);

	if (!object.get()) {
		std::string out;
		for (const auto& token : object_tokens) {
			out += token.string;
			if (&token != &object_tokens.back()) {
				out += " ";
			}
		}
		/// FIXME: This really should be an error. However we are checking everything for object functions. This means that we have a soft fail.
		/// Meaning if we don't find an object or function we assume no error has taken place and continue trying to figure out what the expression should be.
		/// For example, 'string a = b.to_string()' would try to match 'string a = b' as an object with the member function 'to_string'. What we really want is 'string a = (b.to_string())'.
		/// I think this is an issue higher up (calling get_expression_from_tokens when it is not strictly needed).
		/// This behavior does not produce any error in functionality but if user produces an error in the mycelium code (E.G. trying to use a member function on a non-existant object) they will get no help from this when they should.
		warn("Unknown Object " + out, object_tokens[0]);
		return {};
	}


	return get_object_function(object, function_call_tokens, other_tokens);
}

std::shared_ptr<mycelium::expression> mycelium::parser::parse_expression(const std::vector<token>& tokens, size_t& index) {
	index--;
	mycelium::token next_token = tokens[index++];

	if (index >= tokens.size() || tokens[index].string != "(") {
		std::vector<token> line_tokens = get_tokens_until_newline(tokens, index);
		if (spam_debug_output) {
			std::cout << "getting expression from line in parse expression: " << token_vec_to_string(line_tokens) << "\n";
		}
		std::shared_ptr<expression> expr = get_expression_from_tokens(line_tokens);
		if (expr.get()) {
			return expr;
		}
		else {
			throw_error("Unknown expression \"" + token_vec_to_string(line_tokens) + "\"", next_token);
			return {};
		}
	}

	if (tokens[index].string == ".") {
		return get_object_function(tokens, index - 1);
	}

	return get_function(tokens, --index);
}

//bool mycelium::parser::can_match_pattern(const mycelium::pattern_match& match, const mycelium::pattern_match& other) {
//    pattern_match smushed_other;
//    for (int i = 0, j = 0; i < match.pattern.size(); i++) {
//        if (match.pattern[i].is_expression && other.pattern[j].is_expression) {
//            continue;
//        }
//        else if (!match.pattern[i].is_expression) {
//            if (other.pattern[i].oper == match.pattern[i].oper) {
//                if (other.pattern[i - 1].is_expression && other.pattern[j - 1].is_expression) {
//                    smushed_other.pattern.push_back(other.pattern[j - 1]);
//                }
//                smushed_other.pattern.push_back(other.pattern[j]);
//            }
//            else
//        }
//
//    }
//}

std::shared_ptr<mycelium::variable> mycelium::parser::get_word_variable(const mycelium::token& word) {
	return this->get_variable(word.string);
}

std::shared_ptr<mycelium::constant> mycelium::parser::get_constant(const mycelium::token& word) {
	if (word.type == num) {
		return constant::make_constant(std::stoi(word.string));
	}
	if (word.type == string_literal) {
		return constant::make_constant(word.string);
	}
	else {
		return {};
	}
}

std::shared_ptr<mycelium::scope> mycelium::parser::generate_new_scope() {
	scopes.push_back(std::make_shared<scope>(current_scope));
	return scopes.back();
}

void mycelium::parser::change_scope(const std::shared_ptr<scope>& new_scope, bool delete_previous_scope) {
	if (delete_previous_scope) {
		for (size_t i = 0; i < scopes.size(); i++) {
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
	std::shared_ptr<function> main;
	for (auto& fn : this->functions) {
		if (fn->name.string == "main") {
			if (fn->args.pattern.empty()) {
				main = fn;
				break;
			}
		}
	}
	if (mycelium::show_debug_lines) {
		if (!main) {
			std::cout << "Could not find main function" << std::endl;
		}
		else {
			std::cout << "Found main function" << std::endl;
		}
		std::cout << "Executing global statements:\n";
	}
	for (auto& pt : this->parsed_tokens) {
		if (pt) {
			pt->execute();
		}
	}

	if (main) {
		std::vector<std::shared_ptr<variable>> args;
		std::shared_ptr<variable> exit_code = {};
		if (mycelium::show_debug_lines) {
			std::cout << "Calling main function:\n";
			for (auto &pt: main->body) {
				if (pt.get()) {
					std::cout << pt->to_string() << "\n";
				}
			}
		}
		main->call(args, exit_code);

		if (exit_code != 0) {

		}
	}
}

std::shared_ptr<mycelium::variable> builtin_print(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	if (!args.empty()) {
		for (auto& arg : args) {
			if (arg != nullptr) {
				std::cout << arg->get_as_string();
				if (&arg != &args.back()) {
					std::cout << ' ';
				}
			}
			else {
				mycelium::throw_error("nullptr variable in print");
			}
		}
	}

	return mycelium::constant::make_constant(0);
}

std::shared_ptr<mycelium::variable> builtin_println(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	if (!args.empty()) {
		for (auto& arg : args) {
			if (arg != nullptr) {
				std::cout << arg->get_as_string();
				if (&arg != &args.back()) {
					std::cout << ' ';
				}
			}
			else {
				mycelium::throw_error("nullptr variable in println");
			}
		}
	}
	std::cout << "\n";

	return mycelium::constant::make_constant(0);
}


std::shared_ptr<mycelium::variable> builtin_get_arg(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(mycelium::parser::program_args[args[0]->get_value()->value]);
}

std::shared_ptr<mycelium::variable> builtin_num_args(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(mycelium::parser::program_args.size());
}

std::shared_ptr<mycelium::variable> builtin_int_to_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->get_as_string());
}


std::shared_ptr<mycelium::variable> builtin_exit(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	if (args.empty()) {
		exit(0);
	}

	exit(args[0]->value);
}




std::shared_ptr<mycelium::variable> builtin_file_get_line(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	std::string line;

	std::getline(*args[0]->file, line);

	args[0]->good = std::ios::goodbit;

	return mycelium::constant::make_constant(line);
}

std::shared_ptr<mycelium::variable> builtin_file_has_line(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_bool_constant(args[0]->good);
}



std::shared_ptr<mycelium::variable> builtin_list_push_back(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->list_ptr->push_back(args[1]->get_value());
	return mycelium::constant::make_constant(0);
}

std::shared_ptr<mycelium::variable> builtin_list_length(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant( args[0]->list_ptr->size() );
}

std::shared_ptr<mycelium::variable> builtin_list_is_empty(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant( args[0]->list_ptr->empty() );
}


std::shared_ptr<mycelium::variable> builtin_string_split(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	std::string str = *args[0]->str;
	std::vector<std::shared_ptr<mycelium::variable>> spl = *args[1]->list_ptr;

	std::vector<std::shared_ptr<mycelium::variable>> out;


	if (str.empty()) {
		return mycelium::constant::make_constant(out);
	}

	long pos;
	long prev_pos = 0;
	long min_pos;
	std::string split;

	while ((size_t)prev_pos < str.length()) {

		min_pos = -1;

		for (auto &comp: spl) {
			if ((size_t)(pos = str.find(*comp->str, prev_pos)) != std::string::npos) {
				if (pos < min_pos || min_pos == -1) {
					min_pos = pos;
					split = *comp->str;
				}
			}
		}

		if (min_pos == -1) {
			break;
		}

		if (min_pos != prev_pos) {
			out.push_back(mycelium::constant::make_constant(str.substr(prev_pos, min_pos - prev_pos)));
		}

		out.push_back(mycelium::constant::make_constant(split));

		prev_pos = min_pos + split.length();
	}

	if ((size_t)min_pos >= str.length()) {
		out.push_back(mycelium::constant::make_constant(str.substr(prev_pos, min_pos - prev_pos)));
	}

	return mycelium::constant::make_constant(out);
}

std::shared_ptr<mycelium::variable> builtin_string_length(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant( args[0]->str->size() );
}

std::shared_ptr<mycelium::variable> builtin_string_is_empty(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant( args[0]->str->empty() );
}



std::vector<std::shared_ptr<mycelium::function>> mycelium::parser::create_base_functions() {
	std::vector<std::shared_ptr<function>> out;

	out.push_back(
			std::make_shared<builtin_function>("print", std::vector<type>({}), std::vector<type>({type::integer}), builtin_print, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("print", std::vector<type>({}), std::vector<type>({type::string}), builtin_print, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({type::integer}), builtin_println, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({type::integer, type::integer}), builtin_println, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({type::string}),  builtin_println, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({type::boolean}),  builtin_println, generate_new_scope())
				  );


	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({type::list}),  builtin_println, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({}), builtin_println, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({type::file}), builtin_println, generate_new_scope())
	);


	out.push_back(
			std::make_shared<builtin_function>("exit", std::vector<type>({}), std::vector<type>({}), builtin_exit, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("exit", std::vector<type>({}), std::vector<type>({type::integer}), builtin_exit, generate_new_scope())
	);


	out.push_back(
			std::make_shared<builtin_function>("get_arg", std::vector<type>({type::string}), std::vector<type>({type::integer}), builtin_get_arg, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_function>("num_args", std::vector<type>({type::integer}), std::vector<type>({}), builtin_num_args, generate_new_scope())
	);


	// Integer Functions
	type::integer.add_member_function(
		std::make_shared<mycelium::builtin_function>("to_string", std::vector<mycelium::type>({type::string}), std::vector<mycelium::type>({}), builtin_int_to_string, generate_new_scope())
	);


	// File Functions
	type::file.add_member_function(
		std::make_shared<mycelium::builtin_function>("get_line", std::vector<mycelium::type>({type::string}), std::vector<mycelium::type>({}), builtin_file_get_line, generate_new_scope())
	);

	type::file.add_member_function(
		std::make_shared<mycelium::builtin_function>("has_line", std::vector<mycelium::type>({type::boolean}), std::vector<mycelium::type>({}), builtin_file_has_line, generate_new_scope())
	);


	// List Functions
	type::list.add_member_function(
		std::make_shared<mycelium::builtin_function>("push", std::vector<mycelium::type>({}), std::vector<mycelium::type>({type::integer}), builtin_list_push_back, generate_new_scope())
	);

	type::list.add_member_function(
		std::make_shared<mycelium::builtin_function>("push", std::vector<mycelium::type>({}), std::vector<mycelium::type>({type::string}), builtin_list_push_back, generate_new_scope())
	);

	type::list.add_member_function(
		std::make_shared<mycelium::builtin_function>("length", std::vector<mycelium::type>({type::integer}), std::vector<mycelium::type>({}), builtin_list_length, generate_new_scope())
	);

	type::list.add_member_function(
		std::make_shared<mycelium::builtin_function>("is_empty", std::vector<mycelium::type>({type::boolean}), std::vector<mycelium::type>({}), builtin_list_is_empty, generate_new_scope())
	);


	// String Functions
	type::string.add_member_function(
		std::make_shared<mycelium::builtin_function>("split", std::vector<mycelium::type>({type::list}), std::vector<mycelium::type>({type::list}), builtin_string_split, generate_new_scope())
	);

	type::string.add_member_function(
		std::make_shared<mycelium::builtin_function>("length", std::vector<mycelium::type>({type::integer}), std::vector<mycelium::type>({}), builtin_string_length, generate_new_scope())
	);

	type::string.add_member_function(
		std::make_shared<mycelium::builtin_function>("is_empty", std::vector<mycelium::type>({type::boolean}), std::vector<mycelium::type>({}), builtin_string_is_empty, generate_new_scope())
	);


	return out;
}


/// TODO: Move this to another file
std::shared_ptr<mycelium::variable> builtin_eq_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value == args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_neq_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value != args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_lt_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value < args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_gt_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value > args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_leq_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value <= args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_geq_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value >= args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_add_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value + args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_subtract_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value - args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_multiply_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value * args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_divide_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value / args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_modulo_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(args[0]->value % args[1]->value);
}

std::shared_ptr<mycelium::variable> builtin_append_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_constant(*args[0]->str + *args[1]->str);
}


std::shared_ptr<mycelium::variable> builtin_append_in_place_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	*args[0]->str += *args[1]->str;
	return args[0];
}

std::shared_ptr<mycelium::variable> builtin_assign_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value = args[1]->value;
	return args[0];
}

std::shared_ptr<mycelium::variable> builtin_inc_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value++;
	return mycelium::constant::make_constant(args[0]->value - 1);
}

std::shared_ptr<mycelium::variable> builtin_dec_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value--;
	return mycelium::constant::make_constant(args[0]->value + 1);
}


std::shared_ptr<mycelium::variable> builtin_assign_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	*args[0]->str = *args[1]->get_value()->str;
	return args[0];
}

std::shared_ptr<mycelium::variable> builtin_eq_string(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_bool_constant(*args[0]->str == *args[1]->get_value()->str);
}

std::shared_ptr<mycelium::variable> builtin_eq_bool(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_bool_constant(args[0]->value == args[1]->get_value()->value);
}

std::shared_ptr<mycelium::variable> builtin_or_bool(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_bool_constant(args[0]->value || args[1]->get_value()->value);
}

std::shared_ptr<mycelium::variable> builtin_and_bool(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_bool_constant(args[0]->value && args[1]->get_value()->value);
}

std::shared_ptr<mycelium::variable> builtin_not_bool(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	return mycelium::constant::make_bool_constant(!args[0]->get_value()->value);
}



std::shared_ptr<mycelium::variable> builtin_plus_equals_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value += args[1]->value;
	return args[0];
}

std::shared_ptr<mycelium::variable> builtin_minus_equals_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value = args[0]->value - args[1]->value;

	return args[0];
}

std::shared_ptr<mycelium::variable> builtin_multiply_equals_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value = args[0]->value * args[1]->value;

	return args[0];
}


std::shared_ptr<mycelium::variable> builtin_assign_file(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	*args[0]->str = *args[1]->get_value()->str;
	args[0]->file->open(*args[0]->str);
	return args[0];
}


std::shared_ptr<mycelium::variable> builtin_index_list(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	if (!args[0]->list_ptr || (size_t)args[1]->value >= args[0]->list_ptr->size()) {
		/// TODO: Make this a function
		std::cerr << "Runtime Error: Index Out Of Bounds\n";
		exit(1);
	}
	return args[0]->list_ptr->at(args[1]->get_value()->value);
}


std::shared_ptr<mycelium::variable> builtin_assign_list(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	*args[0]->list_ptr = *args[1]->get_value()->list_ptr;
	return args[0];
}

std::vector<std::shared_ptr<mycelium::operatr>> mycelium::parser::create_base_operators() {
	std::vector<std::shared_ptr<operatr>> out;

	/// TODO: Make these priority value not just made up
	/// Math
	out.push_back(
			std::make_shared<builtin_operator>("+", std::vector<token>({token("int"), token("a"), token("+"), token("int"), token("b")}), "builtin_add_int", std::vector<type>({type::integer}), builtin_add_int, 20, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("-", std::vector<token>({token("int"), token("a"), token("-"), token("int"), token("b")}), "builtin_subtract_int", std::vector<type>({type::integer}), builtin_subtract_int, 20, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("*", std::vector<token>({token("int"), token("a"), token("*"), token("int"), token("b")}), "builtin_multiply_int", std::vector<type>({type::integer}), builtin_multiply_int, 10, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("/", std::vector<token>({token("int"), token("a"), token("/"), token("int"), token("b")}), "builtin_multiply_int", std::vector<type>({type::integer}), builtin_divide_int, 10, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("%", std::vector<token>({token("int"), token("a"), token("%"), token("int"), token("b")}), "builtin_modulo_int", std::vector<type>({type::integer}), builtin_modulo_int, 10, generate_new_scope())
	);

	/// Strings
	out.push_back(
			std::make_shared<builtin_operator>("+", std::vector<token>({token("string"), token("a"), token("+"), token("string"), token("b")}), "builtin_append_string", std::vector<type>({type::string}), builtin_append_string, 20, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("+=", std::vector<token>({token("string"), token("a"), token("+="), token("string"), token("b")}), "builtin_append_in_place_string", std::vector<type>({type::string}), builtin_append_in_place_string, 20, generate_new_scope())
	);


	out.push_back(
			std::make_shared<builtin_operator>("=", std::vector<token>({token("string"), token("a"), token("="), token("string"), token("b")}), "builtin_assign_string", std::vector<type>({type::string}), builtin_assign_string, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("==", std::vector<token>({token("string"), token("a"), token("=="), token("string"), token("b")}), "builtin_eq_string", std::vector<type>({type::boolean}), builtin_eq_string, 99, generate_new_scope())
	);



	/// Ints

	out.push_back(
			std::make_shared<builtin_operator>("=", std::vector<token>({token("int"), token("a"), token("="), token("int"), token("b")}), "builtin_assign_int", std::vector<type>({type::integer}), builtin_assign_int, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("+=", std::vector<token>({token("int"), token("a"), token("+="), token("int"), token("b")}), "builtin_plus_eq_int", std::vector<type>({type::integer}), builtin_plus_equals_int, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("*=", std::vector<token>({token("int"), token("a"), token("*="), token("int"), token("b")}), "builtin_multiply_eq_int", std::vector<type>({type::integer}), builtin_multiply_equals_int, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("-=", std::vector<token>({token("int"), token("a"), token("-="), token("int"), token("b")}), "builtin_minus_eq_int", std::vector<type>({type::integer}), builtin_minus_equals_int, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("++", std::vector<token>({token("int"), token("a"), token("++")}), "builtin_inc_int", std::vector<type>({type::integer}), builtin_inc_int, 5, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("--", std::vector<token>({token("int"), token("a"), token("--")}), "builtin_dec_int", std::vector<type>({type::integer}), builtin_dec_int, 5, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("==", std::vector<token>({token("int"), token("a"), token("=="), token("int"), token("b")}), "builtin_eq_int", std::vector<type>({type::boolean}),
											   builtin_eq_int, 50, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("!=", std::vector<token>({token("int"), token("a"), token("!="), token("int"), token("b")}), "builtin_neq_int", std::vector<type>({type::boolean}),
											   builtin_neq_int, 50, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("<", std::vector<token>({token("int"), token("a"), token("<"), token("int"), token("b")}), "builtin_lt_int", std::vector<type>({type::boolean}),
											   builtin_lt_int, 50, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>(">", std::vector<token>({token("int"), token("a"), token(">"), token("int"), token("b")}), "builtin_gt_int", std::vector<type>({type::boolean}),
											   builtin_gt_int, 50, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("<=", std::vector<token>({token("int"), token("a"), token("<="), token("int"), token("b")}), "builtin_leq_int", std::vector<type>({type::boolean}),
											   builtin_leq_int, 50, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>(">", std::vector<token>({token("int"), token("a"), token(">="), token("int"), token("b")}), "builtin_geq_int", std::vector<type>({type::boolean}),
											   builtin_geq_int, 50, generate_new_scope())
	);

	/// Bools

	out.push_back(
			std::make_shared<builtin_operator>("=", std::vector<token>({token("bool"), token("a"), token("="), token("int"), token("b")}), "builtin_assign_bool", std::vector<type>({type::boolean}),
											   builtin_assign_int, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("==", std::vector<token>({token("bool"), token("a"), token("=="), token("bool"), token("b")}), "builtin_eq_bool", std::vector<type>({type::boolean}),
											   builtin_eq_bool, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("||", std::vector<token>({token("bool"), token("a"), token("||"), token("bool"), token("b")}), "builtin_or_bool", std::vector<type>({type::boolean}),
											   builtin_or_bool, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("&&", std::vector<token>({token("bool"), token("a"), token("&&"), token("bool"), token("b")}), "builtin_and_bool", std::vector<type>({type::boolean}),
											   builtin_and_bool, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("!", std::vector<token>({token("!"), token("bool"), token("a")}), "builtin_not_bool", std::vector<type>({type::boolean}),
											   builtin_not_bool, 90, generate_new_scope())
	);

	/// Files
	out.push_back(
			std::make_shared<builtin_operator>("=", std::vector<token>({token("file"), token("a"), token("="), token("string"), token("b")}), "builtin_assign_file", std::vector<type>({type::file}),
											   builtin_assign_file, 99, generate_new_scope())
	);


	/// Lists
	out.push_back(
			std::make_shared<builtin_operator>("[]", std::vector<token>({token("list"), token("a"), token("["), token("int"), token("b"), token("]")}), "builtin_index_list_string", std::vector<type>({type::string}),
											   builtin_index_list, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("[]", std::vector<token>({token("list"), token("a"), token("["), token("int"), token("b"), token("]")}), "builtin_index_list_int", std::vector<type>({type::integer}),
											   builtin_index_list, 99, generate_new_scope())
	);


	out.push_back(
			std::make_shared<builtin_operator>("=", std::vector<token>({token("list"), token("a"), token("="), token("list"), token("b")}), "builtin_assign_list", std::vector<type>({type::list}),
											   builtin_assign_list, 99, generate_new_scope())
	);

	out.push_back(
			std::make_shared<builtin_operator>("=", std::vector<token>({token("list"), token("a"), token("="), token("list"), token("b")}), "builtin_assign_list", std::vector<type>({type::list}),
											   builtin_assign_list, 99, generate_new_scope())
	);


	return out;
}


std::shared_ptr<mycelium::variable> builtin_if_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args, std::shared_ptr<mycelium::variable>& return_value) {
	if (args[1]->get_value()->value) {
		std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
		args[0]->get_value()->fn_ptr->call(inside_args, return_value);
		return mycelium::constant::make_constant(true);
	}
	return mycelium::constant::make_constant(false);
}

std::shared_ptr<mycelium::variable> builtin_while_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args, std::shared_ptr<mycelium::variable>& return_value) {
	bool out = (args[1]->get_value()->value);
	std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
	while (args[1]->get_value()->value) {
		args[0]->get_value()->fn_ptr->call(inside_args, return_value);
	}
	return mycelium::constant::make_constant(out);
}

std::shared_ptr<mycelium::variable> builtin_for_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args, std::shared_ptr<mycelium::variable>& return_value) {
	// Execute the first statement
	args[1]->get_value();
	// Check the second
	bool out = (args[2]->get_value()->value);
	std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
	while (args[2]->get_value()->value) {
		args[0]->get_value()->fn_ptr->call(inside_args, return_value);
		args[3]->get_value();
	}
	return mycelium::constant::make_constant(out);
}

std::shared_ptr<mycelium::variable> builtin_for_index_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args, std::shared_ptr<mycelium::variable>& return_value) {
	std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
	bool out = false;
	for (int i = args[2]->get_value()->value; i < args[3]->get_value()->value; i++) {
		std::static_pointer_cast<mycelium::variable>(args[1]->get_value())->value = i;
		args[0]->get_value()->fn_ptr->call(inside_args, return_value);
		out = true;
	}
	return mycelium::constant::make_constant(out);
}


std::shared_ptr<mycelium::variable> builtin_for_list_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args, std::shared_ptr<mycelium::variable>& return_value) {
	std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
	bool out = false;
	for (auto& i : *args[2]->get_value()->list_ptr) {
		std::static_pointer_cast<mycelium::variable>(args[1]->get_value())->set_value(i->get_value());
		args[0]->get_value()->fn_ptr->call(inside_args, return_value);
		out = true;
	}
	return mycelium::constant::make_constant(out);
}

std::vector<std::shared_ptr<mycelium::conditional>> mycelium::parser::create_base_conditionals() {
	std::vector<std::shared_ptr<conditional>> out;

	// const std::string& op_token, const mycelium::pattern_match& pattern_match, const std::string& name, const std::vector<mycelium::type>& ret, std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int scope

	out.push_back(
		std::make_shared<mycelium::builtin_conditional>("if", std::vector<mycelium::type>({type::boolean}),
														builtin_if_conditional, generate_new_scope())
	);

	out.push_back(
			std::make_shared<mycelium::builtin_conditional>("while", std::vector<mycelium::type>({type::boolean}),
															builtin_while_conditional, generate_new_scope())
	);


	out.push_back(
			std::make_shared<mycelium::builtin_conditional>("for", std::vector<token>({token("int"), token("a"), token("in"), token("int"), token("b"), token(".."), token("int"), token("c")}),
															builtin_for_index_conditional, generate_new_scope())
	);


	out.push_back(
			std::make_shared<mycelium::builtin_conditional>("for", std::vector<token>({token("int"), token("a"), token("in"), token("list"), token("b")}),
															builtin_for_list_conditional, generate_new_scope())
	);


	out.push_back(
			std::make_shared<mycelium::builtin_conditional>("for", std::vector<token>({token("string"), token("a"), token("in"), token("list"), token("b")}),
															builtin_for_list_conditional, generate_new_scope())
	);

	return out;
}
