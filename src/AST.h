#pragma once

#include <vector>

#include "Token.h"
#include "grammer/CFG.h"
#include "json/CJsonObject.hpp"

namespace AST {
	class node {
		Token name;
		Grammer::Symbol type;
		std::vector<node*> childs;
	public:
		node(Grammer::Symbol _type): type(_type) {}
		void set_name(Token _name) { name = _name; }
		void add_child(node* nd) { childs.push_back(nd); }

		std::string json_print() {
			neb::CJsonObject json;
			if (type == "num" || type == "identifier") {
				return name.get_data();
			}
			else if (childs.size()) {
				for (int i = childs.size() - 1; i >= 0; --i) {
					neb::CJsonObject j(childs[i]->json_print());
					if (childs[i]->childs.size()) json.Add(childs[i]->type.get_name() + ": ", j);
					else json.Add(childs[i]->type.get_name(), childs[i]->json_print());
				}
			}
			else return "";
			return json.ToFormattedString();
		}
	};
}
