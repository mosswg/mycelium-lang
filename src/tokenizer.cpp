//
// Created by moss on 2/12/22.
//

#include "tokenizer.h"

void mycelium::tokenizer::tokenize() {

	tokens.clear();

	if (show_debug_lines) {
		std::cout << lines << std::endl;
	}

	std::vector<std::vector<std::string>> split_lines;

	std::vector<std::string> split_groups = {};

	split_groups.insert(split_groups.end(), token::whitespace_strings.begin(), token::whitespace_strings.end());
	split_groups.insert(split_groups.end(), token::oper_strings.begin(), token::oper_strings.end());
	split_groups.insert(split_groups.end(), token::grouping_strings.begin(), token::grouping_strings.end());
	split_groups.insert(split_groups.end(), token::separator_strings.begin(), token::separator_strings.end());
	split_groups.insert(split_groups.end(), token::comment_strings.begin(), token::comment_strings.end());

	for (auto& line : lines) {
		split_lines.push_back(string_split(line, split_groups));
	}

	int line_index = 1;
	for (auto& line : split_lines) {
		for (int i = 0; i < line.size(); i++) {
			std::string token_string = line[i];
			// Check for comments
			if (token_string.size() == 2 && token_string[0] == '/' && token_string[1] == '/') {
				// Skip the rest of this line
				break;
			}
			// Handle string literals with spaces in them
			if (token_string.front() == '"' && (token_string.back() != '"' || (token_string.size() == 1))) {
				for (int j = 1; (j + i) < line.size(); j++) {
					if (line[i + j].back() == '"') {
						token_string.append(line[i + j]);
						i += j;
						break;
					}
					else if (line[i + j].front() == '"') {
						token_string.append("\"");
						i += j;
						break;
					}
					token_string.append(line[i + j]);
				}
			}
			token_type type = token::find_type(token_string);
			if (type != whitespace) {
				if (token_string == "<" && line[i + 1] == "<") {
					tokens.emplace_back(op, "<<", line_index);
					i++;
				} else if (token_string == ">" && line[i + 1] == ">") {
					tokens.emplace_back(op, ">>", line_index);
					i++;
				}

				if (type == string_literal) {
					tokens.emplace_back(type, token_string.substr(1, token_string.size() - 2), line_index);
				} else {
					tokens.emplace_back(type, token_string, line_index);
				}
			}
		}
		line_index++;
	}
}

mycelium::token mycelium::tokenizer::get_next_token() {
	if (current_token_index >= this->tokens.size()) {
		return {};
	}

	return this->tokens[current_token_index++];
}

mycelium::token mycelium::tokenizer::get_next_token_without_increment() const {
	if (current_token_index >= this->tokens.size()) {
		throw_error("Next token is out of bounds");
		return {};
	}

	return this->tokens[current_token_index];
}

mycelium::token mycelium::tokenizer::get_current_token() const {
	return this->tokens[current_token_index-1];
}

mycelium::token mycelium::tokenizer::get_next_non_whitespace_token() {
	if (current_token_index >= this->tokens.size()) {
		return {};
	}

	if (this->tokens[current_token_index].type != mycelium::whitespace) {
		return this->tokens[current_token_index++];
	}

	while (this->tokens[++current_token_index].type == mycelium::whitespace && current_token_index < tokens.size());

	return this->tokens[current_token_index++];
}

std::vector<mycelium::token> mycelium::tokenizer::get_next_non_whitespace_tokens_until(const std::string& until) {
	std::vector<mycelium::token> out;

	while (this->tokens[current_token_index].string != until && current_token_index < this->tokens.size()) {
		if (this->tokens[current_token_index].type != mycelium::whitespace) {
			out.push_back(this->tokens[current_token_index]);
		}
		current_token_index++;
	}
	current_token_index++;

	return out;
}

std::vector<mycelium::token> mycelium::tokenizer::get_tokens_inside_grouping() {
	int search_depth = 0;
	std::string opening_grouping = this->tokens[current_token_index++].string;
	std::string closing_grouping = token::get_closing_grouping(opening_grouping);

	std::vector<token> out;
	while (!(search_depth == 0 && this->tokens[current_token_index].string == closing_grouping) && current_token_index < tokens.size()) {
		if (this->tokens[current_token_index].string == opening_grouping) {
			search_depth++;
		}
		else if (this->tokens[current_token_index].string == closing_grouping) {
			search_depth--;
		}
		out.push_back(this->tokens[current_token_index]);
		current_token_index++;
	}

	return out;
}

int mycelium::tokenizer::get_ending_grouping_token_index() {
	int search_depth = 0;
	std::string opening_grouping = this->tokens[current_token_index++].string;
	std::string closing_grouping = token::get_closing_grouping(opening_grouping);
	int index = current_token_index;

	while (!(search_depth == 0 && this->tokens[index].string == closing_grouping) && current_token_index < tokens.size()) {
		if (this->tokens[index].string == opening_grouping) {
			search_depth++;
		}
		else if (this->tokens[index].string == closing_grouping) {
			search_depth--;
		}
		index++;
	}
	return index;
}

void mycelium::tokenizer::skip_tokens_inside_grouping() {
	int search_depth = 0;
	std::string opening_grouping = this->tokens[current_token_index++].string;
	std::string closing_grouping = token::get_closing_grouping(opening_grouping);

	while (!(search_depth == 0 && this->tokens[current_token_index].string == closing_grouping) && current_token_index < tokens.size()) {
		if (this->tokens[current_token_index].string == opening_grouping) {
			search_depth++;
		}
		else if (this->tokens[current_token_index].string == closing_grouping) {
			search_depth--;
		}
		current_token_index++;
	}
}

bool mycelium::tokenizer::has_next_token() const {
	return current_token_index < tokens.size();
}

int mycelium::tokenizer::num_tokens_until_newline() {
	int pushed_index = this->current_token_index;
	int count, current_line = this->tokens[current_token_index - 1].line;
	for (count = 0; this->get_next_token().line == current_line; count++);
	this->current_token_index = pushed_index;
	return count;
}


std::vector<mycelium::token> mycelium::tokenizer::tokens_until_newline() {
	int pushed_index = this->current_token_index;
	std::vector<token> out;
	/// Decrementing so that we get the current token
	int count, current_line = this->tokens[--current_token_index].line;
	/// If we're on the last line we just want to return the remaining tokens
	if (this->tokens[current_token_index - 1].line == this->tokens.back().line) {
		for (int i = current_token_index; i < this->tokens.size(); i++) {
			out.push_back(tokens[i]);
		}
		this->current_token_index = pushed_index;
		return out;
	}


	token next;
	for (count = 0; (next = this->get_next_token()).line == current_line; count++) {
		out.push_back(next);
	}
	this->current_token_index = pushed_index;
	return out;
}


void mycelium::tokenizer::skip_to_newline() {
	int current_line = this->tokens[current_token_index - 1].line;

	if (current_line == this->tokens.back().line) {
		this->current_token_index = tokens.size();
		return;
	}

	for (; this->get_next_token().line == current_line;);
	this->current_token_index--;
}
