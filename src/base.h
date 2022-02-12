//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>
#include <vector>


namespace mycelium {

	class type {
		public:
			const int pointer = 0;
			const int integer = 1;
			const int character = 2;
			const int string = 3;
			const int list = 4;
			const int tuple = 5;
			const int oper = 6;
			const int array = 7;
			const int cstring = 8;

			std::string names[9] = {"pointer", "integer", "char", "string", "list", "tuple", "oper", "array", "cstring"};

			int size[9] = {8, 8, 1, 8, 8, 8, 8, 8, 8};
	};


	class list {
		private:
			void* storage;

		public:
			list(int length) {

			}
	};

	template <typename T>
	bool vector_contains(const std::vector<T>& vec, T con) {
		for (const auto &v: vec) {
			if (v == con) return true;
		}
		return false;
	}

	std::vector<std::string> string_split(const std::string& str, const std::vector<char>& spl) {
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

}