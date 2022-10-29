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
		std::vector<std::shared_ptr<mycelium::conditional>> conditionals = {};
        std::vector<std::shared_ptr<mycelium::variable>> variables = {};

		int global_scope = 0;

		int current_scope;

        int last_found_operator_index = tokenizer.tokens.size();

        std::vector<bool> scopes = std::vector<bool>(2048);

		mycelium::tokenizer tokenizer;
		mycelium::token search_type;
		std::string searching_for;
		std::vector<std::shared_ptr<mycelium::parsed_token>> temp = {};
		std::vector<std::shared_ptr<mycelium::parsed_token>> parsed_tokens = {};

		void parse();

        std::vector<std::shared_ptr<mycelium::function>> create_base_functions();

        std::vector<std::shared_ptr<mycelium::operatr>> create_base_operators();

        explicit parser(mycelium::tokenizer tokenizer) : tokenizer(std::move(tokenizer)), search_type() {
			current_scope = global_scope;
            functions = this->create_base_functions();
            operators = this->create_base_operators();
		}

		std::shared_ptr<parsed_token> parse_token();

        std::vector<std::shared_ptr<mycelium::parsed_token>> parse_tokens(int start, int end);

		void find_function_declarations();

		std::shared_ptr<mycelium::function> parse_function(bool get_body = false);

		std::shared_ptr<mycelium::operatr> parse_operator(bool get_body = false);

		std::shared_ptr<mycelium::conditional> parse_cond(bool get_body = false);

		std::vector<token> find_in_grouping(int& index, const std::string &open, const std::string &close);

		std::shared_ptr<mycelium::variable> parse_variable(int variable_type);

        std::shared_ptr<mycelium::expression> parse_expression();

        std::shared_ptr<mycelium::function> get_word_function(const mycelium::token& word, const std::vector<type>& args_types);

        std::shared_ptr<mycelium::variable> get_word_variable(const mycelium::token& word);

        std::shared_ptr<mycelium::constant> get_constant(const mycelium::token& word);

        pattern_match create_from_known_variables(int num_of_tokens);

        int generate_new_scope();

        void change_scope(int new_scope, bool delete_previous_scope = true);

        std::shared_ptr<mycelium::variable> get_variable(const std::string& name);

        void execute();

        std::vector<std::shared_ptr<operator_use>> find_ops();

        std::shared_ptr<mycelium::expression> find_ops_in(int number_of_tokens);
    };
}