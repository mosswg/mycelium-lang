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

		public:

			static const std::vector<std::string> whitespace_token_strings;
			static const std::vector<std::string> grouping_token_strings;
			static const std::vector<std::string> keyword_token_strings;
			static const std::vector<std::string> oper_token_strings;
			static const std::vector<std::vector<std::string>> token_string_lists;
			static std::vector<std::string> token_strings;



		explicit tokenizer(std::vector<std::string>  lines) : lines(std::move(lines)) {
				if (token_strings.empty()) {
					for (const auto& token_string_list : token_string_lists) {
						for (const auto& token_string : token_string_list) {
							token_strings.push_back(token_string);
						}
					}
				}
				this->tokenize();
			}

			void tokenize();
	};
}