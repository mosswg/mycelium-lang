//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <fstream>
#include <functional>

namespace mycelium {

	namespace colored_text {
		//the following are LINUX, and MacOS ONLY terminal color codes.
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

	void throw_error(const std::string& error);

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
		string_literal,
		invalid
	};

	template<typename T>
	bool vector_contains(const std::vector<T> &vec, const T &con);

	class token {
	public:
		token_type type;
		std::string string;
		int line;

		static std::vector<std::string> whitespace_strings;
		static std::vector<std::string> grouping_strings;
		static std::vector<std::string> keyword_strings;
		static std::vector<std::string> oper_strings;
		static std::vector<std::string> line_end;
		static std::vector<std::string> separator_strings;
		static std::vector<std::string> comment_strings;
		static std::vector<std::vector<std::string>> string_lists;
		static std::vector<std::string> strings;
		static std::vector<std::string> type_names;
		static std::string line_comment;
		static std::string open_block_comment;
		static std::string close_block_comment;
		static std::string function_keyword;
		static std::string operator_keyword;
		static std::string conditional_keyword;
		static std::string override_keyword;
		static std::string class_keyword;
		static std::string this_keyword;
		static std::string return_keyword;


	public:
		token(token_type type, std::string str, int line) : type(type), string(std::move(str)), line(line) {}

		token(token_type type, std::string str) : type(type), string(std::move(str)), line(-1) {}

		token() : type(invalid), line(-1) {}

		token(const std::string& str, int line) : string(str), line(line) {
			this->type = find_type(str);
		}

		explicit token(const std::string& str) : string(str), line(-1) {
			this->type = find_type(str);
		}

		static token_type find_type(const std::string &string);

		static std::string get_closing_grouping(const std::string& opening_grouping);

		bool operator==(const token& tk) const {
			return this->type == tk.type && this->string == tk.string;
		}
	};

	class function_base;

	class type {
	public:
		int code;
		std::string name;
		int size;
		static std::vector<std::vector<std::shared_ptr<function_base>>> member_functions;

		static std::vector<std::string> strings;

		type(const int code, std::string name, const int size) : code(code), name(std::move(name)), size(size) {}

		type(const int code, std::string name, const int size, const std::vector<std::shared_ptr<function_base>> functions) : code(code), name(std::move(name)), size(size) {
			member_functions[code] = functions;
		}

		static const type reference;
		static const type integer;
		static const type character;
		static const type string;
		static const type list;
		static const type tuple;
		static const type boolean;
		static const type oper;
		static const type func;
		static const type array;
		static const type cstring;
		static const type token;
		static const type file;
		static const type none;
		static std::vector<type> types;

		static int validate_type(const mycelium::token& type) {
			for (int i = 0; i < type::strings.size(); i++) {
				if (type::strings[i] == type.string) {
					if (show_debug_lines) {
						//    std::cout << "type \"" << type.string << "\" validated" << std::endl;
					}
					return i;
				}
			}
			mycelium::throw_error("unknown type \"" + type.string + "\"");
			return -1;
		}

		std::string to_string() const {
			return strings[this->code];
		}


		std::vector<std::shared_ptr<function_base>> get_member_functions() const {
			return member_functions[this->code];
		}

		static std::vector<std::shared_ptr<function_base>> get_member_functions(const mycelium::type& type) {
			return member_functions[type.code];
		}

		void add_member_function(std::shared_ptr<function_base> fn) const {
			member_functions[this->code].push_back(fn);
		}


		bool operator== (const type& b) const {
			return this->code == b.code;
		}

		static std::vector<type> convert_tokens_to_types(const std::vector<mycelium::token>& tks) {
			/// HACK: This is a very stupid conversion in the sense that it doesn't care about anything other than types.
			/// Might want to change this later.
			std::vector<type> out;
			for (const auto& tk : tks) {
				if (tk.type == ttype) {
					int type_code = validate_type(tk);
					out.push_back(types[type_code]);
				}
			}
			return out;
		}
	};


	enum parsed_token_type {
		func,
		oper,
		cond,
		var,
		expr,
		oper_use,
		func_call,
		function_return,
		bad
	};

	class parsed_token {
	public:

		mycelium::token token;

		mycelium::parsed_token_type type;

		parsed_token() : token(), type(bad) {}

		explicit parsed_token(mycelium::token token, parsed_token_type type) : token(std::move(token)), type(type) {}

		virtual ~parsed_token() = default;

		virtual std::string to_string() const = 0;

		virtual void execute() = 0;

		virtual bool is_similar(std::shared_ptr<parsed_token> compare) = 0;
	};

	class variable;

	class expression : public parsed_token {
	public:
		explicit expression(mycelium::token name) : parsed_token(std::move(name), expr) {}

		explicit expression(mycelium::token name, mycelium::parsed_token_type type) : parsed_token(std::move(name), type) {}

		virtual std::shared_ptr<variable> get_value() = 0;

		virtual mycelium::type get_type() = 0;

		std::string to_string() const override {
			return "expr";
		}
	};

	class function;

	std::string function_to_string(const std::shared_ptr<function>&);

	class variable : public expression, public std::enable_shared_from_this<variable> {
	public:
		mycelium::type type;
		long value;
		std::shared_ptr<std::string> str;
		std::shared_ptr<mycelium::function> fn_ptr;
		std::shared_ptr<std::fstream> file;

		variable(mycelium::token name, const mycelium::type& type) : expression(std::move(name)), type(type) {
			if (type == type::string) {
				this->str = std::make_shared<std::string>();
			}
			if (type == type::func) {
				// Do nothing
			}
			if (type == type::file) {
				this->str = std::make_shared<std::string>();
				this->file = std::make_shared<std::fstream>();
			}
			else if (type == type::list) {
				throw_error("Unsupported Creation of list");
			}
			else if (type == type::tuple) {
				throw_error("Unsupported Creation of tuple");
			}
			else {
				this->value = 0;
			}
		}

		variable(mycelium::token name, long value) : expression(std::move(name)), type(type::integer), value(value) {}

		variable(mycelium::token name, std::string str) : expression(std::move(name)), type(type::string), str(std::make_shared<std::string>(str)) {}

		variable(mycelium::token name, std::shared_ptr<mycelium::function> fn_ptr) : expression(std::move(name)), type(type::func), fn_ptr(std::move(fn_ptr)) {}

		variable(mycelium::token name, std::shared_ptr<std::fstream> file_ptr) : expression(std::move(name)), type(type::file), file(std::move(file_ptr)) {
			this->str = std::make_shared<std::string>("");
		}

		~variable() override {
			this->destroy();
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, long value) {
			std::shared_ptr<variable> out = std::make_shared<variable>(name, value);
			return out;
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::string& str) {
			std::shared_ptr<variable> out = std::make_shared<variable>(name, str);
			return out;
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::shared_ptr<function>& fn_ptr) {
			std::shared_ptr<variable> out = std::make_shared<variable>(name, fn_ptr);
			return out;
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::shared_ptr<std::fstream>& file_ptr) {
			return std::make_shared<variable>(name, file_ptr);
		}

		static std::vector<std::shared_ptr<mycelium::variable>> convert_tokens_to_variables(const std::vector<mycelium::token>& tks) {
			/// HACK: This is a very stupid conversion in the sense that it doesn't care about anything other than types.
			/// Might want to change this later.
			std::vector<std::shared_ptr<variable>> out;
			for (int i = 0; i < tks.size(); i++) {
				if (tks[i].type == ttype) {
					int type_code = type::validate_type(tks[i]);
					const mycelium::token& name = tks[i + 1];
					out.push_back(std::make_shared<variable>(name, type::types[type_code]));
					i++;
				}
			}
			return out;
		}

		static std::vector<std::shared_ptr<mycelium::variable>> convert_types_to_variables(const std::vector<mycelium::type>& types) {
			/// HACK: This is a very stupid conversion in the sense that it doesn't care about anything other than types.
			/// Might want to change this later.
			std::vector<std::shared_ptr<variable>> out;
			for (auto& type : types) {
				out.push_back(std::make_shared<variable>(mycelium::token(word, type.name + std::to_string(out.size())), type));
			}
			return out;
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == var) {
				auto* compvar = (variable*)compare.get();

				return compvar->type.code == this->type.code;
			}
			return false;
		}

		void destroy() {
			if (type.code == type::string.code) {
				str->clear();
			}
			else if (type.code == type::func.code) {
				// explicitly do nothing
			}
			else if (type.code == type::list.code) {
				throw_error("Unsupported Destroy of list");
			}
			else if (type.code == type::tuple.code) {
				throw_error("Unsupported Destroy of tuple");
			}

			this->type = type::none;
			this->value = 0;
		}

		void execute() override {}

		virtual void set_value(const mycelium::variable& other) {
			if (other.type == type::string) {
				if (this->type == type::string) {
					*this->str = *other.str;
				}
				else if (this->type == type::integer) {
					throw_error("Cannot convert string to int");
				}
				else if (this->type == type::file) {
					*this->str = *other.str;
					this->file = std::make_shared<std::fstream>(*other.str, std::fstream::in | std::fstream::out);
				}
			}
			else if (other.type == type::func) {
				throw_error("Cannot mutate function");
			}
			else if (other.type == type::integer) {
				if (this->type == type::string) {
					*this->str = std::to_string(other.value);
				}
				else if (this->type == type::integer) {
					this->value = other.value;
				}
			}
		}

		virtual void set_value(const std::shared_ptr<mycelium::variable>& other) {
			if (other->type == type::string) {
				if (this->type == type::string) {
					*this->str = *other->str;
				}
				else if (this->type == type::integer) {
					throw_error("Cannot convert string to int");
				}
				else if (this->type == type::file) {
					*this->str = *other->str;
					this->file = std::make_shared<std::fstream>(*other->str, std::fstream::in | std::fstream::out);
				}
			}
			else if (other->type == type::integer) {
				if (this->type == type::string) {
					*this->str = std::to_string(other->value);
				}
				else if (this->type == type::integer) {
					this->value = other->value;
				}
			}
		}

		mycelium::type get_type() override {
			return this->type;
		}

		std::shared_ptr<variable> get_value() override {
			return shared_from_this();
		}

		virtual std::string get_as_string() const {
			if (type == type::string) {
				return *this->str;
			}
			else if (type.code == type::list.code) {
				throw_error("Unsupported Get As String of list");
			}
			else if (type.code == type::tuple.code) {
				throw_error("Unsupported Get As String of tuple");
			}
			else if (type == type::func) {
				return function_to_string(this->fn_ptr);
			}
			else if (type == type::file) {
				if (this->str.get()) {
					return "file(" + *this->str + ")";
				}
				return "file";
			}
			else if (type == type::boolean) {
				return this->value ? "true" : "false";
			}
			else {
				return std::to_string(this->value);
			}
			return "ERROR IN STRING CONVERSION";
		}


		std::string to_string() const override {
			std::string out = "variable ";
			out += type.name;
			out += ' ';
			out += token.string;
			out += ": ";
			out += get_as_string();
			return out;
		}
	};

	class constant : public variable {
	public:
		explicit constant(long value) : variable({}, value) {}

		explicit constant(std::string value) : variable({}, std::move(value)) {}


		void set_value(const mycelium::variable&) override {
			throw_error("Cannot Set Value of Constant");
		}


		void set_value(const std::shared_ptr<mycelium::variable>&) override {
			throw_error("Cannot Set Value of Constant");
		}

		std::shared_ptr<variable> get_value() override {
			return shared_from_this();
		}

		static std::shared_ptr<constant> make_constant(long value) {
			return std::make_shared<constant>(value);
		}

		static std::shared_ptr<constant> make_constant(const std::string& value) {
			return std::make_shared<constant>(value);
		}


		static std::shared_ptr<constant> make_bool_constant(bool value) {
			auto out = std::make_shared<constant>(value);
			out->type = type::boolean;
			return out;
		}

		std::string to_string() const override {
			std::string out = "constant ";
			out += type.name;
			out += ' ';
			out += token.string;
			out += ": ";
			out += get_as_string();
			return out;
		}
	};

	class scope {
	public:
		std::shared_ptr<scope> parent_scope;
		std::vector<std::shared_ptr<variable>> variables = {};

		explicit scope(std::shared_ptr<scope> parent_scope) : parent_scope(std::move(parent_scope)) {}


		std::shared_ptr<variable> make_variable(std::shared_ptr<variable> out) {
			if (get_variable_in_current_scope_only(out->token.string)) {
				throw_error("Cannot create variable with name \"" + out->token.string + "\" because a variable with that name already exists");
			}
			this->variables.push_back(out);
			return out;
		}
		std::shared_ptr<variable> make_variable(const mycelium::token& name, const mycelium::type& type) {
			return make_variable(std::make_shared<variable>(name, type));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, long value) {
			return make_variable(std::make_shared<variable>(name, value));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, const std::string& str) {
			return make_variable(std::make_shared<variable>(name, str));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, const std::shared_ptr<mycelium::function>& fn_ptr) {
			return make_variable(std::make_shared<variable>(name, fn_ptr));
		}

		std::shared_ptr<variable> get_variable(const std::string& name) {
			for (auto& var : variables) {
				if (var->token.string == name) {
					return var;
				}
			}
			if (parent_scope) {
				return parent_scope->get_variable(name);
			}
			return {};
		}

		std::shared_ptr<variable> get_variable_in_current_scope_only(const std::string& name) {
			for (auto& var : variables) {
				if (var->token.string == name) {
					return var;
				}
			}
			return {};
		}
	};

	class pattern_token {
	public:
		std::string oper;
		std::shared_ptr<expression> expr;
		bool is_expression = false;

		explicit pattern_token(std::shared_ptr<expression> expr) : expr(std::move(expr)), is_expression(true) {}

		explicit pattern_token(std::string oper) : oper(std::move(oper)), is_expression(false) {}

		std::string to_string() const {
			if (is_expression) {
				return expr->to_string();
			}
			return oper;
		}
	};

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
			/// TODO: Handle two types in a row
			for (int i = start_index; i < end_index; i++) {
				token tk = tks[i];
				if (tk.type == ttype) {
					if (current_type.type != token_type::invalid) {
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
				if (pt.is_expression) {
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
				if (test_pattern.pattern[i].is_expression != pattern[i].is_expression) {
					return false;
				}
				else {
					if (pattern[i].is_expression) {
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

		//        bool is_match_at_index(const std::vector<token>& test, int index) {
		//            pattern_match test_pattern = create_pattern_for_match(test, index - this->operator_offset, this->pattern.size());
		//            return is_match(test_pattern);
		//        }

		//        int find_match(const std::vector<std::shared_ptr<parsed_token>>& tokens, int num_to_skip = 0) {
		//            for (int i = 0; i < tokens.size(); i++) {
		//                if (is_match_at_index(tokens, i)) {
		//                    if (num_to_skip != 0) {
		//                        num_to_skip--;
		//                        continue;
		//                    }
		//                    return i;
		//                }
		//            }
		//            return -1;
		//        }
	};

	class function_base : public parsed_token {
	public:
		/// Why name and token?
		mycelium::token name;
		std::vector<mycelium::type> ret;
		int body_start_index, body_end_index;

		std::vector<std::shared_ptr<mycelium::parsed_token>> body;
		std::shared_ptr<mycelium::scope> scope;

		pattern_match args;

		function_base(mycelium::token token, mycelium::token name, std::vector<mycelium::type> ret, pattern_match args, parsed_token_type type, std::shared_ptr<mycelium::scope> scope) : parsed_token(std::move(token), type),
																																														  name(std::move(name)), ret(std::move(ret)), args(std::move(args)), scope(std::move(scope)), body_start_index(0), body_end_index(0) {}

		void execute() override {
			throw_error("Unimplemented Function");
		}
	};


	class return_from_function : public expression {
		public:
		std::shared_ptr<function_base> fn;
		std::shared_ptr<expression> return_value;

		return_from_function(std::shared_ptr<function_base> fn, const std::shared_ptr<expression>& value) : fn(fn), return_value(value), expression(mycelium::token("return", token_type::keyword), function_return) {}


		return_from_function(std::shared_ptr<function_base> fn) : fn(fn), return_value({}), expression(mycelium::token("return", token_type::keyword), function_return) {}

		std::string to_string() const override {
			if (return_value.get()) {
				return "Return from " + fn->to_string() + " with " + return_value->to_string();
			}

			return "Return from " + fn->to_string();
		}


		void execute() override {}

		std::shared_ptr<variable> get_value() override {
			return return_value->get_value();
		}

		mycelium::type  get_type() override {
			if (return_value.get()) {
				return return_value->get_type();
			}

			return type::none;
		}


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}
	};

	class function : public function_base {
	public:
		function(mycelium::token token, mycelium::token name, std::vector<mycelium::type> ret, pattern_match args, std::shared_ptr<mycelium::scope> scope) :
			function_base(std::move(token), std::move(name),
						  std::move(ret), std::move(args), func, std::move(scope)) {
			for (auto& var : this->args.pattern) {
				if (var.is_expression) {
					this->scope->variables.push_back(var.expr->get_value());
				}
			}
		}


		static pattern_match generate_arguments_pattern_match_from_variables(const std::vector<std::shared_ptr<variable>>& args) {
			pattern_match out;

			for (auto& arg : args) {
				out.pattern.emplace_back(arg);
				if (&arg != &args.back()) {
					out.pattern.emplace_back(",");
				}
			}

			return out;
		}

		static std::shared_ptr<variable> make_anonymous_function(std::vector<std::shared_ptr<parsed_token>> body, const std::vector<mycelium::type>& ret, const std::vector<std::shared_ptr<variable>>& args, std::shared_ptr<mycelium::scope>& scope) {
			std::shared_ptr<function> out = std::make_shared<function>(mycelium::token(""), mycelium::token(""), ret,
																	   generate_arguments_pattern_match_from_variables(args), scope);
			out->body = std::move(body);
			return variable::make_variable_without_scope(mycelium::token(), out);
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (function*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args.is_match(this->args));
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "function";
			if (!ret.empty()) {
				out += "<";
				for (int i = 0; i < ret.size(); i++) {
					out += ret[i].name;
					if (i < ret.size()-1) {
						out += ", ";
					}
				}
				out += ">";
			}
			out += " " + name.string + '(' + args.to_string() + ')';

			return out;
		}

		void execute() override {
			throw_error("Unimplemented Function: Function execute");
		}

		virtual std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& call_args) {
			for (int i = 0; i < call_args.size(); i++) {
				this->scope->variables[i]->set_value(call_args[i]);
			}
			for (auto& pt : body) {
				if (pt->type == function_return) {
					auto* return_expression = (return_from_function*)pt.get();
					return return_expression->get_value();
				}
				pt->execute();
			}
			return {};
		}
	};

	class builtin_function : public function {
	public:
		std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec;

		builtin_function(const std::string& name, std::vector<mycelium::type> ret, const std::vector<mycelium::type>& args, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, std::shared_ptr<mycelium::scope> scope) :
			function(mycelium::token(name), mycelium::token(name),
					 std::move(ret), generate_arguments_pattern_match_from_variables(variable::convert_types_to_variables(args)), std::move(scope)), exec(std::move(exec)) {
			this->body_start_index = 0;
			this->body_end_index = 0;
		}

		std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& args) override {
			return exec(args);
		}
	};

	class function_call : public expression {

	public:
		std::shared_ptr<function> fn;
		std::vector<std::shared_ptr<expression>> args;


		function_call(const std::shared_ptr<function>& fn, std::vector<std::shared_ptr<expression>> args) : fn(fn), args(std::move(args)), expression(fn->name) {}

		std::string to_string() const override {
			std::string out = "Call to " + fn->to_string() + " with ";
			for (auto& arg : args) {
				out += arg->to_string() + " ";
			}
			return out;
		}

		void execute() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			fn->call(var_args);
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}

		std::shared_ptr<variable> get_value() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			return fn->call(var_args);
		}

		mycelium::type get_type() override {
			if (this->fn->ret.empty()) {
				return type::none;
			}
			else if (this->fn->ret.size() == 1) {
				return this->fn->ret[0];
			}
			else {
				return type::tuple;
			}
		}
	};

	class operatr : public function_base {
	public:
		int number_of_args;
		int priority;

		std::vector<mycelium::token> context;

		operatr(mycelium::token token, std::vector<mycelium::token> context, mycelium::pattern_match args, std::string name, std::vector<mycelium::type> ret, int priority, std::shared_ptr<mycelium::scope> scope) : function_base(std::move(token), mycelium::token(word,std::move(name)), std::move(ret), std::move(args), parsed_token_type::oper, std::move(scope)), context(std::move(context)), priority(priority) {
			number_of_args = 0;

			for (auto& tk : this->args.pattern) {
				if (tk.is_expression) {
					number_of_args++;
					this->scope->variables.push_back(tk.expr->get_value());
				}
			}
		}


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (operatr*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->context == this->context);
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "operator ";
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
				out += ret[i].name;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}

		static std::string encode_operator(const std::string& oper);

		static std::string generate_name_from_context(std::vector<mycelium::token>& context);


		void execute() override {
			throw_error("Unimplemented Function");
		}

		virtual std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& call_args) const {
			for (int i = 0; i < call_args.size(); i++) {
				this->scope->variables[i]->set_value(call_args[i]);
			}
			for (auto& pt : body) {
				if (pt->type == function_return) {
					auto* return_expression = (return_from_function*)pt.get();
					return return_expression->get_value();
				}
				pt->execute();
			}
			return {};
		}
	};

	class builtin_operator : public operatr {
	public:
		std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec;

		builtin_operator(const std::string& op_token, const std::vector<mycelium::token>& pattern_match_tokens, const std::string& name, const std::vector<mycelium::type>& ret, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int priority, std::shared_ptr<mycelium::scope> scope) :
			operatr(mycelium::token(op_token), pattern_match_tokens, pattern_match::create_from_tokens(pattern_match_tokens), name, ret, priority, std::move(scope)), exec(std::move(exec)) {
		}

		std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& call_args) const override {
			return exec(call_args);
		}
	};

	class operator_use : public expression {
	public:
		std::shared_ptr<operatr> op;
		std::vector<std::shared_ptr<expression>> args;

		operator_use(std::shared_ptr<operatr> op, std::vector<std::shared_ptr<expression>> args) : expression(op->name), op(std::move(op)), args(std::move(args)) {}

		std::string to_string() const override {
			return "operator_use: " + op->to_string();
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return compare->token.string == this->token.string;
		}

		void execute() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			op->call(var_args);
		}

		std::shared_ptr<variable> get_value() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			return op->call(var_args);
		}

		mycelium::type get_type() override {
			if (this->op->ret.empty()) {
				return type::none;
			}
			else if (this->op->ret.size() == 1) {
				return this->op->ret[0];
			}
			else {
				return type::tuple;
			}
		}


		static std::shared_ptr<parsed_token> create_operator_call(std::shared_ptr<operatr>& op, std::vector<std::shared_ptr<parsed_token>> pts, int index) {
			std::vector<std::shared_ptr<expression>> args;
			for (int i = index; i < index + op->args.pattern.size(); i++) {
				if (pts[i]->type == var) {
					auto* var = (variable*)pts[i].get();
					args.push_back(static_cast<const std::shared_ptr<variable>>(var));
				}
			}
			return std::make_shared<operator_use>(operator_use(op, args));
		}
	};

	class conditional : public function_base {
	public:
		conditional(mycelium::token token, mycelium::token name, pattern_match args, std::shared_ptr<mycelium::scope> scope) :
			function_base(std::move(token), std::move(name),
						  {type::boolean}, std::move(args), cond, std::move(scope)) {
			for (auto& var : this->args.pattern) {
				if (var.is_expression) {
					this->scope->variables.push_back(var.expr->get_value());
				}
			}
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == cond) {
				auto* compfunc = (conditional*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args.is_match(this->args));
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "conditional ";
			out += name.string;
			out += '(';
			for (const auto & arg : args.pattern) {
				if (arg.is_expression) {
					out += arg.expr->to_string();
				}
				else {
					out += arg.oper;
				}
			}
			out += ')';
			out += ": ";
			for (int i = 0; i < ret.size(); i++) {
				out += ret[i].name;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}

		void execute() override {
			throw_error("Unimplemented Function");
		}

		virtual std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::expression>>& call_args) const {
			throw_error("Unimplemented: User Defined Conditional Calls");
			return {};
		}
	};

	class builtin_conditional : public conditional {
	public:

		std::vector<mycelium::token> args;
		std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::expression>>&)> exec;

		builtin_conditional(const std::string& name, const std::vector<mycelium::type>& args, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::expression>>&)> exec, std::shared_ptr<mycelium::scope> scope) :
			conditional(mycelium::token(name), mycelium::token(name), function::generate_arguments_pattern_match_from_variables(variable::convert_types_to_variables(args)), std::move(scope)), exec(std::move(exec)) {
		}

		std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::expression>>& call_args) const override {
			if (call_args[0]->get_type().code != type::func.code) {
				throw_error("Calling builtin conditional without function body");
			}
			return exec(call_args);
		}
	};

	class conditional_call : public expression {
	public:
		std::shared_ptr<conditional> cn;
		std::vector<std::shared_ptr<expression>> args;
		std::shared_ptr<mycelium::scope> body_scope;


		conditional_call(std::shared_ptr<conditional> cn, std::vector<std::shared_ptr<expression>> args,
						 std::shared_ptr<mycelium::scope> body_scope) : cn(std::move(cn)), args(std::move(args)), expression(cn->name), body_scope(std::move(body_scope)) {}

		std::string to_string() const override {
			std::string out = "Call to " + cn->to_string() + " with ";
			for (auto& arg : args) {
				out += arg->to_string() + " ";
			}

			out += ": Body:\n";

			for (auto& pt : args[0]->get_value()->fn_ptr->body) {
				out += "\t" + pt->to_string() + "\n";
			}

			return out;
		}

		void execute() override {
			cn->call(args);
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}

		std::shared_ptr<variable> get_value() override {
			return cn->call(args);
		}

		mycelium::type get_type() override {
			if (this->cn->ret.empty()) {
				return type::none;
			}
			else if (this->cn->ret.size() == 1) {
				return this->cn->ret[0];
			}
			else {
				return type::tuple;
			}
		}
	};

	// from: https://stackoverflow.com/questions/5100718/integer-to-hex-string-in-c
	template <typename I> std::string int_to_hex_string(I w, size_t hex_len = sizeof(I)<<1) {
		static const char* digits = "0123456789abcdef";
		std::string rc(hex_len,'0');
		for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
			rc[i] = digits[(w>>j) & 0x0f];
		return rc;
	}

	std::vector<std::string> string_split(const std::string &str, const std::vector<std::string> &spl);

	template <typename T>
	std::ostream& operator<<(std::ostream&, const std::vector<T>&);
}
