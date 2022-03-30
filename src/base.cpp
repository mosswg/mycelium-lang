//
// Created by moss on 2/12/22.
//

#include "base.h"

const mycelium::type pointer(0, "pointer", 8);
const mycelium::type integer(1, "integer", 8);
const mycelium::type character(2, "character", 1);
const mycelium::type string(3, "string", 8);
const mycelium::type list(4, "list", 8);
const mycelium::type tuple(5, "tuple", 8);
const mycelium::type oper(6, "oper", 8);
const mycelium::type array(7, "array", 8);
const mycelium::type cstring(8, "cstring", 8);
const mycelium::type token(9, "token", 8);

std::vector<std::string> mycelium::token::whitespace_strings = {};
std::vector<std::string> mycelium::token::grouping_strings = {};
std::vector<std::string> mycelium::token::keyword_strings = {};
std::vector<std::string> mycelium::token::oper_strings = {};
std::vector<std::vector<std::string>> mycelium::token::string_lists = {};
std::vector<std::string> mycelium::token::strings = {};
std::vector<std::string> mycelium::token::type_names = {};

void mycelium::initialize_static_values() {
	token::whitespace_strings.insert(token::whitespace_strings.end(), {" ", "	"});
	token::grouping_strings.insert(token::grouping_strings.end(), {"(", ")", "{", "}", "[", "]"});
	token::keyword_strings.insert(token::keyword_strings.end(), {"fn", "op", "cn"});
	token::oper_strings.insert(token::oper_strings.end(), {"+", "-", "*", "/", "%", "=", "==", "!=", "<", "<=", ">", ">=",
														   "&&", "||", "++", "--", "!", "<<", ">>", "&", "|", "~"});
	token::string_lists.insert(token::string_lists.end(), {token::oper_strings, token::whitespace_strings,
														   token::grouping_strings, token::keyword_strings});

	mycelium::token::type_names.insert(mycelium::token::type_names.end(), {"operator", "whitespace", "grouping", "keyword", "word", "num",
													 "invalid", "newline"});


	for (const auto& token_string_list : token::string_lists) {
		for (const auto& token_string : token_string_list) {
			token::strings.push_back(token_string);
		}
	}
}

template <typename T>
bool mycelium::vector_contains(const std::vector<T>& vec, const T& con) {
	for (const auto &v: vec) {
		if (v == con) return true;
	}
	return false;
}

template <typename T>
std::ostream& mycelium::operator<<(std::ostream& os, const std::vector<T>& dt) {
	if (dt.empty()) {
		os << "{}";
		return os;
	}

	int i = 0;

	os << "{ ";

	for (; i < dt.size() - 1; i++) {
		if (std::is_same<T, std::string>::value) {
			os << "\"" << dt[i] << "\"" << ", ";
		}
		else {
			os << dt[i] << ", ";
		}
	}

	if (std::is_same<T, std::string>::value) {
		os << "\"" << dt[i] << "\"" << " }";
	}
	else {
		os << dt[i] << " }";
	}

	return os;
}

std::vector<std::string> mycelium::string_split(const std::string& str, const std::vector<std::string>& spl) {
	if (str.empty()) {
		return {};
	}

	std::vector<std::string> out;

	size_t pos;
	size_t prev_pos = 0;
	size_t min_pos;
	std::string split;

	while (prev_pos < str.length()) {

		min_pos = -1;

		for (auto &comp: spl) {
			if ((pos = str.find(comp, prev_pos)) != std::string::npos) {
				if (pos < min_pos || min_pos == -1) {
					min_pos = pos;
					split = comp;
				}
			}
		}

		if (min_pos == -1) {
			break;
		}

		if (min_pos != prev_pos) {
			out.push_back(str.substr(prev_pos, min_pos - prev_pos));
		}

		out.push_back(split);

		prev_pos = min_pos + split.length();
	}
	if (min_pos >= str.length()) {
		out.push_back(str.substr(prev_pos, min_pos - prev_pos));
	}

	return out;
}

mycelium::token_type mycelium::token::find_type(const std::string &string) {
	auto type = 0;
	for (auto& token_string_list : string_lists) {
		if (mycelium::vector_contains(token_string_list, string)) {
			return (token_type)type;
		}
		type++;
	}

	bool is_num = true;

	for (char c: string) {
		if (!isdigit(c)) {
			is_num = false;
		}
	}

	if (is_num) {
		return num;
	} else if (isdigit(string.c_str()[0])) {
		return invalid;
	} else {
		return word;
	}
}
