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


void mycelium::parser::throw_error(const std::string &error, const token& tk) {
	std::cerr << "ERROR on line " << tk.line << ": "  << error << std::endl;
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

mycelium::pattern_token mycelium::parser::get_pattern_token(const mycelium::token& tk) {
	if (tk.type == ttype) {
		/// Still not sure what the right thing to do about types is but
		/// I think we should just treat them like string in this case
		   return pattern_token(tk.string);
	} else if (tk.type == word) {
		std::shared_ptr<variable> var = get_word_variable(tk);
		if (var.get()) {
			return pattern_token(var);
		} else {
			return pattern_token(tk.string);
		}
	} else if (tk.type == num) {
		return pattern_token(constant::make_constant(std::stoi(tk.string)));
	} else if (tk.type == string_literal) {
		return pattern_token(constant::make_constant(tk.string));
	} else if (tk.type == grouping) {
		/// FIXME: Handle Parentheses In Operator
//    tokenizer.current_token_index--;
//    std::vector<std::shared_ptr<operator_use>> ops = find_ops();
//    for (auto& new_op : ops) {
//        pattern.pattern.emplace_back(new_op);
//    }
		return pattern_token(tk.string);
	} else {
		return pattern_token(tk.string);
	}
}

mycelium::pattern_match mycelium::parser::get_pattern_from_tokens(int number_of_tokens) {
	pattern_match pattern;
	token current_type;
	while (pattern.pattern.size() < number_of_tokens) {
		pattern_token new_token = get_pattern_token(tokenizer.get_next_token());
		if (!new_token.is_expression && new_token.oper.empty()) {
			// Return when we have an empty token because that means we have an invalid token e.g. a type
			return {};
		}
		pattern.pattern.emplace_back(new_token);
	}

	return pattern;
}

mycelium::pattern_match mycelium::parser::get_pattern_from_tokens(int start_index, int end_index) {
	pattern_match pattern;
	for (int current_token_index = start_index; current_token_index < end_index; current_token_index++) {
		pattern.pattern.push_back(get_pattern_token(tokenizer.tokens[current_token_index]));
	}

	return pattern;
}

mycelium::pattern_match mycelium::parser::get_pattern_from_tokens(const std::vector<token>& tks) {
	pattern_match pattern;
	for (auto& tk : tks) {
		pattern.pattern.push_back(get_pattern_token(tk));
	}

	return pattern;
}

std::vector<std::shared_ptr<mycelium::operator_use>> mycelium::parser::find_ops() {
	std::vector<std::shared_ptr<operator_use>> out;
	int pushed_pos;
	for (auto &op: operators) {
		if ((tokenizer.current_token_index + op->args.pattern.size()) > tokenizer.tokens.size()) {
			continue;
		}
		pushed_pos = tokenizer.current_token_index;

		pattern_match pattern = get_pattern_from_tokens(op->args.pattern.size());

		if (op->args.is_match(pattern)) {
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
	int last_found_operator_index = tokenizer.current_token_index;
	bool found_op = false;
	for (; tokenizer.current_token_index < start_index + number_of_tokens; tokenizer.current_token_index++) {
		std::vector<std::shared_ptr<operator_use>> found_ops = find_ops();

		if (!found_ops.empty()) {
			last_found_operator_index = tokenizer.current_token_index + found_ops.back()->args.size() + 1;
			found_op = true;
			operator_uses.insert(operator_uses.end(), found_ops.begin(), found_ops.end());

			std::sort(operator_uses.begin(), operator_uses.end(),
					  [](const std::shared_ptr<operator_use> &a, const std::shared_ptr<operator_use> &b) -> bool {
						  return std::static_pointer_cast<operatr>(a->op)->priority >
								 std::static_pointer_cast<operatr>(b->op)->priority;
					  });
		}
		else if (!found_op) {
			// If we don't find anything the first time we want to stop.
			break;
		}
	}
	for (int i = 1; i < operator_uses.size(); i++) {
		operator_uses[i - 1]->args.back() = operator_uses[i];
	}

	tokenizer.current_token_index = last_found_operator_index;
	if (operator_uses.empty()) {
		return {};
	}


	// Offset the position by the size of the last operator minus the distance from the start of the operator call to the first operator token plus 1 to include the operator. For example:
	//           1 + 2
	// We're here    ^
	// We want to take the length (3) and subtract the operator offset (1) and subtract 1 for the operator.
	//           1 + 2
	// We end here     ^
	// This means that we ignore things we already parsed
//    tokenizer.current_token_index += operator_uses.back()->op->args.pattern.size() - operator_uses.back()->op->operator_offset - 1;

	// Return the first operator which will recursively call the others
	return operator_uses.front();
}

std::shared_ptr<mycelium::expression> mycelium::parser::get_expression_from_tokens(const std::vector<token>& tks) {


	if (tks.empty()) {
		/// If we have no token then warn and return nothing indicating the expression is invalid
		warn("No tokens passed to get_expression", token());
		return {};
	}

	if (tks.size() == 1) {
		/// If all we have is a single token we want to return that as either a variable or nothing
		auto pt = get_pattern_token(tks[0]);
		if (pt.is_expression) {
			return pt.expr;
		}
		return {};
	}

	if (tks.front().string == "(" && tks.back().string == ")") {
		int search_depth = 0;
		std::string opening_grouping = "(";
		std::string closing_grouping = ")";

		std::vector<token> out;
		int current_token_index = 1;
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

	for (auto& op : operators) {
		pattern_match op_use_pattern = this->generate_pattern_from_function(op, tks);
		if (op->args.is_match(op_use_pattern)) {
			return std::make_shared<operator_use>(op, op_use_pattern.get_expressions());
		}
	}

	/// If no matching operators are found we return an empty expression.
	return {};
}

std::shared_ptr<mycelium::parsed_token> mycelium::parser::parse_token() {
	mycelium::token current_token = tokenizer.get_next_token();

	mycelium::token next_token;
	if (tokenizer.has_next_token()) {
		next_token = tokenizer.get_next_token_without_increment();
	}


	// Check for operator on every token.
	std::vector<token> remaining_line_tokens = tokenizer.tokens_until_newline();
	std::shared_ptr<expression> op = get_expression_from_tokens(remaining_line_tokens);

	if (op) {
		tokenizer.skip_to_newline();
		return op;
	}

	switch (current_token.type) {
		case token_type::op:
			throw_error("Unknown Operator: " + tokenizer.lines[current_token.line - 1], current_token);
		case keyword:
			if (next_token.type == token_type::invalid) {
				throw_error("Keyword with nothing following found", current_token);
			}

			throw_error("Invalid keyword use", current_token);
			break;
		case word:
		case num:
		case string_literal:
			return parse_expression();
		case ttype: {
			int type = type::validate_type(current_token);

			if (next_token.type != token_type::word) {
				throw_error("Type with no following variable name found", current_token);
			}

			std::shared_ptr<variable> var = parse_variable(type);
			if (show_debug_lines) {
				std::cout << "Got variable: " << var->type.name << ": " << var->token.string << "\n";
			}

			/// Check again for operators
			remaining_line_tokens = tokenizer.tokens_until_newline();
			op = get_expression_from_tokens(remaining_line_tokens);

			if (op) {
				tokenizer.skip_to_newline();
				return op;
			}

			/// If we don't find an operator after the variable declaration skip checking the name of the variable
			tokenizer.current_token_index++;

			/// Don't return the variable declaration token because we already created the variable.
			return {};
		}
		case grouping: {
			tokenizer.current_token_index--;
			std::vector<token> tokens_inside_grouping = tokenizer.get_tokens_inside_grouping();
			std::shared_ptr<expression> op = get_expression_from_tokens(tokens_inside_grouping);

			for (auto& tk : tokens_inside_grouping) {
				std::cout << "\"" << tk.string << "\" ";
			}

			std::cout << op->to_string();

			if (op) {
				std::cout << " = " << op->get_value()->get_as_string() << "\n";
				tokenizer.skip_to_newline();
				return op;
			}
			else {
				throw_error("Unknown Value Inside Grouping", current_token);
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
												   mycelium::pattern_match::create_from_tokens(args), scope);

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

	token next_token = tokenizer.get_next_non_whitespace_token();

	if (next_token.string == "<") {
		for (auto& token : find_in_grouping(tokenizer.current_token_index, "<", ">")) {
			if (show_debug_lines) {
				std::cout << "ret: " << token.string << std::endl;
			}
			ret.push_back(token);
		}
	}
	else {
		throw_error("operator definitions must contain a context", keyword_token);
	}

	next_token = tokenizer.get_next_token();

	if (next_token.string == "<") {
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
		tokenizer.current_token_index--;
	}

	pattern_match context_pattern = pattern_match::create_from_tokens(context);

	/// TODO: Get priority
	std::shared_ptr<operatr> out = std::make_shared<mycelium::operatr>(token(), context, context_pattern, name, type::convert_tokens_to_types(ret), 1, scope);

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
	// Skip all found tokens along with the closing grouping token
	tokenizer.current_token_index = index + 1;
	return out;
}

void mycelium::parser::find_function_declarations() {

	// Find all the declarations for functions
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

	// Loop over all the new functions and generate their body code. Doing things in this order allows for function
	// definitions to be in any order without forward declarations in the file
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

mycelium::pattern_match mycelium::parser::generate_pattern_from_function(const std::shared_ptr<mycelium::function_base>& fn, const std::vector<mycelium::token>& tks) {
	if (fn->args.pattern.empty()) {
		if (tks.empty()) {
			return {};
		}
		else {
			/// Create a pattern with something in it so that when we try to match against nothing it fails
			pattern_match out;
			out.pattern.emplace_back("Invalid Pattern");
			return out;
		}
	}

	std::vector<std::vector<token>> landmark_chunks;
	std::vector<int> desired_chunk_sizes = {0};
	int number_of_chunks = 1;
	std::vector<token> landmarks;

	int landmark_chunk_index = 0;
	for (auto & arg : fn->args.pattern) {
		if (!arg.is_expression) {
			desired_chunk_sizes.push_back(0);
			landmarks.emplace_back(arg.oper);
			landmark_chunks.emplace_back();
			for (; tks[landmark_chunk_index].string != arg.oper && landmark_chunk_index < tks.size(); landmark_chunk_index++) {
				landmark_chunks.back().push_back(tks[landmark_chunk_index]);
			}

			/// If we don't find the landmark this pattern is invalid.
			if (tks[landmark_chunk_index].string != arg.oper) {
				return {};
			}

			if (&arg != &fn->args.pattern.back()) {
				number_of_chunks++;
			}
		}
		else {
			desired_chunk_sizes.back()++;
		}
	}

	/// If there are no landmark use all the tokens to create one expression
	if (landmarks.empty()) {
		landmark_chunks.emplace_back();
		pattern_match out;
		std::shared_ptr<expression> expr = get_expression_from_tokens(tks);
		if (expr) {
			out.pattern.emplace_back(expr);
		}
		return out;
	}

	/// If the last pattern token is a land mark then we don't want get the last chunk as there isn't one
	if (landmark_chunk_index + 1 < tks.size()) {
		/// Get the last tokens into a chunk if there is one
		landmark_chunks.emplace_back();
		for (int i = landmark_chunk_index + 1; i < tks.size(); i++) {
			landmark_chunks.back().push_back(tks[i]);
		}
	}

	if (number_of_chunks != landmark_chunks.size()) {
		return {};
	}

	std::vector<std::shared_ptr<expression>> landmark_chunk_expressions;

	for (int i = 0; i < landmark_chunks.size(); i++) {
		if (desired_chunk_sizes[i] == landmark_chunks[i].size()) {
			for (auto& tk : landmark_chunks[i]) {
				auto pt = get_pattern_token(tk);
				if (pt.is_expression) {
					landmark_chunk_expressions.push_back(pt.expr);
				}
				else {
					/// If we get an invalid expression then the pattern is not valid
					return {};
				}
			}
		}
		else {
			std::shared_ptr<expression> expr = get_expression_from_tokens(landmark_chunks[i]);
			if (!expr) {
				/// If we get an invalid expression then the pattern is not valid
				return {};
			}

			landmark_chunk_expressions.push_back(expr);
		}
	}

	pattern_match out;

	int expr_index = 0;
	for (int i = 0; i < landmark_chunk_expressions.size(); i++) {
		for (int j = 0; j < desired_chunk_sizes[i]; j++) {
			out.pattern.emplace_back(landmark_chunk_expressions[expr_index++]);
		}
		if (i < landmarks.size()) {
			out.pattern.emplace_back(landmarks[i].string);
		}
	}

	return out;
}

std::shared_ptr<mycelium::expression> mycelium::parser::parse_expression() {
	tokenizer.current_token_index--;
	mycelium::token next_token = tokenizer.get_next_token();

	if (!tokenizer.has_next_token() || tokenizer.get_next_token_without_increment().string != "(") {
		auto var = get_word_variable(next_token);
		if (!var) {
			var = get_constant(next_token);
			if (!var) {
				throw_error("Unknown word \"" + next_token.string + "\"", next_token);
			}
		}
		std::shared_ptr<expression> op = find_ops_in(tokenizer.num_tokens_until_newline());
		if (op) {
			return op;
		}
		else if (var) {
			return var;
		}
		else {
			throw_error("Unknown variable \"" + next_token.string + "\"", next_token);
			return {};
		}
	}

	/// TODO: Change this to work like operators:
	///     Loop through all the functions and check if this could be a match
	///     Using this we can handle things like expression inside of patterns
	///     ...
	///     Although expression inside of patterns don't work for operators they
	///     just call more operators. I think the current system works fine for now
	int close_grouping_index = tokenizer.get_ending_grouping_token_index();

	std::vector<token> inside_grouping;

	for (int i = tokenizer.current_token_index; i < close_grouping_index; i++) {
		inside_grouping.push_back(tokenizer.tokens[i]);
	}

	std::vector<std::shared_ptr<function>> funcs;
	std::vector<pattern_match> func_call_patterns;

	for (auto& fn : functions) {
		if (fn->name == next_token) {
			pattern_match fn_call_pattern = this->generate_pattern_from_function(fn, inside_grouping);
			if (fn->args.is_match(fn_call_pattern)) {
				funcs.push_back(fn);
				func_call_patterns.push_back(fn_call_pattern);
			}
		}
	}


	tokenizer.current_token_index = close_grouping_index + 1;
	if (funcs.empty()) {
		std::vector<std::shared_ptr<conditional>> conds;
		std::vector<pattern_match> cond_call_patterns;
		for (auto& cn : conditionals) {
			if (cn->name == next_token) {
				pattern_match cn_call_pattern = this->generate_pattern_from_function(cn, inside_grouping);
				if (cn->args.is_match(cn_call_pattern)) {
					conds.push_back(cn);
					cond_call_patterns.push_back(cn_call_pattern);
				}
			}
		}

		if (!conds.empty()) {
			// Assume there is only one match for conditionals
			if (conds.size() != 1) {
				// Only output the first two because
				throw_error("Ambiguous Function Call Between " + conds[0]->to_string() + " and " + conds[1]->to_string(), next_token);
			}
			std::shared_ptr<conditional> cn = conds[0];
			if (this->tokenizer.get_next_token_without_increment().string == "{") {
				int starting_body_location = tokenizer.current_token_index;
				int ending_body_location = tokenizer.get_ending_grouping_token_index();
				std::shared_ptr<mycelium::scope> pushed_scope = current_scope;
				std::shared_ptr<mycelium::scope> body_scope = generate_new_scope();
				change_scope(body_scope);
				std::vector<std::shared_ptr<parsed_token>> call_body = parse_tokens(starting_body_location + 1, ending_body_location - 1);
				change_scope(pushed_scope);
				tokenizer.current_token_index = ending_body_location + 1;

				/// Create an anonymous function with body of the conditional statement and pass that as the first argument
				std::vector<std::shared_ptr<expression>> cn_call_expressions({function::make_anonymous_function(call_body, {}, {}, body_scope)});
				std::vector<std::shared_ptr<expression>> cn_pattern_expressions = cond_call_patterns[0].get_expressions();
				cn_call_expressions.insert(cn_call_expressions.end(), cn_pattern_expressions.begin(), cn_pattern_expressions.end());

				return std::make_shared<conditional_call>(cn, cn_call_expressions, body_scope);
			}
			else {
				throw_error("Conditional Call Must Have Body", next_token);
			}
		}
		else {
			pattern_match fn_call_pattern = get_pattern_from_tokens(inside_grouping);
			std::string pattern_string;
			for (auto &arg: fn_call_pattern.pattern) {
				if (arg.is_expression) {
					pattern_string += arg.expr->get_type().name + ' ';
				} else {
					pattern_string += arg.oper + ' ';
				}
			}
			throw_error("Unknown function or conditional \"" + next_token.string + "(" + pattern_string + ")", next_token);
		}
	}
	else if (funcs.size() == 1) {
		return std::make_shared<function_call>(funcs[0], func_call_patterns[0].get_expressions());
	}
	else {
		/// TODO: Decide if this should be an error
		///         Having it just assume that you want the pattern match version could be very confusing.
		///         It would probably be best to make it an error to declare an ambiguous functions and
		///         then we won't have to deal with it here.
		int longest_call_index = 0;
		for (int i = 0; i < funcs.size(); i++) {
			if ((funcs[longest_call_index]->args.pattern.size() < funcs[i]->args.pattern.size())) {
				longest_call_index = i;
				continue;
			}

			if (funcs[longest_call_index]->args.pattern.size() == funcs[i]->args.pattern.size()) {
				throw_error("Ambiguous Function Call Between " + funcs[longest_call_index]->to_string() + " and " + funcs[i]->to_string(), next_token);
			}
		}
		return std::make_shared<function_call>(function_call(funcs[longest_call_index], func_call_patterns[longest_call_index].get_expressions()));
	}

	return {};
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
		if (mycelium::show_debug_lines) {
			std::cout << "Calling main function:\n";
			for (auto &pt: main->body) {
				if (pt.get()) {
					std::cout << pt->to_string() << "\n";
				}
			}
		}
		main->call(args);
	}
}

std::shared_ptr<mycelium::variable> builtin_print(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	if (!args.empty()) {
		for (auto& arg : args) {
			std::cout << arg->get_as_string();
			if (&arg != &args.back()) {
				std::cout << ' ';
			}
		}
	}

	return mycelium::constant::make_constant(0);
}

std::shared_ptr<mycelium::variable> builtin_println(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	if (!args.empty()) {
		for (auto& arg : args) {
			std::cout << arg->get_as_string();
			if (&arg != &args.back()) {
				std::cout << ' ';
			}
		}
	}
	std::cout << "\n";

	return mycelium::constant::make_constant(0);
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
			std::make_shared<builtin_function>("println", std::vector<type>({}), std::vector<type>({}), builtin_println, generate_new_scope())
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
	args[0]->str = args[1]->str;
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

std::shared_ptr<mycelium::variable> builtin_multiply_equals_int(std::vector<std::shared_ptr<mycelium::variable>>& args) {
	args[0]->value = args[0]->value * args[1]->value;

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
			std::make_shared<builtin_operator>("=", std::vector<token>({token("string"), token("a"), token("="), token("string"), token("b")}), "builtin_assign_string", std::vector<type>({type::string}), builtin_assign_string, 99, generate_new_scope())
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


	return out;
}


std::shared_ptr<mycelium::variable> builtin_if_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args) {
	if (args[1]->get_value()->value) {
		std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
		args[0]->get_value()->fn_ptr->call(inside_args);
		return mycelium::constant::make_constant(true);
	}
	return mycelium::constant::make_constant(false);
}

std::shared_ptr<mycelium::variable> builtin_while_conditional(std::vector<std::shared_ptr<mycelium::expression>>& args) {
	bool out = (args[1]->get_value()->value);
	std::vector<std::shared_ptr<mycelium::variable>> inside_args({});
	while (args[1]->get_value()->value) {
		args[0]->get_value()->fn_ptr->call(inside_args);
	}
	return mycelium::constant::make_constant(out);
}

std::vector<std::shared_ptr<mycelium::conditional>> mycelium::parser::create_base_conditionals() {
	std::vector<std::shared_ptr<conditional>> out;

	// const std::string& op_token, const mycelium::pattern_match& pattern_match, const std::string& name, const std::vector<mycelium::type>& ret, std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int scope

	std::shared_ptr<mycelium::scope> builtin_scope = generate_new_scope();

	out.push_back(
		std::make_shared<mycelium::builtin_conditional>("if", std::vector<mycelium::type>({type::boolean}),
														builtin_if_conditional, builtin_scope)
	);

	out.push_back(
			std::make_shared<mycelium::builtin_conditional>("while", std::vector<mycelium::type>({type::boolean}),
															builtin_while_conditional, builtin_scope)
	);

	return out;
}
