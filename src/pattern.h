#pragma once

#include "parsed_token.h"
#include "token.h"
#include "type.h"
#include "base.h"
#include <string>
#include <memory>
#include <vector>

namespace mycelium {
	enum pattern_token_type {
		pt_oper = 0,
		pt_expr,
		pt_metatype,
		pt_metalist,

	};

	class pattern_token {
	public:
		pattern_token_type type;
		uint32_t token_length;

		explicit pattern_token(pattern_token_type ptt) : type(ptt) {}

		/**
			Returns the number of matched tokens starting at the start of the vector or zero if no match.
			Assumes that other is not empty.

			@ret number of matched tokens
			*/
		virtual uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other);

		virtual std::string to_string() const;
	};

	namespace pattern_tokens {
		class oper : public pattern_token {
		public:
			std::string value;
			oper(std::string str) : pattern_token(std::move(pt_oper)), value(str) {}

			uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) override {
				if (other[0]->type != pt_oper) {
					return 0;
				}
				auto other_as_oper = (oper*)other[0].get();
				if (other_as_oper->value == value) {
					return 1;
				}
				return 0;
			}

			std::string to_string() const override {
				return value;
			}
		};


		class expr : public pattern_token {
		public:
			std::shared_ptr<expression> value;
			expr(std::shared_ptr<expression> expr) : pattern_token(pt_oper), value(expr) {}

			uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) override {
				if (other[0]->type != pt_expr) {
					return 0;
				}
				auto other_as_expr = (expr*)other[0].get();
				if (other_as_expr->value->get_type().code == value->get_type().code) {
					return 1;
				}
				return 0;
			}

			std::string to_string() const override {
				return value->to_string();
			}
		};

		class list : public pattern_token {
			public:
			std::vector<std::shared_ptr<pattern_token>> value;
			bool allow_zero_length = false;
			list(const std::vector<std::shared_ptr<pattern_token>>& list) : pattern_token(pt_oper), value(list) {}

			std::string to_string() const override {
				std::string out = "{";
				for (auto li : value) {
					out += li->to_string();
					if (&li != &value.back()) {
						out += ", ";
					}
				}
				out += "}...";
				return out;
			}

			uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) override {
				if (other.size() < value.size()) {
					return 0;
				}

				if (other[0]->type != value[0]->type) {
					return 0;
				}

				uint32_t num_consumed = 0;
				while (other[num_consumed]->type == value[num_consumed % value.size()]->type) {

					num_consumed++;
				}
				return 0;
			}
		};
	}

	class pattern_match {
	public:

		std::vector<pattern_token> pattern;

		int operator_offset = 0;

		pattern_match() : pattern() {}

		explicit pattern_match(std::vector<pattern_token> tokens) : pattern(std::move(tokens)) {}

		static pattern_match create_from_tokens(const std::vector<token>& tks, int start_index, int end_index) {
			pattern_match out;
			token current_type;
			bool found_op = false;
			for (int i = start_index; i < end_index; i++) {
				token tk = tks[i];
				if (tk.type == ttype) {
					if (current_type.type != token_type::invalid) {
						std::cout << tk.string << " & " << tks[i - 1].string << "\n";
						throw_error("Two type names in a row in operator definition");
					}
					current_type = tk;
				}
				else if (tk.type == word) {
					if (current_type.type != token_type::invalid) {
						out.pattern.emplace_back(std::make_shared<variable>(tk, type::types[type::validate_type(current_type)]));

						current_type.type = token_type::invalid;
						if (!found_op) {
							out.operator_offset++;
						}
					}
					else {
						out.pattern.emplace_back(tk.string);
						found_op = true;
					}
				}
				else {
					out.pattern.emplace_back(tk.string);
					found_op = true;
				}
			}
			return out;
		}

		static pattern_match create_from_tokens(const std::vector<token>& tks) {
			return create_from_tokens(tks, 0, (int)tks.size());
		}

		std::string to_string() const {
			std::string out;

			for (const auto& pt : pattern) {
				out += pt.to_string();
				if (&pt != &pattern.back()) {
					out += ' ';
				}
			}

			return out;
		}

		std::vector<std::shared_ptr<expression>> get_expressions() {
			std::vector<std::shared_ptr<expression>> out;
			for (auto& pt : pattern) {
				if (pt.type == pt_expr) {
					out.push_back(pt.expr);
				}
			}
			return out;
		}

		bool is_match(const pattern_match& test_pattern) {
			long test_size = (long)test_pattern.pattern.size();

			if (test_size != pattern.size()) {
				return false;
			}

			for (int i = 0; i < test_size; ++i) {
				if (test_pattern.pattern[i].type != pattern[i].type) {
					return false;
				}
				else {
					if (pattern[i].type == pt_expr) {
						if (test_pattern.pattern[i].expr->get_type().code != pattern[i].expr->get_type().code) {
							return false;
						}
					}
					else {
						if (test_pattern.pattern[i].oper != pattern[i].oper) {
							return false;
						}
					}
				}
			}
			return true;
		}

		bool is_match(const std::vector<token>& test) {
			pattern_match test_pattern = create_from_tokens(test);
			return is_match(test_pattern);
		}
	};
}
