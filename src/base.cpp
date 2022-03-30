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