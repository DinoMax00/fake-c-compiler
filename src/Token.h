#pragma once

#include "json/CJsonObject.hpp"
#include "grammer/CFG.h"
#include "TokenKinds.h"

class Token {
	/// token type
	TokenKind kind = TokenKind::UNKNOWN;

	/// Location of the token
	unsigned int row = 1, col = 1;

	/// token data
	std::string data;

public:
	Token(){}
	Token(TokenKind k, int row, int col, std::string data) : kind(k), row(row), col(col), data(data) {}
	TokenKind get_kind() { return kind; }
	unsigned int get_row() { return row; }
	unsigned int get_col() { return col; }
	std::string get_data() { return data; }
	
	/// transfer to a symbol type
	Grammer::Symbol get_name() {
		using Grammer::Symbol;
		if (is(TokenKind::INT))
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
			return is(TokenKind::INT);
		}

		if (rhs.get_name() == "identifier") {
			return is(TokenKind::IDENTIFER);
		}

		return rhs.get_name() == data;
	}
	bool operator != (Grammer::Symbol rhs) {
		return !(*this == rhs);
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
		json.Add("type", "haha");
		json.Add("val", get_data());
		json.AddEmptySubObject("pos");
		json["pos"].Add("row", get_row());
		json["pos"].Add("col", get_col());
		return json.ToFormattedString();
	}
};
