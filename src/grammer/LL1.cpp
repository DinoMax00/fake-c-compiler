#include "LL1.h"

std::set<Symbol> LL1::get_first_set(Symbol sym) {
	std::set<Symbol> firstSet;

	/// rule1:
	/// for terminal symbol, 
	/// it's first set is itself
	if (sym.get_type() == SymbolType::TERMINAL) {
		
		firstSet.insert(sym);
		return firstSet;
	}

	/// rule2:
	/// for non terminal symbol,
	/// A->XYZ, 
	/// if X=>epsilon, then first(A).insert(first(X)), check Y
	/// if XYZ=>epsilon, then first(A).insert(epsilon)
	int n = productions.size();
	for (int i = 0; i < n; ++i) {
		if (productions[i]->get_header() != sym)
			continue;

		for (auto body: productions[i]->get_bodys()) {
			bool epsilon = true;

			for (auto it : body) {
				auto tmpSet = get_first_set(it);

				if (!tmpSet.count(std::string("¦Å"))) {
					epsilon = false;
				}

				/// merge
				firstSet.insert(tmpSet.begin(), tmpSet.end());

				if (!epsilon)
					break;
			}

			if (epsilon)
				firstSet.insert(std::string("¦Å"));
		}
	}

	return firstSet;
}

std::set<Symbol> LL1::get_follow_set(Symbol sym) {
	std::set<Symbol> followSet;

	/// rule1:
	/// if sym is the start symbol, inset '$' to the set
	if (sym == startSymbol) {
		followSet.insert(Symbol("$"));
	}

	for (auto pro : productions) {
		for (auto body : pro->get_bodys()) {
			for (int i = 0; i < (int)body.size(); ++i) {
				auto tmp = body[i];
				if (tmp != sym)
					continue;

				/// rule2:
				/// if sym is in the end of the body,
				/// then insert follow(head) to the set
				if (i == body.size() - 1) {
					/// avoid endless loop
					if (!followRecord[pro->get_header()]) {
						followRecord[pro->get_header()] = true;
						auto s = get_follow_set(pro->get_header());
						followSet.insert(s.begin(), s.end());
					}
					continue;
				}

				/// rule3:
				/// S->XYZ
				/// insert first(Z) into follow(Y)
				auto fSet = get_first_set(body[i + 1]);
				bool epsilon = fSet.count(Symbol("¦Å"));

				fSet.erase(Symbol("¦Å"));
				followSet.insert(fSet.begin(), fSet.end());

				if (epsilon) {
					followRecord[body[i + 1]] = true;
					fSet = get_follow_set(body[i + 1]);
					followSet.insert(fSet.begin(), fSet.end());
				}
			}
		}
	}
	return followSet;
}
