#pragma once

#include <vector>
#include <string>

namespace mycelium {
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
		boolean_literal,
		invalid
	};

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
		static std::string boolean_true;
		static std::string boolean_false;


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

}
