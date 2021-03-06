#pragma once

#include <string>

constexpr int keywordsNum = 6;
static std::string keywords[keywordsNum] = {"if", "else", "while", "return", "int", "void", };

enum class TokenKind : unsigned short {
	/// no type
	UNKNOWN, 

	/// number
	INTEGER,

	/// reserved keywords
	INT,
	Void,
	IF,
	ELSE,
	RETURN,
	WHILE,

	KEYWORDS, // if while ...
	/// identifer
	IDENTIFER,

	/// number
	
	/// algebra
	PLUS, // "+"
	MINUS, // "-"
	MULT, // "*"
	DIVIDE, // "/"
	EQUAL, // "="
	EQUAL_EQUAL, // "=="
	GREATER, // ">"
	LESS, // "<"
	L_EQUAL, // "<="
	R_EQUAL, // ">="
	N_EQUAL, // "!="

	/// scope
	L_PAREN, // "("
	R_PAREN, // ")"
	L_BRACE, // "{"
	R_BRACE, // "}"

	/// comment
	S_COMMENT, // "//"
	L_COMMENT, // "/*"
	R_COMMENT, // "*/"

	/// symbol
	COMMA, // ","
	SEMI, // ";"
	E_O_F, // "#"
};