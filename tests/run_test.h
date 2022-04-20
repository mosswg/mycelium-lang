//
// Created by moss on 4/20/22.
//

#pragma once

#include <iostream>
#include <base.h>

void print_failed() {
	std::cerr << '[' << mycelium::colored_text::RED << "FAILED" << mycelium::colored_text::RESET << "]:  ";
}

void print_success() {
	std::cerr << '[' << mycelium::colored_text::GREEN << "SUCCESS" << mycelium::colored_text::RESET << "]: ";
}

void print_test_name(const std::string& name) {
	std::cerr << mycelium::colored_text::YELLOW << name << mycelium::colored_text::RESET << " ";
}