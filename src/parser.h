//
// Created by moss on 3/30/22.
//
#pragma once

#include <utility>

#include "base.h"
#include "tokenizer.h"

namespace mycelium {

	template <typename T>
	int vector_find(const std::vector<T>& vec, const T& search);

	class parser {

	public:

		std::vector<std::shared_ptr<mycelium::function>> functions;
		std::vector<std::shared_ptr<mycelium::operatr>> operators;
		std::vector<std::shared_ptr<mycelium::conditional>> conditionals;
		std::vector<std::shared_ptr<mycelium::scope>> scopes = {};

		std::shared_ptr<scope> global_scope;

		std::shared_ptr<scope> current_scope;

		std::shared_ptr<function_base> current_parsing_function;

		mycelium::tokenizer tokenizer;
		mycelium::token search_type;
		std::string searching_for;
		std::vector<std::shared_ptr<mycelium::parsed_token>> temp = {};
		std::vector<std::shared_ptr<mycelium::parsed_token>> parsed_tokens = {};

		void parse();

		std::vector<std::shared_ptr<mycelium::function>> create_base_functions();

		std::vector<std::shared_ptr<mycelium::operatr>> create_base_operators();

		std::vector<std::shared_ptr<mycelium::conditional>> create_base_conditionals();

		explicit parser(mycelium::tokenizer tokenizer) : tokenizer(std::move(tokenizer)), search_type() {
			global_scope = std::make_shared<scope>(nullptr);
			current_scope = global_scope;
			functions = this->create_base_functions();
			operators = this->create_base_operators();
			conditionals = this->create_base_conditionals();
		}

		std::shared_ptr<parsed_token> parse_token();

		std::vector<std::shared_ptr<mycelium::parsed_token>> parse_tokens(int start, int end);

		void find_function_declarations();

		std::shared_ptr<mycelium::function> parse_function(bool get_body = false);

		std::shared_ptr<mycelium::operatr> parse_operator(bool get_body = false);

		std::shared_ptr<mycelium::conditional> parse_cond(bool get_body = false);

		std::vector<token> find_in_grouping(int& index, const std::string &open, const std::string &close);

		std::shared_ptr<mycelium::return_from_function> parse_return();

		std::shared_ptr<mycelium::variable> parse_variable(int variable_type);

		std::vector<mycelium::token> get_tokens_in_curlies(const std::vector<token>& tks, int search_index);

		std::vector<mycelium::token> get_tokens_in_curlies(const std::vector<token>& tks, int search_index, int& start_parentheses, int& end_parentheses);

		std::vector<mycelium::token> get_tokens_in_parentheses(const std::vector<token>& tks, int search_index);

		std::vector<mycelium::token> get_tokens_in_parentheses(const std::vector<token>& tks, int search_index, int& start_parentheses, int& end_parentheses);

		std::shared_ptr<mycelium::expression> get_object_function(const std::vector<token>& tks, int index);

		std::shared_ptr<mycelium::expression> get_function(const std::vector<token>& tks, int index);

		std::shared_ptr<mycelium::expression> get_function(const std::vector<token>& tks, int index, const std::vector<std::shared_ptr<function_base>>& search_functions);

		std::shared_ptr<mycelium::expression> parse_expression();

		std::shared_ptr<mycelium::variable> get_word_variable(const mycelium::token& word);

		std::shared_ptr<mycelium::constant> get_constant(const mycelium::token& word);

		std::shared_ptr<mycelium::scope> generate_new_scope();

		void change_scope(const std::shared_ptr<scope>& new_scope, bool delete_previous_scope = true);

		std::shared_ptr<mycelium::variable> get_variable(const std::string& name);

		void execute();

		std::vector<std::shared_ptr<operator_use>> find_ops();

		std::shared_ptr<mycelium::expression> find_ops_in(int number_of_tokens);

		pattern_match get_pattern_from_tokens(int number_of_tokens);

		pattern_token get_pattern_token(const std::vector<mycelium::token>& tks, int& index);

		pattern_match get_pattern_from_tokens(int start_index, int end_index);

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
