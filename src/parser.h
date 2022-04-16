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

		std::vector<mycelium::function_base> functions = {};

		enum state {
			idle,
			searching
		};


		mycelium::tokenizer tokenizer;
		state state;
		mycelium::token search_type;
		std::string searching_for;
		std::vector<mycelium::parsed_token> temp = {};
		std::vector<mycelium::parsed_token> parsed_tokens = {};

		void parse();

		int parse_keyword_token(const parsed_token&, int index);

		explicit parser(mycelium::tokenizer tokenizer) : tokenizer(std::move(tokenizer)), state(idle), search_type({}) {}

		parsed_token parse_token(int &index);

		std::vector<parsed_token> parse_func_body(int &index);

		function parse_function(int &index);

		function parse_operator(int &index);

		function parse_cond(int &index);
	};
}