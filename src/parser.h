//
// Created by moss on 3/30/22.
//
#pragma once

#include <utility>

#include "base.h"
#include "tokenizer.h"
#include "function.h"

namespace mycelium {

	template <typename T>
	int vector_find(const std::vector<T>& vec, const T& search);

	class parser {

	public:

		std::vector<std::shared_ptr<mycelium::function>> functions;
		std::vector<std::shared_ptr<mycelium::operatr>> operators;
		std::vector<std::shared_ptr<mycelium::conditional>> conditionals;
		std::vector<std::shared_ptr<mycelium::scope>> scopes = {};

		int number_of_builtin_functions;
		int number_of_builtin_operators;
		int number_of_builtin_conditionals;

		std::shared_ptr<scope> global_scope;

		std::shared_ptr<scope> current_scope;

		std::shared_ptr<function_base> current_parsing_function;

		static std::vector<std::string> program_args;

		mycelium::tokenizer tokenizer;
		std::vector<std::shared_ptr<mycelium::parsed_token>> temp = {};
		std::vector<std::shared_ptr<mycelium::parsed_token>> parsed_tokens = {};

		void parse();

		std::vector<std::shared_ptr<mycelium::function>> create_base_functions();

		std::vector<std::shared_ptr<mycelium::operatr>> create_base_operators();

		std::vector<std::shared_ptr<mycelium::conditional>> create_base_conditionals();

		explicit parser(mycelium::tokenizer tokenizer, std::vector<std::string> args) : tokenizer(std::move(tokenizer)) {
			global_scope = std::make_shared<scope>(nullptr);
			current_scope = global_scope;
			functions = this->create_base_functions();
			number_of_builtin_functions = functions.size();
			operators = this->create_base_operators();
			number_of_builtin_operators = operators.size();
			conditionals = this->create_base_conditionals();
			number_of_builtin_conditionals = conditionals.size();

			program_args = args;
		}

		std::shared_ptr<parsed_token> parse_token(const std::vector<std::shared_ptr<parsed_token>>& previous_tokens, const std::vector<token>& tokens, size_t& index);

		std::vector<std::shared_ptr<mycelium::parsed_token>> parse_tokens(const std::vector<token>& tokens);

		std::vector<std::shared_ptr<mycelium::parsed_token>> parse_tokens(size_t start, size_t end);

		void find_function_declarations();

		std::shared_ptr<mycelium::function> parse_function(bool get_body = false);

		std::shared_ptr<mycelium::operatr> parse_operator(bool get_body = false);

		std::shared_ptr<mycelium::conditional> parse_cond(bool get_body = false);

		std::shared_ptr<mycelium::return_from_function> parse_return(const std::vector<token>& tokens, size_t& index);

		std::shared_ptr<mycelium::variable> parse_variable(const std::vector<token>& tokens, size_t& index, int variable_type);

		std::vector<mycelium::token> get_tokens_until_newline(const std::vector<token>& tokens, size_t index);

		void skip_to_newline(const std::vector<token>& tokens, size_t& index);

		void skip_function_definition(const std::vector<token>& tokens, size_t& index);

		std::vector<mycelium::token> get_tokens_in_grouping(const std::vector<token>& tks, size_t search_index, size_t& open_token_index, size_t& close_token_index, const std::string& open_token_string, const std::string& close_token_string);

		std::vector<mycelium::token> get_tokens_in_angle_brackets(const std::vector<token>& tks, size_t search_index);

		std::vector<mycelium::token> get_tokens_in_angle_brackets(const std::vector<token>& tks, size_t search_index, size_t& open_angle_index, size_t& close_angle_index);

		std::vector<mycelium::token> get_tokens_in_curlies(const std::vector<token>& tks, size_t search_index);

		std::vector<mycelium::token> get_tokens_in_curlies(const std::vector<token>& tks, size_t search_index, size_t& open_curly_index, size_t& close_curly_index);

		std::vector<mycelium::token> get_tokens_in_parentheses(const std::vector<token>& tks, size_t search_index);

		std::vector<mycelium::token> get_tokens_in_parentheses(const std::vector<token>& tks, size_t search_index, size_t& open_parentheses_index, size_t& close_parentheses_index);

		std::shared_ptr<mycelium::expression> get_object_function(const std::shared_ptr<expression>& object, const std::vector<token>& function_tokens, const std::vector<token>& other_tokens);

		std::shared_ptr<mycelium::expression> get_object_function(const std::vector<token>& tks, size_t index);

		std::shared_ptr<mycelium::expression> get_function(const std::vector<token>& tks, size_t& index);

		std::shared_ptr<mycelium::expression> get_function(const std::vector<token>& tks, size_t& index, const std::vector<std::shared_ptr<function_base>>& search_functions);

		std::shared_ptr<mycelium::expression> parse_expression(const std::vector<token>& tokens, size_t& index);

		std::shared_ptr<mycelium::variable> get_word_variable(const mycelium::token& word);

		std::shared_ptr<mycelium::constant> get_constant(const mycelium::token& word);

		std::shared_ptr<mycelium::scope> generate_new_scope();

		void change_scope(const std::shared_ptr<scope>& new_scope, bool delete_previous_scope = true);

		std::shared_ptr<mycelium::variable> get_variable(const std::string& name);

		void execute();

		std::vector<std::shared_ptr<operator_use>> find_ops();

		std::shared_ptr<mycelium::expression> find_ops_in(size_t number_of_tokens);

		pattern_match get_pattern_from_tokens(size_t number_of_tokens);

		std::shared_ptr<pattern_token> get_pattern_token(const std::vector<mycelium::token>& tks, size_t& index);

		pattern_match get_pattern_from_tokens(size_t start_index, size_t end_index);

		void throw_error(const std::string& error, const token& token);

		bool can_match_pattern(const pattern_match& match, const pattern_match& other);

		static void warn(const std::string &warning, const token &tk);

		pattern_match generate_pattern_from_function(const std::shared_ptr<mycelium::function_base> &fn,
					       const std::vector<mycelium::token> &tks);

		pattern_match generate_pattern_from_function(const std::shared_ptr<mycelium::function_base> &fn,
					       const mycelium::token& tk);

		std::shared_ptr<expression> get_expression_from_single_token(const token &tk);

		std::shared_ptr<expression> get_expression_from_tokens(const std::vector<token> &tks);

		pattern_match get_pattern_from_tokens(const std::vector<token> &tks);

	};
}
