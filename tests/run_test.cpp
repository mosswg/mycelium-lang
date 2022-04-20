//
// Created by moss on 4/20/22.
//
#include "pattern_match.h"

int main() {
	bool tests_passed = true;

	if (!test_pattern_match()) {
		tests_passed = false;
	}

	return tests_passed ? 0 : -1;
}