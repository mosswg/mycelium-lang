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

		std::vector<std::shared_ptr<mycelium::function>> functions = {};
		std::vector<mycelium::operatr> operators = {};
		std::vector<mycelium::conditional> conditionals = {};

		scope global_scope;

		scope* current_scope;

		enum state {
			idle,
			searching
		};


		mycelium::tokenizer tokenizer;
		state state;
		mycelium::token search_type;
		std::string searching_for;
		std::vector<std::shared_ptr<mycelium::parsed_token>> temp = {};
		std::vector<std::shared_ptr<mycelium::parsed_token>> parsed_tokens = {};

		void parse();

		explicit parser(mycelium::tokenizer tokenizer) : tokenizer(std::move(tokenizer)), state(idle), search_type(), global_scope(nullptr) {
			current_scope = &global_scope;
		}

		std::shared_ptr<parsed_token> parse_token();

        std::vector<std::shared_ptr<mycelium::parsed_token>> parse_tokens(int start, int end);

		void find_function_declarations();

		std::shared_ptr<mycelium::function> parse_function(bool get_body = false);

		std::shared_ptr<mycelium::operatr> parse_operator(bool get_body = false);

		std::shared_ptr<mycelium::conditional> parse_cond(bool get_body = false);

		std::vector<token> find_in_grouping(int& index, const std::string &open, const std::string &close);

		static int validate_type(const token& type);

		std::shared_ptr<mycelium::variable> parse_variable(int variable_type);
	};
}