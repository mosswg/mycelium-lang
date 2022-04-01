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

	class parsed_token {
	public:
		mycelium::token token;
		std::vector<std::vector<mycelium::token>> context = {};


		parsed_token(mycelium::token token, const std::vector<mycelium::token>& ctx) : token(std::move(token)) {
			context.emplace_back(ctx);
		}

		explicit parsed_token(mycelium::token token) : token(std::move(token)) {}
	};



	class parser {

	public:

		enum state {
			idle,
			searching
		};


		mycelium::tokenizer tokenizer;
		state state;
		mycelium::token search_type;
		std::string searching_for;
		std::vector<mycelium::token_type> wading_search;
		std::vector<mycelium::token> temp = {};
		std::vector<mycelium::parsed_token> parsed_tokens = {};


		void parse();

		int parse_keyword_token(const parsed_token&, int index);

		explicit parser(mycelium::tokenizer tokenizer) : tokenizer(std::move(tokenizer)), state(idle), search_type({}) {}

	};
}