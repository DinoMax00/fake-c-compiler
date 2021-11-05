#pragma once

#include <iostream>
#include <iomanip>
#include <Windows.h>

#include "grammer/GrammerRules.h"
#include "Lexer.h"

void print_productions() {
	using std::cout;
	for (int i = 0; i < listLength; ++i) {
		if (rules[i][0] == END_SYMBOL)
			break;

		if (i && rules[i][0] == rules[i - 1][0]) {
			for (int j = 0; j < (int)(rules[i][0].length() + 4); ++j)
				putchar(' ');
			cout << "| ";
		}
		else {
			cout << rules[i][0] << " -->  ";
		}
		for (int j = 1; j < tokenNum; ++j) {
			if (rules[i][j] == END_SYMBOL)
				break;
			cout << rules[i][j] << " ";
		}
		cout << std::endl;
	}
}

void print_tokens(const char* filename, Lexer* lexer) {
	using std::cout;
	using std::endl;

	auto vec = lexer->get_token_list();
	for (auto& it: vec) {
		cout << std::setw(12) << std::setiosflags(std::ios::left) << it.get_tokenkind_name();
		cout << std::setw(8) << std::setiosflags(std::ios::left) << std::string("id=") + std::to_string(it.get_id());
		cout << std::setw(13) << std::setiosflags(std::ios::left) << std::string(" \"") + it.get_data() + "\"";
		cout << " Loc=<" << filename << ":" << it.get_row() << ":" << it.get_col() << ">";
		cout << endl;
	}
}

void print_tokens_json(const char* filename, Lexer* lexer) {
	using std::cout;
	using std::endl;

	auto vec = lexer->get_token_list();
	for (auto& it : vec) {
		cout << it.json_print() << endl;
	}
}
