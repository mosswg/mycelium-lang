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

	namespace colored_text {
		//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
		static const char *RESET = "\033[0m";
		static const char *BLACK = "\033[30m";      /* Black */
		static const char *RED = "\033[31m";      /* Red */
		static const char *GREEN = "\033[32m";      /* Green */
		static const char *YELLOW = "\033[33m";      /* Yellow */
		static const char *BLUE = "\033[34m";      /* Blue */
		static const char *MAGENTA = "\033[35m";      /* Magenta */
		static const char *CYAN = "\033[36m";      /* Cyan */
		static const char *WHITE = "\033[37m";      /* White */
		static const char *BOLDBLACK = "\033[1m\033[30m";      /* Bold Black */
		static const char *BOLDRED = "\033[1m\033[31m";      /* Bold Red */
		static const char *BOLDGREEN = "\033[1m\033[32m";      /* Bold Green */
		static const char *BOLDYELLOW = "\033[1m\033[33m";      /* Bold Yellow */
		static const char *BOLDBLUE = "\033[1m\033[34m";      /* Bold Blue */
		static const char *BOLDMAGENTA = "\033[1m\033[35m";      /* Bold Magenta */
		static const char *BOLDCYAN = "\033[1m\033[36m";      /* Bold Cyan */
		static const char *BOLDWHITE = "\033[1m\033[37m";      /* Bold White */
	}







	static bool show_debug_lines = true;

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

		explicit token(const std::string& str) : string(str) {
			this->type = find_type(str);
		}

		static token_type find_type(const std::string &string);

        static std::string get_closing_grouping(const std::string& opening_grouping);

		bool operator==(const token& tk) const {
			return this->type == tk.type;
		}
	};

	class type {
	public:
		int code = -1;
		std::string name;
		int size = 8;


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
		oper_use,
		bad
	};

	class parsed_token {
	public:

		mycelium::token token;

		mycelium::parsed_token_type type;

		parsed_token() : token(invalid, ""), type(bad) {}

		explicit parsed_token(mycelium::token token, parsed_token_type type) : token(std::move(token)), type(type) {}

        virtual ~parsed_token() = default;

        virtual std::string to_string() = 0;

		virtual bool is_similar(std::shared_ptr<parsed_token> compare) = 0;
	};

	class operator_use : public parsed_token {
	public:
		explicit operator_use(mycelium::token token) : parsed_token(std::move(token), oper_use) {}

		std::string to_string() override {
			return "operator_use: " + token.string;
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return compare->token.string == this->token.string;
		}
	};

	class pattern_match {
	public:
		std::vector<token> pattern;

		pattern_match() : pattern({}) {}

		explicit pattern_match(std::vector<token> tokens) : pattern(std::move(tokens)) {}

		bool is_match(const std::vector<token>& test) {
			long test_size = test.size();
			if (test.back().type == newline) {
				test_size--;
			}

			if (test_size != pattern.size()) {
				std::cout << "not long enough :(" << std::endl;
				std::cout << test_size << '\n' << pattern.size() << std::endl;
				return false;
			}

			for (int i = 0; i < test_size; ++i) {
				if (test[i].type != pattern[i].type) {
					return false;
				}
				else {
					if (test[i].type == ttype && test[i].string != pattern[i].string) {
						return false;
					}
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

		int find_match(const std::vector<token>& tokens, int num_to_skip = 0) {
			for (int i = 0; i < tokens.size(); i++) {
				if (is_match_at_index(tokens, i)) {
					if (num_to_skip != 0) {
						num_to_skip--;
						continue;
					}
					return i;
				}
			}
			return -1;
		}
	};

	class scope;

	class variable : public parsed_token {
	public:
		mycelium::type type;
		long value = 0;
		scope* parent_scope;

		variable(mycelium::token name, mycelium::type type, scope* parent_scope) : parsed_token(std::move(name), var), type(std::move(type)), parent_scope(parent_scope) {}

		std::string to_string() override {
			std::string out = "variable ";
			out += type.name;
			out += ' ';
			out += token.string;
			out += ": ";
			out += std::to_string(value);
			return out;
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == var) {
				auto* compvar = (variable*)compare.get();

				return compvar->type.code == this->type.code;
			}
			return false;
		}
	};

	class scope {
		std::vector<variable> variables = {};
		bool has_parent = true;
		scope* parent;

	public:
		explicit scope(scope* parent) : parent(parent) {}

		~scope() {
			variables.clear();
		}

		void add_variable(const mycelium::variable* var) {
			this->variables.push_back(*var);
		}

		int get_variable_index(const std::string& name) {
			for (int i = 0; i < variables.size(); i++) {
				if (variables[i].token.string == name) {
					return i;
				}
			}
			return -1;
		}
	};

	class function_base : public parsed_token {
	public:

		mycelium::token name;
		std::vector<mycelium::token> ret;

		std::vector<std::shared_ptr<mycelium::parsed_token>> body;

		mycelium::scope scope;

		function_base(mycelium::token token, mycelium::token name, std::vector<mycelium::token> ret, parsed_token_type type, mycelium::scope* parent_scope) : parsed_token(std::move(token), type),
																										name(std::move(name)), ret(std::move(ret)), scope(parent_scope) {}
    };

	class function : public function_base {
	public:
		std::vector<mycelium::token> args;

		function(mycelium::token token, mycelium::token name, std::vector<mycelium::token> ret, std::vector<mycelium::token> args, mycelium::scope* parent_scope) :
																				function_base(std::move(token), std::move(name),
																							  std::move(ret), func, parent_scope), args(std::move(args)) {}


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (function*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args == this->args);
			}
			return false;
		}

		std::string to_string() override {
			std::string out = "function ";
			out += name.string;
			out += '(';
			for (int i = 0; i < args.size(); i++) {
				out += args[i].string;
				if (i < args.size()-1) {
					out += ", ";
				}
			}
			out += ')';
			out += ": ";
			for (int i = 0; i < ret.size(); i++) {
				out += ret[i].string;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}
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


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (operatr*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->context == this->context);
			}
			return false;
		}

		std::string to_string() override {
			std::string out = "function ";
			out += name.string;
			out += '<';
			for (int i = 0; i < context.size(); i++) {
				out += context[i].string;
				if (i < context.size()-1) {
					out += ", ";
				}
			}
			out += '>';
			out += ": ";
			for (int i = 0; i < ret.size(); i++) {
				out += ret[i].string;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}

		static std::string encode_operator (const std::string& oper);

		static std::string generate_name_from_context(std::vector<mycelium::token>& context);
	};

	class conditional : public function_base {
	public:
		std::vector<mycelium::token> args;

		conditional(mycelium::token token, mycelium::token name, std::vector<mycelium::token> args, mycelium::scope* parent_scope) : function_base(std::move(token), std::move(name),
																																				   {{}}, parsed_token_type::cond, parent_scope),
																																				   args(std::move(args)) {}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (conditional*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args == this->args);
			}
			return false;
		}

		std::string to_string() override {
			std::string out = "function ";
			out += name.string;
			out += '(';
			for (int i = 0; i < args.size(); i++) {
				out += args[i].string;
				if (i < args.size()-1) {
					out += ", ";
				}
			}
			out += ')';
			out += ": ";
			for (int i = 0; i < ret.size(); i++) {
				out += ret[i].string;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}
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