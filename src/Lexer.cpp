#include "Lexer.h"

void Lexer::next() {
	++curPos;
	++col;
	if (curPos < text.length()) {
		curChar = text[curPos];
	}
}

Token Lexer::get_number() {
	std::string res = "";
	int stCol = col;
	while (!end_of_file() && isdigit(curChar)) {
		res += curChar;
		next();
	}
	return Token(TokenKind::INT, row, stCol, res);
}

Token Lexer::get_identifier() {
	std::string res;
	int stCol = col;
	while (!end_of_file() && (isalnum(curChar) || curChar == '_')) {
		res += curChar;
		next();
	}
	return Token(TokenKind::INT, row, stCol, res);
}

Token Lexer::get_next_token() {
	while (!end_of_file()) {
		if (isalpha(curChar)) {
			return get_identifier();
		}
		else if (isdigit(curChar)) {
			return get_number();
		}
		else if (curChar == ' ' || curChar == '\t') {
			next();
		}
		else if (curChar == '+') {
			next();
			return Token(TokenKind::PLUS, row, col, "+");
		}
		else if (curChar == '-') {
			next();
			return Token(TokenKind::MINUS, row, col, "-");
		}
		else if (curChar == '*') {
			next();
			return Token(TokenKind::MULT, row, col, "*");
		}
		else if (curChar == '/') {
			next();
			return Token(TokenKind::DIVIDE, row, col, "/");
		}
		else if (curChar == '=') {
			next();
			return Token(TokenKind::EQUAL, row, col, "=");
		}
		else if (curChar == '(') {
			next();
			return Token(TokenKind::L_PAREN, row, col, "(");
		}
		else if (curChar == ')') {
			next();
			return Token(TokenKind::R_PAREN, row, col, ")");
		}
		else if (curChar == '{') {
			next();
			return Token(TokenKind::L_BRACE, row, col, "{");
		}
		else if (curChar == '}') {
			next();
			return Token(TokenKind::R_BRACE, row, col, "{");
		}
		else if (curChar == ',') {
			next();
			return Token(TokenKind::COMMA, row, col, ",");
		}
		else if (curChar == ';') {
			next();
			return Token(TokenKind::SEMI, row, col, ";");
		}
		else if (curChar == '\n') {
			++row;
			col = 0;
			next();
		}
		else {
			std::cerr << "unknown character:" << curChar << ";" << std::endl;
		}
	}
	return Token(TokenKind::E_O_F, row, col, "EOF");
}
