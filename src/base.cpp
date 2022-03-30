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

template <typename T>
bool mycelium::vector_contains(const std::vector<T>& vec, T con) {
	for (const auto &v: vec) {
		if (v == con) return true;
	}
	return false;
}

std::vector<std::string> mycelium::string_split(const std::string& str, const std::vector<char>& spl) {
	std::vector<std::string> out;

	int start = 0;

	for (int i = 0; i < str.length(); i++) {
		// std::cout << str[i] << ": " << i << ": " << start << ": " << (vector_contains(spl, str[i])) << "\n";
		if (vector_contains(spl, str[i])) {
			out.push_back(str.substr(start, i));
			start = i+1;
		}
	}

	if (vector_contains(spl, (char)1)) {
		out.push_back(str.substr(start, str.length()));
	}

	return out;
}