//
// Created by moss on 2/12/22.
//

#pragma once

#include <utility>

#include "base.h"

namespace mycelium {

	class tokenizer {
		private:
			std::vector<mycelium::token> tokens;
			std::vector<std::string> lines;

			const std::vector<std::string> math_token_strings = {"+", "-", "*", "/", "%", "="};
			const std::vector<std::string> logic_token_strings = {"==", "!=", "<", "<=", ">", ">=", "&&", "||"};
			const std::vector<std::string> unary_token_strings = {"++", "--", "!"};
			const std::vector<std::string> bitwise_token_strings = {"<<", ">>", "&", "|", "~"};
			const std::vector<std::string> whitespace_token_strings = {" ", "	"};
			const std::vector<std::string> grouping_token_strings = {"(", ")", "{", "}", "[", "]"};
			const std::vector<std::string> keyword_token_strings = {"fn", "op", "cn"};
			const std::vector<std::vector<std::string>> token_string_lists = {math_token_strings, logic_token_strings, unary_token_strings, bitwise_token_strings, whitespace_token_strings, grouping_token_strings, keyword_token_strings};
			const std::vector<std::string> token_strings = {"++", "--", "+", "-", "*", "/", "%", "=", "==", "!=", "<<", "<=", "<", ">>", ">=", ">", "&&", "||", "!", "&", "|", "~", " ", "	", "(", ")", "{", "}", "[", "]", "fn", "op", "cn"};


		public:


			explicit tokenizer(std::vector<std::string>  lines) : lines(std::move(lines)) {
				this->tokenize();
			}

			void tokenize();
	};
}