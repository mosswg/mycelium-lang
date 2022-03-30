//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>
#include <vector>

namespace mycelium {

	enum token_type {
		word,
		num,
		op,
		whitespace,
		keyword
	};

	class token {
		private:
			token_type type;
			std::string string;
		public:
			token(token_type type, std::string str) : type(type), string(std::move(str)) {}
	};

	class type {
		public:
			const int code;
			const std::string name;
			const int size;

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
	};


	class list {
		private:
			void* storage;

		public:
			list(int length) {

			}
	};

	template <typename T>
	bool vector_contains(const std::vector<T>& vec, T con);

	std::vector<std::string> string_split(const std::string& str, const std::vector<char>& spl);

}