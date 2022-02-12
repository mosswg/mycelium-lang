//
// Created by moss on 2/12/22.
//

#pragma once

#include "base.h"

namespace mycelium {
	class tokenizer {
		private:
			std::vector<mycelium::token> tokens;

		public:

			explicit tokenizer(const std::vector<std::string>& lines) {
				this->tokenize(lines);
			}

			void tokenize(const std::vector<std::string>& lines) {
				// TODO: Implement
			}
	};
}