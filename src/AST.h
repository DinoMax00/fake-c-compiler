#pragma once

#include <vector>
#include <Windows.h>

#include "Token.h"
#include "grammer/CFG.h"
#include "json/CJsonObject.hpp"

/// color print
constexpr int RED = FOREGROUND_RED;
constexpr int YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
constexpr int GREEN = FOREGROUND_GREEN;
constexpr int BLUE = FOREGROUND_BLUE;
constexpr int WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

namespace AST {
	class node {
		Token name;
		Grammer::Symbol type;
		std::vector<node*> childs;

		void color_print(std::string str, int COLOR) {
			using namespace std;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | COLOR);
			cout << str;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
	public:
		node(Grammer::Symbol _type): type(_type) {}
		void set_name(Token _name) { name = _name; }
		void add_child(node* nd) { childs.push_back(nd); }

		void print_space(std::vector<int>* deps, bool flg = false) {
			using std::cout;

			int cur = 0;
			for (int i = 1; cur < deps->size(); ++i) {
				if (i == (*deps)[cur]) {
					
					if (cur == deps->size() - 1) {
						if (flg) cout << "©¸©¤ ";
						else cout << "©À©¤ ";
					}
					else
						cout << "©¦  ";
					++cur;
				}
				else 
					cout << "   ";
			}
		}

		void print(int d = 0, std::vector<int>* deps = new std::vector<int>, bool flg = false) {
			using std::cout;
			using std::endl;
			print_space(deps, flg);

			if (childs.size()) {
				color_print(type.get_name(), GREEN);
				cout << endl;
			}
			else {
				color_print(name.get_tokenkind_name(), YELLOW);
				cout << ": " << name << endl;
			}
			
			if (flg && deps->size()) {
				deps->pop_back();
			}
			for (int i = childs.size() - 1; i >= 0; --i) {
				auto v = new std::vector<int>(*deps);
				v->push_back(d + 1);
				if (i == 0) 
					childs[i]->print(d + 1, v, true);
				else 
					childs[i]->print(d + 1, v);
			}
		}

		std::string json_print() {
			neb::CJsonObject json;
			if (childs.size()) {
				for (int i = childs.size() - 1; i >= 0; --i) {
					neb::CJsonObject j(childs[i]->json_print());
					if (childs[i]->childs.size()) json.Add(childs[i]->type.get_name(), j);
					else json.Add(childs[i]->name.get_tokenkind_name(), childs[i]->json_print());
				}
			}
			else 
				return name.get_data();
			return json.ToFormattedString();
		}
	};
}
