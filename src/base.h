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
		static const char* RESET = "\033[0m";
		static const char* BLACK = "\033[30m";      /* Black */
		static const char* RED = "\033[31m";      /* Red */
		static const char* GREEN = "\033[32m";      /* Green */
		static const char* YELLOW = "\033[33m";      /* Yellow */
		static const char* BLUE = "\033[34m";      /* Blue */
		static const char* MAGENTA = "\033[35m";      /* Magenta */
		static const char* CYAN = "\033[36m";      /* Cyan */
		static const char* WHITE = "\033[37m";      /* White */
		static const char* BOLDBLACK = "\033[1m\033[30m";      /* Bold Black */
		static const char* BOLDRED = "\033[1m\033[31m";      /* Bold Red */
		static const char* BOLDGREEN = "\033[1m\033[32m";      /* Bold Green */
		static const char* BOLDYELLOW = "\033[1m\033[33m";      /* Bold Yellow */
		static const char* BOLDBLUE = "\033[1m\033[34m";      /* Bold Blue */
		static const char* BOLDMAGENTA = "\033[1m\033[35m";      /* Bold Magenta */
		static const char* BOLDCYAN = "\033[1m\033[36m";      /* Bold Cyan */
		static const char* BOLDWHITE = "\033[1m\033[37m";      /* Bold White */
	}

	static bool show_debug_lines = false;

	void throw_error(const std::string& error);

	void warn(const std::string& warning);

	void initialize_static_values();

	template<typename T>
	bool vector_contains(const std::vector<T> &vec, const T &con);

	std::string escape_string(std::string string);




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
