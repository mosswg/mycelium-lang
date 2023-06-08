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

		pattern_token(pattern_token_type ptt) : type(ptt), token_length(0) {}

		pattern_token(const pattern_token& pt) : type(pt.type), token_length(pt.token_length) {}

		/**
			Returns the number of matched tokens starting at the start of the vector or zero if no match.
			Assumes that other is not empty.

			@ret number of matched tokens
			*/
		virtual uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) const {
			return 0;
		}

		virtual std::string to_string() const {
			return "";
		}
	};

	namespace pattern_tokens {
		class oper : public pattern_token {
		public:
			std::string value;
			explicit oper(std::string str) : pattern_token(std::move(pt_oper)), value(str) {}
			explicit oper(const token& tk) : pattern_token(std::move(pt_oper)), value(tk.string) {}

			uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) const override {
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


		class variable : public pattern_token {
		public:
			std::shared_ptr<expression> value;
			explicit variable(std::shared_ptr<expression> expr) : pattern_token(pt_expr), value(expr) {}

			uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) const override {
				if (other.front()->type != pt_expr) {
					return 0;
				}
				auto other_as_expr = (variable*)other.front().get();
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

			list() : pattern_token(pt_oper), value({}) {}

			list(const std::vector<std::shared_ptr<pattern_token>>& list) : pattern_token(pt_oper), value(list) {}


			static list create_from_tokens(const std::vector<token>& tokens) {
				list out;
				for (const auto& token : tokens) {
					if (token.type == ttype) {
						out.value.push_back(std::make_shared<pattern_tokens::variable>(std::make_shared<mycelium::variable>(token, type::types[type::validate_type(token)])));
					}
					else if (token.type == word) {
						out.value.push_back(std::make_shared<pattern_tokens::oper>(token));
					}
					else {
						throw_error("pattern list create_from_token doesn't know what to do with \"" + token.string + "\", type: " + std::to_string((int)token.type));
					}
				}
				return out;
			}

			uint32_t num_matched(const std::vector<std::shared_ptr<pattern_token>>& other) const override {
				if (other.size() < value.size()) {
					return 0;
				}

				if (other[0]->type != value[0]->type) {
					return 0;
				}

				uint32_t num_consumed = 0;
				std::vector<std::shared_ptr<pattern_token>> other_subvec = other;
				std::vector<std::shared_ptr<pattern_token>> value_subvec = value;
				/// TODO: This includes any matching subsets of the list. E.G. {<int,>...ints} would match all of "1, 2, 3" but also {<+int>...ints} would match "1 + 2 +". Decide if thats good or not.
				while ((num_consumed < other.size()) && (other_subvec.front()->type == value[num_consumed % value.size()]->type)) {
					uint32_t matched = value[num_consumed % value.size()]->num_matched(other_subvec);
					if (matched == 0) {
						break;
					}
					// take the subvector of other_subvect from 'matched' to end
					other_subvec = {other_subvec.begin() + matched, other_subvec.end()};
					num_consumed += matched;
				}
				return num_consumed;
			}

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

		};
	}


	std::shared_ptr<pattern_token> make_pattern_token(const std::vector<token>&);


	/// FIXME: This should be shared_ptr but many thing expect it to not be.

	inline std::shared_ptr<pattern_token> make_pattern_token(const std::string& str) {
		return std::make_shared<pattern_tokens::oper>(str);
	}

	inline std::shared_ptr<pattern_token> make_pattern_token(const std::shared_ptr<expression>& expr) {
		return std::make_shared<pattern_tokens::variable>(expr);
	}

	class pattern_match {
	public:

		std::vector<std::shared_ptr<pattern_token>> pattern;

		int operator_offset = 0;

		pattern_match() : pattern() {}

		explicit pattern_match(std::vector<std::shared_ptr<pattern_token>> tokens) : pattern(std::move(tokens)) {}

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
						out.pattern.push_back(make_pattern_token(std::make_shared<variable>(tk, type::types[type::validate_type(current_type)])));

						current_type.type = token_type::invalid;
						if (!found_op) {
							out.operator_offset++;
						}
					}
					else {
						out.pattern.push_back(make_pattern_token(tk.string));
						found_op = true;
					}
				}
				else {
					out.pattern.push_back(make_pattern_token(tk.string));
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
				out += pt->to_string();
				if (&pt != &pattern.back()) {
					out += ' ';
				}
			}

			return out;
		}

		std::vector<std::shared_ptr<expression>> get_expressions() {
			/*
			std::vector<std::shared_ptr<expression>> out;
			for (auto& pt : pattern) {
				if (pt.type == pt_expr) {
					out.push_back(pt.expr);
				}
			}
			return out;
			*/
			return {};
		}

		bool is_match(const pattern_match& test_pattern) {
			auto test_size = test_pattern.pattern.size();

			if (test_size != pattern.size()) {
				return false;
			}

			std::vector<std::shared_ptr<pattern_token>> test_subvec = test_pattern.pattern;
			for (size_t i = 0; i < test_size; ++i) {
				if (test_pattern.pattern[i]->type != test_subvec.front()->type) {
					return false;
				}
				else {
					int num_matching = pattern[i]->num_matched(test_subvec);

					if (num_matching == 0) {
						return false;
					}

					test_subvec = {test_subvec.begin() + num_matching, test_subvec.end()};
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
