#pragma once
#include "TokenKinds.h"
#include <json/json.h>


class Token {
	/// token type
	TokenKind kind;

	/// Location of the token
	unsigned int row, col;

	/// token data
	std::string data;

public:
	Token(TokenKind k, int row, int col, std::string data) : kind(k), row(row), col(col), data(data) {}
	TokenKind get_kind() { return kind; }
	unsigned int get_row() { return row; }
	unsigned int get_col() { return col; }
	std::string get_data() { return data; }
	void setKind(TokenKind k) { kind = k; }

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
		Json::StreamWriterBuilder writerBuilder;
		std::ostringstream os;

		Json::Value rootValue = Json::objectValue;
		rootValue["type"] = "Identifer";
		rootValue["val"] = get_data();
		rootValue["position"] = Json::objectValue;
		rootValue["position"]["col"] = get_col();
		rootValue["position"]["row"] = get_row();

		std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
		jsonWriter->write(rootValue, &os);

		return os.str();
	}
};
