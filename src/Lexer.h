#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "token.h"

class Lexer {
	/// code
	std::string text;

	/// current position
	unsigned int row, col;

	/// char pointer
	unsigned int curPos;
	char curChar;

public:
	Lexer(std::ifstream& in) {
		std::ostringstream tmp;
		tmp << in.rdbuf();
		text = tmp.str();
		
		row = 1;
		col = 1;
		curPos = 0;
		curChar = text[curPos];
	}

	/// check if read all char
	bool end_of_file() { return curPos >= text.length(); }

	/// add the char pointer by one
	void next();

	/// obtain the next token
	Token get_next_token();
	Token get_number();
	Token get_identifier();

	/// check curChar type
	bool is_space() { return curChar == ' '; }

	/// test function
	void test() {
		std::cout << text << std::endl;
		while (true) {
			Token t = get_next_token();
			std::cout << t.json_print() << std::endl;
			if (t.get_data() == "EOF") return;
		}
	}
};
