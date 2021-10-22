#include <assert.h>
#include <map>

#include "CFG.h"
#include "GrammerRules.h"

namespace Grammer {

	CFG::CFG() {
		startSymbol = Symbol(rules[0][0]);

		for (int i = 0; i < listLength; ++i) {
			std::string* arr = rules[i];

			if (arr[0] == END_SYMBOL)
				break;

			shared_ptr<Production> pPtr = std::make_shared<Production>();
			ProductionBody pb;
			
			assert(pPtr);

			pPtr->set_header(Symbol(arr[0]));

			for (int j = 0; j < tokenNum; ++j) {
				assert(arr[j].length() > 0);
				if (arr[j] == END_SYMBOL)
					break;

				Symbol sym(arr[j]);

				if (sym.get_type() == SymbolType::TERMINAL)
					terminalSet.insert(sym);
				else
					nonTerminalSet.insert(sym);

				if (j)
					pb.push_back(sym);
				else
					pPtr->set_header(sym);
			}
			pPtr->append_body(pb);

			productions.push_back(pPtr);
		}
		merge_productions();
	}

	void CFG::merge_productions() {
		/// merge
		std::vector<shared_ptr<Production> > v;
		std::map<Symbol, bool> vis;

		for (int i = 0; i < (int)productions.size(); ++i) {
			auto& pro = *productions[i];
			auto lhs = pro.get_header();
			auto rhs = pro.get_bodys();

			if (vis[lhs])
				continue;
			vis[lhs] = true;

			for (int j = i + 1; j < (int)productions.size(); ++j) {
				auto pp = *productions[j];
				pro += pp;
			}

			v.push_back(productions[i]);
		}
		productions = v;
	}

	CFG& CFG::remove_recursive() {
		int n = productions.size();

		for (int i = 0; i < n; ++i) {
			/// remove indirect left recursion
			/// S->Ab, A->Sa
			for (int j = 0; j < i; ++j) {
				int k = 0;
				while (k < (int)productions[i]->get_bodys().size()) {
					auto& bodys = productions[i]->get_bodys();
					if (bodys[k][0] == productions[j]->get_header()) {
						
						auto tmp = bodys[k];
						bodys.erase(bodys.begin() + k);
						/// replace the first symbol
						for (int t = 0; t < (int)productions[j]->get_bodys().size(); ++t) {
							/// create new productionBody
							ProductionBody pb;
							pb.insert(pb.end(), productions[j]->get_bodys()[t].begin()
								, productions[j]->get_bodys()[t].end());
							pb.insert(pb.end(), tmp.begin() + 1, tmp.end());
							bodys.push_back(pb);
						}
						k = 0;
					}
					else
						++k;
				}
			}

			/// remove direct left recursion
			/// A -> Ax | Ay | a | b
			/// A -> aA' | bA', A' -> xA' | yA' | epsilon
			bool isLeftRecursive = false;
			for (int j = 0; j < (int)productions[i]->get_bodys().size(); ++j) {
				if (productions[i]->get_header() == productions[i]->get_bodys()[j][0]) {
					isLeftRecursive = true;
					break;
				}
			}
			
			if (!isLeftRecursive)
				continue;

			Symbol newHeader(productions[i]->get_header().get_name() + "`");
			for (int j = 0; j < (int)productions[i]->get_bodys().size();) {
				/// A -> Ax
				if (productions[i]->get_header() == productions[i]->get_bodys()[j][0]) {
					assert(productions[i]->get_bodys()[j].size() > 1);
					ProductionBody pb;

					pb.insert(pb.end(), productions[i]->get_bodys()[j].begin() + 1,
						productions[i]->get_bodys()[j].end());
					pb.push_back(newHeader);

					auto newProduction = shared_ptr<Production>(new Production(newHeader));
					newProduction->append_body(pb);
					productions.push_back(newProduction);

					auto it = productions[i]->get_bodys().erase(productions[i]->get_bodys().begin() + j);
					j = it - productions[i]->get_bodys().begin();

					nonTerminalSet.insert(newHeader);
				}
				else {
					if (productions[i]->get_bodys()[j][0].get_name() == "¦Å") {

					}
					else {
						productions[i]->get_bodys()[j].push_back(Symbol(newHeader));
					}
					++j;
				}
			}
			if (productions[i]->get_bodys().size() == 0) {
				productions.erase(productions.begin() + i--);
			}
			auto newProduction = shared_ptr<Production>(new Production(newHeader));
			ProductionBody pb;
			pb.push_back(Symbol("¦Å"));
			newProduction->append_body(pb);
			productions.push_back(newProduction);

			terminalSet.insert(Symbol("¦Å"));
		}
		merge_productions();
		return *this;
	}

	std::ostream& operator << (std::ostream& out, const CFG& cfg) {
		for (auto& it : cfg.productions) {
			out << it->get_header().get_name() << " -> ";

			for (auto& body : it->get_bodys()) {
				for (auto& sym : body) {
					out << sym.get_name() << " ";
				}
				out << "| ";
			}
			out << std::endl;
		}
		return out;
	}

	std::ostream& operator <<(std::ostream& out, Symbol sym) {
		out << sym.get_name();
		return out;
	}

	std::ostream& operator <<(std::ostream& out, Production sym) {
		out << sym.get_header().get_name() << " -> ";

		for (auto& body : sym.get_bodys()) {
			for (auto& sym : body) {
				out << sym.get_name() << " ";
			}
			out << "| ";
		}
		return out;
	}
}
