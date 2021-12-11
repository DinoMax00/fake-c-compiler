#include "Lexer.h"

void Lexer::next() {
	++curPos;
	++col;
	if (curPos < text.length()) {
		curChar = text[curPos];
	}
}

std::vector<Token> Lexer::get_token_list() {
	std::vector<Token> v;
	while (true) {
		auto tk = get_next_token();
		if (tk.get_kind() != TokenKind::UNKNOWN)
			v.push_back(tk);
		if (tk.get_kind() == TokenKind::E_O_F)
			break;
	}
	return v;
}

Token Lexer::get_number() {
	std::string res = "";
	int stCol = col;
	while (!end_of_file() && isdigit(curChar)) {
		res += curChar;
		next();
	}
	return Token(TokenKind::INTEGER, row, stCol, res);
}

void Lexer::remove_comment() {
	while (!end_of_file() && curChar != '\n')
		next();
}

void Lexer::remove_comment_mult() {
	int stRow = row;
	while (!end_of_file()) {
		next();
		if (curChar == '*') {
			next();
			if (curChar == '/') {
				next();
				return;
			}
		}
	}
	std::cerr << "illegal comment at row: " << stRow << std::endl;
	exit(0);
}

Token Lexer::get_div() {
	next();
	if (curChar == '*') {
		remove_comment_mult();
		return Token(TokenKind::UNKNOWN, row, col, "");
	}
	else if (curChar == '/') {
		remove_comment();
		return Token(TokenKind::UNKNOWN, row, col, "");
	}
	else 
		return Token(TokenKind::DIVIDE, row, col, "/");
}

Token Lexer::get_equal() {
	next();
	if (curChar == '=') {
		next();
		return Token(TokenKind::EQUAL_EQUAL, row, col, "==");
	}
	else {
		return Token(TokenKind::EQUAL, row, col, "=");
	}
}

Token Lexer::get_greater() {
	next();
	if (curChar == '=') {
		next();
		return Token(TokenKind::R_EQUAL, row, col, ">=");
	}
	else {
		return Token(TokenKind::GREATER, row, col, ">");
	}
}

Token Lexer::get_not() {
	next();
	if (curChar == '=') {
		next();
		return Token(TokenKind::N_EQUAL, row, col, "!=");
	}
	else {
		/// ������ҵ��û�У������
		std::cerr << "unknown symbol '!' at row: " << row << " col: " << col << std::endl;
		exit(0);
	}
}

Token Lexer::get_less() {
	next();
	if (curChar == '=') {
		next();
		return Token(TokenKind::R_EQUAL, row, col, "<=");
	}
	else {
		return Token(TokenKind::GREATER, row, col, "<");
	}
}

Token Lexer::get_identifier() {
	std::string res;
	int stCol = col;
	while (!end_of_file() && (isalnum(curChar) || curChar == '_')) {
		res += curChar;
		next();
	}
	if (res == "int") 
		return Token(TokenKind::INT, row, stCol, res);
	else if (res == "void")
		return Token(TokenKind::VOID, row, stCol, res);
	else if (res == "if")
		return Token(TokenKind::IF, row, stCol, res);
	else if (res == "else")
		return Token(TokenKind::ELSE, row, stCol, res);
	else if (res == "while")
		return Token(TokenKind::WHILE, row, stCol, res);
	else if (res == "return")
		return Token(TokenKind::RETURN, row, stCol, res);
	else
		return Token(TokenKind::IDENTIFER, row, stCol, res);
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
			return get_div();
		}
		else if (curChar == '=') {
			return get_equal();
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
			return Token(TokenKind::R_BRACE, row, col, "}");
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
		else if (curChar == '#') {
			next();
			return Token(TokenKind::E_O_F, row, col, "#");
		}
		else if (curChar == '>') {
			return get_greater();
		}
		else if (curChar == '<') {
			return get_less();
		}
		else if (curChar == '!') {
			return get_not();
		}
		else {
			std::cerr << "unknown character:" << curChar << ";" << std::endl;
			exit(0);
		}
	}
	return Token(TokenKind::E_O_F, row, col, "#");
}
