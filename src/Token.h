#pragma once

#include "json/CJsonObject.hpp"
#include "grammer/CFG.h"
#include "TokenKinds.h"

#include <iomanip>
static int token_id = 0;

class Token {
	/// token type
	TokenKind kind = TokenKind::UNKNOWN;

	/// Location of the token
	unsigned int row = 1, col = 1;

	/// token data
	std::string data;

	/// unique id
	int id;

public:
	Token() { id = ++token_id; }
	Token(TokenKind k, int row, int col, std::string data) : kind(k), row(row), col(col), data(data) { id = ++token_id; }
	TokenKind get_kind() { return kind; }
	unsigned int get_row() { return row; }
	unsigned int get_col() { return col; }
	int get_id() { return id; }
	std::string get_data() { return data; }
	
	/// transfer to a symbol type
	Grammer::Symbol get_name() {
		using Grammer::Symbol;
		if (is(TokenKind::INTEGER))
			return Symbol("num");
		if (is(TokenKind::IDENTIFER))
			return Symbol("identifier");
		return Symbol(data);
	}
	
	void setKind(TokenKind k) { kind = k; }

	bool operator == (Grammer::Symbol& rhs) {
		if (rhs.get_type() == Grammer::SymbolType::N_TERMINAL)
			return false;
		
		if (rhs.get_name() == "num") {
			return is(TokenKind::INTEGER);
		}

		if (rhs.get_name() == "identifier") {
			return is(TokenKind::IDENTIFER);
		}

		return rhs.get_name() == data;
	}

	bool operator != (Grammer::Symbol rhs) {
		return !(*this == rhs);
	}

	friend std::ostream& operator << (std::ostream& out, Token& tk) {
		out << std::string("id=") + std::to_string(tk.get_id()) << ", ";
		out << "value=" << std::string(" \"") + tk.get_data() + "\", ";
		out << "Loc=<" << tk.get_row() << ":" << tk.get_col() << ">";
		return out;
	}

	/// type judge
	bool is(TokenKind k) { return kind == k; }
	bool is_not(TokenKind k) { return kind != k; }
	bool is_one_of(TokenKind k1, TokenKind k2) {
		return is(k1) || is(k2);
	}
	template <typename... Ts>
	bool is_one_of(TokenKind k1, TokenKind k2, Ts... ks) {
		return is(k1) || is_one_of(k2, ks...);
	}

	/// temporaly use this
	std::string json_print() {
		neb::CJsonObject json;
		json.Add("type", get_tokenkind_name());
		json.Add("val", get_data());
		json.AddEmptySubObject("pos");
		json["pos"].Add("row", get_row());
		json["pos"].Add("col", get_col());
		return json.ToFormattedString();
	}

	std::string get_tokenkind_name()
	{
		switch (kind)
		{
			case TokenKind::INTEGER:
				return "integer";
				break;
			case TokenKind::INT:
				return "kw_int";
				break;
			case TokenKind::Void:
				return "kw_void";
				break;
			case TokenKind::IF:
				return "kw_if";
				break;
			case TokenKind::ELSE:
				return "kw_else";
				break;
			case TokenKind::RETURN:
				return "kw_return";
				break;
			case TokenKind::WHILE:
				return "kw_while";
				break;
			case TokenKind::KEYWORDS:
				return "keywords";
				break;
			case TokenKind::IDENTIFER:
				return "identifer";
				break;
			case TokenKind::PLUS:
				return "plus";
				break;
			case TokenKind::MINUS:
				return "minus";
				break;
			case TokenKind::MULT:
				return "mult";
				break;
			case TokenKind::DIVIDE:
				return "div";
				break;
			case TokenKind::EQUAL:
				return "equal";
				break;
			case TokenKind::EQUAL_EQUAL:
				return "equal_equal";
				break;
			case TokenKind::L_EQUAL:
				return "l_equal";
				break;
			case TokenKind::R_EQUAL:
				return "r_equal";
				break;
			case TokenKind::N_EQUAL:
				return "equal";
				break;
			case TokenKind::L_PAREN:
				return "l_paren";
				break;
			case TokenKind::R_PAREN:
				return "r_paren";
				break;
			case TokenKind::L_BRACE:
				return "l_brace";
				break;
			case TokenKind::R_BRACE:
				return "r_brace";
				break;
			case TokenKind::COMMA:
				return "comma";
				break;
			case TokenKind::SEMI:
				return "semi";
				break;
			case TokenKind::GREATER:
				return "greater";
				break;
			case TokenKind::LESS:
				return "less";
				break;
			case TokenKind::E_O_F:
				return "eof";
				break;
			default: return "UNKNOW";
		}
	}
};
