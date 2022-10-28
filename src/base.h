//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <memory>
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







	static bool show_debug_lines = false;

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
		int code;
		std::string name;
		int size;

		static std::vector<std::string> strings;

		type(const int code, std::string name, const int size) : code(code), name(std::move(name)), size(size) {}

		static const type reference;
		static const type integer;
		static const type character;
		static const type string;
		static const type list;
		static const type tuple;
		static const type oper;
        static const type func;
		static const type array;
		static const type cstring;
		static const type token;
		static const type none;
		static std::vector<type> types;

        static int validate_type(const mycelium::token& type) {
            for (int i = 0; i < type::strings.size(); i++) {
                if (type::strings[i] == type.string) {
                    if (show_debug_lines) {
                        std::cout << "type \"" << type.string << "\" validated" << std::endl;
                    }
                    return i;
                }
            }
            mycelium::throw_error("unknown type \"" + type.string + "\"", 50001);
            return -1;
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
		oper_use,
        func_call,
		bad
	};

	class parsed_token {
	public:

		mycelium::token token;

		mycelium::parsed_token_type type;

		parsed_token() : token(invalid, ""), type(bad) {}

		explicit parsed_token(mycelium::token token, parsed_token_type type) : token(std::move(token)), type(type) {}

        virtual ~parsed_token() = default;

        virtual std::string to_string() const = 0;

        virtual void execute() = 0;

		virtual bool is_similar(std::shared_ptr<parsed_token> compare) = 0;
	};

	class variable : public parsed_token {
	public:
		mycelium::type type;
		long value = 0;

		int parent_scope;

		variable(mycelium::token name, mycelium::type type, int parent_scope) : parsed_token(std::move(name), var), type(std::move(type)), parent_scope(parent_scope) {
            if (this->type.code == type::string.code) {
                this->value = (long)new std::string;
            }
            else if (this->type.code == type::list.code) {
                throw_error("Unsupported Creation of list", 61001);
            }
            else if (this->type.code == type::tuple.code) {
                throw_error("Unsupported Creation of tuple", 61001);
            }
            else {
                this->value = 0;
            }
        }

        variable(mycelium::token name, mycelium::type type, int parent_scope, long value) : parsed_token(std::move(name), var), type(std::move(type)), parent_scope(parent_scope), value(value) {}

        ~variable() override {
            this->destroy();
        }

		std::string to_string() const override {
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

        void destroy() {
            if (type.code == type::string.code) {
                delete ((std::string*)value);
            }
            else if (type.code == type::list.code) {
                delete ((std::vector<variable>*)value);
            }
            else if (type.code == type::tuple.code) {
                throw_error("Unsupported Destroy of tuple", 61001);
            }

            this->type = type::none;
            this->value = 0;
            this->parent_scope = -1;
        }

        void execute() override {}

        virtual void set_value(long value) {
            this->value = value;
        }

        virtual long get_value() {
            return this->value;
        }

        virtual std::string get_as_string() {
            if (type.code == type::string.code) {
                return *((std::string*)value);
            }
            else if (type.code == type::list.code) {
                throw_error("Unsupported Get As String of list", 61001);
            }
            else if (type.code == type::tuple.code) {
                throw_error("Unsupported Get As String of tuple", 61001);
            }
            else {
                return std::to_string(this->value);
            }
            return "ERROR IN STRING CONVERSION";
        }
	};

    class constant : public variable {
    public:
        constant(mycelium::type type, long value) : variable({}, std::move(type), 0, value) {}


        void set_value(long value) override {
            throw_error("Cannot Set Value of Constant", 61001);
        }

        long get_value() override {
            return this->value;
        }
    };

	class function_base : public parsed_token {
	public:
        /// Why name and token?
		mycelium::token name;
		std::vector<mycelium::type> ret;

		std::vector<std::shared_ptr<mycelium::parsed_token>> body;

		int scope;
        int parent_scope;

		function_base(mycelium::token token, mycelium::token name, std::vector<mycelium::type> ret, parsed_token_type type, int scope, int parent_scope) : parsed_token(std::move(token), type),
																										name(std::move(name)), ret(std::move(ret)), scope(scope), parent_scope(parent_scope) {}


        void execute() override {
            throw_error("Unimplemented Function", 10001);
        }
    };

	class function : public function_base {
	public:
		std::vector<mycelium::type> args;

		function(mycelium::token token, mycelium::token name, std::vector<mycelium::type> ret, std::vector<mycelium::type> args, int scope, int parent_scope) :
																				function_base(std::move(token), std::move(name),
																							  std::move(ret), func, scope, parent_scope), args(std::move(args)) {}


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (function*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args == this->args);
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "function ";
			out += name.string;
			out += '(';
			for (int i = 0; i < args.size(); i++) {
				out += args[i].name;
				if (i < args.size()-1) {
					out += ", ";
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
            throw_error("Unimplemented Function: Function execute", 10001);
        }

        virtual void call(std::vector<std::shared_ptr<mycelium::variable>>& vars) {
            std::cout << "Calling function: " << this->name.string << " with " << vars[0]->to_string();
            throw_error("Unimplemented Function: Function Calls", 10001);
        }
	};


    class builtin_function : public function {
    public:
        std::vector<mycelium::token> args;
        std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec;

        builtin_function(const std::string& name, std::vector<mycelium::type> ret, std::vector<mycelium::type> args, std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int scope) :
        function(mycelium::token(name), mycelium::token(name),
                std::move(ret), std::move(args), scope, 0), exec(std::move(exec)) {
        }

        void call(std::vector<std::shared_ptr<mycelium::variable>>& vars) override {
            exec(vars);
        }
    };

    class function_call : public parsed_token {
        std::shared_ptr<function> fn;
        std::vector<std::shared_ptr<variable>> args;

    public:

        function_call(std::shared_ptr<function> fn, std::vector<std::shared_ptr<variable>> args) : parsed_token({}, func_call), fn(std::move(fn)), args(std::move(args)) {}

        std::string to_string() const override {
            std::string out = "Call to " + fn->to_string() + " with ";
            for (auto& arg : args) {
                out += arg->to_string() + " ";
            }
            return out;
        }

        void execute() override {
            fn->call(args);
        }

        bool is_similar(std::shared_ptr<parsed_token> compare) override {
            return false;
        }
    };


    class pattern_token {
    public:
        std::string oper;
        std::shared_ptr<variable> var;
        bool is_variable = false;

        explicit pattern_token(std::shared_ptr<variable> var) : var(std::move(var)), is_variable(true) {}

        explicit pattern_token(const variable& var) : var(std::make_shared<variable>(var)), is_variable(true) {}

        explicit pattern_token(std::string oper) : var(std::make_shared<variable>(variable(token(""), type::none, 0))), oper(std::move(oper)), is_variable(false) {}
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
                        throw_error("Two type name in a row in operator definition", 30004);
                    }
                    current_type = tk;
                }
                else if (tk.type == word) {
                    if (current_type.type != token_type::invalid) {
                        out.pattern.emplace_back(variable(tk, type::types[type::validate_type(current_type)], 0));
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
            return create_from_tokens(tks, 0, tks.size());
        }

        std::vector<std::shared_ptr<variable>> get_variables() {
            std::vector<std::shared_ptr<variable>> out;
            for (auto& tk : pattern) {
                if (tk.is_variable) {
                    out.push_back(tk.var);
                }
            }
            return out;
        }

        bool is_match(const pattern_match& test_pattern) {
            long test_size = test_pattern.pattern.size();

            if (test_size != pattern.size()) {
                std::cout << "not long enough :(\n";
                std::cout << test_size << '\n' << pattern.size() << std::endl;
                return false;
            }

            for (int i = 0; i < test_size; ++i) {
                if (test_pattern.pattern[i].is_variable != pattern[i].is_variable) {
                    return false;
                }
                else {
                    if (pattern[i].is_variable) {
                        std::shared_ptr<variable> test_var = test_pattern.pattern[i].var;
                        std::shared_ptr<variable> pattern_var = pattern[i].var;
                        if (test_var->type.code != pattern_var->type.code) {
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

    class operatr : public function_base {
	public:
		pattern_match pattern;

        int number_of_args;

		std::vector<mycelium::token> context;

		operatr(mycelium::token token, std::vector<mycelium::token> context, mycelium::pattern_match pattern_match, std::string name, std::vector<mycelium::type> ret, int scope, int parent_scope) : function_base(std::move(token),
                         mycelium::token(word,std::move(name)),
                         std::move(ret), parsed_token_type::oper,
                             scope, parent_scope),
                             context(std::move(context)),
                             pattern(std::move(pattern_match)) {
            number_of_args = 0;

            for (auto& tk : pattern.pattern) {
                if (tk.is_variable) {
                    number_of_args++;
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
            throw_error("Unimplemented Function", 10001);
        }

        virtual void call(std::vector<std::shared_ptr<mycelium::variable>>& args) const {
            std::cout << "Calling operator: " << this->name.string << " with " << args[0]->to_string() << " and " << args[1]->to_string();
            throw_error("Unimplemented Function: Function Calls", 10001);
        }
	};

    class builtin_operator : public operatr {
    public:
        std::vector<mycelium::token> args;
        std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec;

        builtin_operator(const std::string& op_token, const mycelium::pattern_match& pattern_match, const std::string& name, const std::vector<mycelium::type>& ret, std::function<void(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int scope) :
                operatr(mycelium::token(op_token), {}, pattern_match, name, ret, scope, 0), exec(std::move(exec)) {
        }

        void call(std::vector<std::shared_ptr<mycelium::variable>>& args) const override {
            exec(args);
        }
    };

    class operator_use : public parsed_token {
    public:
        const operatr& op;

        std::vector<std::shared_ptr<variable>> args;

        operator_use(mycelium::token token, const operatr& op, std::vector<std::shared_ptr<variable>> args) : parsed_token(std::move(token), oper_use), op(op), args(std::move(args)) {}

        std::string to_string() const override {
            return "operator_use: " + op.to_string();
        }

        bool is_similar(std::shared_ptr<parsed_token> compare) override {
            return compare->token.string == this->token.string;
        }

        void execute() override {
            op.call(args);
        }


        static std::shared_ptr<parsed_token> create_operator_call(const operatr& op, std::vector<std::shared_ptr<parsed_token>> pts, int operator_index, int index) {
            std::vector<std::shared_ptr<variable>> args;
            for (int i = index; i < index + op.pattern.pattern.size(); i++) {
                if (pts[i]->type == var) {
                    auto* var = (variable*)pts[i].get();
                    args.push_back(static_cast<const std::shared_ptr<variable>>(var));
                }
            }
            return std::make_shared<operator_use>(operator_use(op.token, op, args));
        }
    };

	class conditional : public function_base {
	public:
		std::vector<mycelium::token> args;

		conditional(mycelium::token token, mycelium::token name, std::vector<mycelium::token> args, int scope, int parent_scope) : function_base(std::move(token), std::move(name),
																																				   {{}}, parsed_token_type::cond, scope, parent_scope),
																																				   args(std::move(args)) {}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (conditional*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args == this->args);
			}
			return false;
		}

		std::string to_string() const override {
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
				out += ret[i].name;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}

        void execute() override {
            throw_error("Unimplemented Function", 10001);
        }

        void call(std::vector<mycelium::variable>& vars) {
            std::cout << "Calling conditional: " << this->name.string << " with " << vars[0].to_string();
            throw_error("Unimplemented Function: Function Calls", 10001);
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