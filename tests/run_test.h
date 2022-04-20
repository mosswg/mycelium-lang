//
// Created by moss on 4/20/22.
//

#pragma once

#include <iostream>
#include <base.h>

void print_failed() {
	std::cerr << '[' << mycelium::RED << "FAILED" << mycelium::RESET << "]:  ";
}

void print_success() {
	std::cerr << '[' << mycelium::GREEN << "SUCCESS" << mycelium::RESET << "]: ";
}

void print_test_name(const std::string& name) {
	std::cerr << mycelium::YELLOW << name << mycelium::RESET << " ";
}