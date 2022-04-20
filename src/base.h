//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <memory>

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
		static std::vector<std::string> oper_strings;
		static std::vector<std::string> line_end;
		static std::vector<std::string> seperator_strings;
		static std::vector<std::vector<std::string>> string_lists;
		static std::vector<std::string> strings;
		static std::vector<std::string> type_names;
		static std::string line_comment;
		static std::string open_block_comment;
		static std::string close_block_comment;
		static std::string function_keyword;
		static std::string operator_keyword;
		static std::string conditional_keyword;
		static std::string class_keyword;



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
		static std::vector<type> types;
	};


	enum parsed_token_type {
		func,
		oper,
		cond,
		var,
		bad
	};

	class parsed_token {
	public:

		mycelium::token token;

		mycelium::parsed_token_type type;

		parsed_token() : token(invalid, ""), type(bad) {}

		explicit parsed_token(mycelium::token token, parsed_token_type type) : token(std::move(token)), type(type) {}
	};

	class pattern_match {
	public:
		std::vector<parsed_token> pattern;

		pattern_match() : pattern({}) {}

		explicit pattern_match(std::vector<parsed_token> tokens) : pattern(std::move(tokens)) {}

		bool is_match(const std::vector<parsed_token>& test) {
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

		bool is_match_at_index(const std::vector<parsed_token>& test, int index) {

			for (int i = 0; i < pattern.size(); ++i) {
				if (test[i + index].type != pattern[i].type) {
					return false;
				}
			}
			return true;
		}

		int find_match(const std::vector<parsed_token>& tokens) {
			for (int i = 0; i < tokens.size(); i++) {
				if (is_match_at_index(tokens, i)) {
					return i;
				}
			}
			return -1;
		}

		static pattern_match generate_pattern_from_tokens(std::vector<mycelium::token>);
	};

	class variable;

	class scope {
		std::vector<variable> var = {};
		bool has_parent = true;
		scope* parent;

	public:
		explicit scope(scope* parent) : parent(parent) {}
	};

	class variable : public parsed_token {
	public:
		mycelium::type type;
		long value = 0;
		scope* parent_scope;

		variable(mycelium::token name, mycelium::type type, scope* parent_scope) : parsed_token(std::move(name), var), type(std::move(type)), parent_scope(parent_scope) {}
	};

	class function_base : public parsed_token {
	public:

		mycelium::token name;
		std::vector<mycelium::token> ret;

		std::vector<mycelium::token> body;

		mycelium::scope scope;

		function_base(mycelium::token token, mycelium::token name, std::vector<mycelium::token> ret, parsed_token_type type, mycelium::scope* parent_scope) : parsed_token(std::move(token), type),
																										name(std::move(name)), body({}), ret(std::move(ret)), scope(parent_scope) {}
	};

	class function : public function_base {
	public:
		std::vector<mycelium::token> args;

		function(mycelium::token token, mycelium::token name, std::vector<mycelium::token> ret, std::vector<mycelium::token> args, mycelium::scope* parent_scope) :
																				function_base(std::move(token), std::move(name),
																							  std::move(ret), func, parent_scope), args(std::move(args)) {}
	};

	class operatr : public function_base {
	public:
		pattern_match pattern;

		std::vector<mycelium::token> context;

		operatr(mycelium::token token, std::vector<mycelium::token> context, std::string name, std::vector<mycelium::token> ret, mycelium::scope* parent_scope) : function_base(std::move(token),
																																				 mycelium::token(word,std::move(name)),
																																				 std::move(ret), parsed_token_type::oper,
																																				 parent_scope),
																																				 context(std::move(context)) {}

		static std::string encode_operator (const std::string& oper);

		static std::string generate_name_from_context(std::vector<mycelium::token>& context);
	};

	class conditional : public function_base {
	public:
		std::vector<mycelium::token> args;

		conditional(mycelium::token token, mycelium::token name, std::vector<mycelium::token> args, mycelium::scope* parent_scope) : function_base(std::move(token), std::move(name),
																																				   {{}}, parsed_token_type::cond, parent_scope),
																																				   args(std::move(args)) {}
	};

//
//	class variable : public parsed_token {
//	public:
//		static std::vector<mycelium::variable> variables;
//
//		mycelium::token name;
//		mycelium::type type;
//		int value = 0;
//	};

	// from: https://stackoverflow.com/questions/5100718/integer-to-hex-string-in-c
	template <typename I> std::string int_to_hex_string(I w, size_t hex_len = sizeof(I)<<1) {
		static const char* digits = "0123456789ABCDEF";
		std::string rc(hex_len,'0');
		for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
			rc[i] = digits[(w>>j) & 0x0f];
		return rc;
	}






	std::vector<std::string> string_split(const std::string &str, const std::vector<std::string> &spl);

	template <typename T>
	std::ostream& operator<<(std::ostream&, const std::vector<T>&);
}