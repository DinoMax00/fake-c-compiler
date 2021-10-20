#pragma once

enum class TokenKind : unsigned short {
	/// no type
	UNKNOWN, 

	/// reserved keywords
	INT,
	VOID,
	IF,
	ELSE,
	RETURN,

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
