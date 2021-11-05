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
		std::string get_name() { return type.get_name(); }
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

		neb::CJsonObject json_print() {
			neb::CJsonObject js;

			if (childs.size()) {
				js.Add("name", type.get_name());
				js.AddEmptySubArray("children");
				for (int i = childs.size() - 1; i >= 0; --i) {
					js["children"].Add(childs[i]->json_print());
				}
			}
			else {
				js.Add("name", name.get_tokenkind_name());
				js.Add("value", name.get_data());
				js.Add("loc", "<" + std::to_string(name.get_row()) + "," + std::to_string(name.get_col()) + ">");
			}

			return js;
		}
	};
}
