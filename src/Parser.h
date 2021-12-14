#pragma once

#include <iostream>
#include <vector>

#include "Token.h"
#include "grammer/LR1.h"
#include "AST.h"

class Parser {
	LR1 lr1;
	AST::node* root;
	std::vector<Token> tokenList;
	std::vector<State> parserTable;
	std::vector<Token>::iterator curToken;
public:
	Parser(std::vector<Token> v) : tokenList(v) {
		curToken = tokenList.begin();
		parserTable = lr1.get_parser_table();
		root = new AST::node(lr1.get_start_symbol());
	}

	/// get next token
	bool next();

	/// return value refers wether a successful parse
	int build_ast_tree();

	std::string json_print() {
		auto str = root->json_print().ToFormattedString();
		std::string s;
		bool flg = false;
		for (auto ch : str) {
			if (flg) {
				if (ch != ' ' && ch != '\t') {
					s += ' ';
					s += ch;
					flg = false;
				}
				continue;
			}
			s += ch;
			if (ch == ':')
				flg = true;
		}
		return s;
	}

	void print() {
		root->print();
	}

	AST::node* get_root() {
		return root;
	}
};
