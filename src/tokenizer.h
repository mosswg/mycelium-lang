//
// Created by moss on 2/12/22.
//
#pragma once

#include <utility>
#include "base.h"

namespace mycelium {

	class tokenizer {
	public:

		std::vector<mycelium::token> tokens;

		std::vector<std::string> lines;

		int current_token_index = 0;

		explicit tokenizer(std::vector<std::string> lines) : lines(std::move(lines)) {}

		void tokenize();

		token get_next_token();

		token get_next_token_without_increment() const;

		token get_next_non_whitespace_token();

		std::vector<mycelium::token> get_next_non_whitespace_tokens_until(const std::string& until);

		std::vector<mycelium::token> get_tokens_inside_grouping();

		int get_ending_grouping_token_index();

		int num_tokens_until_newline();

		void skip_tokens_inside_grouping();

		bool has_next_token() const;

		std::vector<token> tokens_until_newline();

		void skip_to_newline();
	};
}
