//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>

namespace mycelium {
	void throw_error(const std::string& error, int code);


	void initialize_static_values();

	enum token_type {
		op,
		whitespace,
		grouping,
		keyword,
		ttype,
		newline,
		separator,
		word,
		num,
		invalid
	};

	template<typename T>
	bool vector_contains(const std::vector<T> &vec, const T &con);

	class token {
	public:
		token_type type;
		std::string string;

		static std::vector<std::string> whitespace_strings;
		static std::vector<std::string> grouping_strings;
		static std::vector<std::string> keyword_strings;
		static std::vector<std::string> type_strings;
		static std::vector<std::string> oper_strings;
		static std::vector<std::string> line_end;
		static std::vector<std::string> seperator_strings;
		static std::vector<std::vector<std::string>> string_lists;
		static std::vector<std::string> strings;
		static std::vector<std::string> type_names;


	public:
		token(token_type type, std::string str) : type(type), string(std::move(str)) {}

		token() : type(invalid) {}

		explicit token(std::string str) : string(std::move(str)) {
			this->type = find_type(str);
		}

		static token_type find_type(const std::string &string);
	};

	class type {
	public:
		const int code;
		const std::string name;
		const int size;


		static std::vector<std::string> strings;

		type(int code, std::string name, int size) : code(code), name(std::move(name)), size(size) {}

		static const type pointer;
		static const type integer;
		static const type character;
		static const type string;
		static const type list;
		static const type tuple;
		static const type oper;
		static const type array;
		static const type cstring;
		static const type token;
		static const type none;
	};


	class list {
	private:
		void *storage;

	public:
		list(int length) {

		}
	};

	class pattern_match {
		std::vector<token> pattern;

		bool is_match(const std::vector<token>& test) {
			if (test.size() != pattern.size()) {
				return false;
			}

			for (int i = 0; i < test.size(); ++i) {
				if (test[i].type != pattern[i].type) {
					return false;
				}
			}
			return true;
		}

		bool is_match_at_index(const std::vector<token>& test, int index) {

			for (int i = 0; i < pattern.size(); ++i) {
				if (test[i + index].type != pattern[i].type) {
					return false;
				}
			}
			return true;
		}

		int find_match(const std::vector<token>& tokens) {
			for (int i = 0; i < tokens.size(); i++) {
				if (is_match_at_index(tokens, i)) {
					return i;
				}
			}
			return -1;
		}
	};

	class parsed_token {
	public:

		mycelium::token token;


		explicit parsed_token(mycelium::token token) : token(std::move(token)) {}
	};

	class function_base : public parsed_token {
	public:

		mycelium::token name;
		std::vector<mycelium::token> ret;

		std::vector<mycelium::token> body;

		function_base(mycelium::token token, mycelium::token name, std::vector<mycelium::token> ret, std::vector<mycelium::token> body) : parsed_token(std::move(token)), name(std::move(name)), body(std::move(body)), ret(std::move(ret)) {}
	};

	class function : public function_base {
	public:
		std::vector<mycelium::token> args;

		function(mycelium::token token, mycelium::token name, std::vector<mycelium::token> ret, std::vector<mycelium::token> args, std::vector<mycelium::token> body) : function_base(std::move(token), std::move(name), std::move(ret), std::move(body)), args(std::move(args)) {}
	};

	class oper : public function_base {
	public:
		std::vector<mycelium::token> context;

		oper(mycelium::token token, std::vector<mycelium::token> context, std::string name, std::vector<mycelium::token> ret, std::vector<mycelium::token> body) : function_base(std::move(token), mycelium::token(word, std::move(name)), std::move(ret), std::move(body)), context(std::move(context)) {}
	};

	class cond : public function_base {
	public:
		std::vector<mycelium::token> args;

		cond(mycelium::token token, mycelium::token name, std::vector<mycelium::token> args, std::vector<mycelium::token> body) : function_base(std::move(token), std::move(name), {{}}, std::move(body)), args(std::move(args)) {}
	};


	class variable : public parsed_token {
	public:
		static std::vector<mycelium::variable> variables;

		mycelium::token name;
		mycelium::type type;
		int value = 0;
	};





	std::vector<std::string> string_split(const std::string &str, const std::vector<std::string> &spl);

	template <typename T>
	std::ostream& operator<<(std::ostream&, const std::vector<T>&);
}