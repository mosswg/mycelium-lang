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

			explicit tokenizer(std::vector<std::string> lines) : lines(std::move(lines)) {}

			void tokenize();
	};
}