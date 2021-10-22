#pragma once

#include <iostream>
#include <vector>
#include "Token.h"
#include "TokenKinds.h"

class Parser {
	std::vector<Token> tokenList;
	
	std::vector<Token>::iterator curToken;
public:
	Parser(std::vector<Token>& v) : tokenList(v) {
		curToken = tokenList.begin();
	}
};
