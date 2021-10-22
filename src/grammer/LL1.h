#pragma once

#include <map>

#include "CFG.h"

using namespace Grammer;

class LL1: Grammer::CFG {
	std::map<Symbol, bool> followRecord;
public:
	std::set<Symbol> get_first_set(Symbol sym);
	std::set<Symbol> get_follow_set(Symbol sym);

	void test() {
		startSymbol = Symbol("<Expr>");
		using std::cout;
		using std::endl;
		cout << remove_recursive() << endl;
		cout << "Terminal Set:" << endl;
		for (auto it : terminalSet) cout << it.get_name() << endl;
		cout << endl;
		cout << "nTerminal Set:" << endl;
		for (auto it : nonTerminalSet) cout << it.get_name() << endl;
		cout << endl;
		for (auto it : nonTerminalSet) {
			auto st = get_first_set(it);
			cout << "-->first" << it << endl;
			for (auto it : st) {
				cout << it << endl;
			}
		}
		cout << endl;
		for (auto it : nonTerminalSet) {
			followRecord.clear();
			auto st = get_follow_set(it);
			cout << "-->follow" << it << endl;
			for (auto it : st) {
				cout << it << endl;
			}
		}
	}
};
