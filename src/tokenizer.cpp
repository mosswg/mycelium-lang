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
	split_groups.insert(split_groups.end(), token::grouping_strings.begin(), token::grouping_strings.end());

	for (auto& line : lines) {
		split_lines.push_back(string_split(line, split_groups));
	}

	for (auto& line : split_lines) {
		for (int i = 0; i < line.size(); i++) {
			std::string token_string = line[i];
            if (token_string.front() == '"' && token_string.back() != '"') {
                for (int j = 1; j < i + line.size(); i++) {
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
					tokens.emplace_back(op, "<<");
					i++;
				} else if (token_string == ">" && line[i + 1] == ">") {
					tokens.emplace_back(op, ">>");
					i++;
				}

                if (type == string_literal) {
                    tokens.emplace_back(type, token_string.substr(1, token_string.size() - 2));
                } else {
					tokens.emplace_back(type, token_string);
				}
			}
		}
		if (tokens.back().type != newline) {
			tokens.emplace_back(newline, "");
		}
	}
}

mycelium::token mycelium::tokenizer::get_next_token() {
    if (current_token_index >= this->tokens.size()) {
        return {};
    }

    return this->tokens[current_token_index++];
}

mycelium::token mycelium::tokenizer::get_next_token_without_increment() {
    if (current_token_index >= this->tokens.size()) {
        std::cerr << "Next token is out of bounds\n";
        return {};
    }

    return this->tokens[current_token_index];
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