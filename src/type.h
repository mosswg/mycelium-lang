#pragma once

#include "token.h"
#include <string>
#include <vector>
#include <memory>

namespace mycelium {
	// Forward declare this to avoid include loop cause why allow you to include something unless you know everything about it
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
}
