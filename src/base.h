//
// Created by moss on 2/11/22.
//
#pragma once

#include <string>
#include <iostream>

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
