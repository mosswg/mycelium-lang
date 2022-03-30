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
			explicit tokenizer(std::vector<std::string> lines) : lines(std::move(lines)) {}

			void tokenize();
	};
}