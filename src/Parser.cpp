#include <stack>

#include "Parser.h"

bool Parser::next() {
	++curToken;
	if (curToken == tokenList.end())
		return false;
	return true;
}

int Parser::build_ast_tree() {
	std::stack<Symbol> symbolStack;
	std::stack<AST::node*> nodeStack;

	/// init stacks
	symbolStack.push(Symbol("#"));
	symbolStack.push(ll1.get_start_symbol());

	nodeStack.push(root);
	nodeStack.push(root);

	auto topSym = symbolStack.top();
	auto topNode = nodeStack.top();
	while (topSym != "#") {
		
		if (topSym.get_type() == SymbolType::TERMINAL) {
			topNode->set_name(*curToken);
			if (*curToken != topSym || !next()) {
				std::cerr << "parse error at row: " << curToken->get_row()
					<< " col: " << curToken->get_col() << ", " << curToken->get_data() << std::endl;
				exit(0);
			}
			// std::cout << "pop " << topSym << std::endl;
			symbolStack.pop();
			nodeStack.pop();
		}
		else {
			auto name = curToken->get_name();
			if (!parserTable[topSym].count(name)) {
				std::cerr << "parse error at row: " << curToken->get_row()
					<< " col: " << curToken->get_col() << ", " << curToken->get_data() << std::endl;
				exit(0);
			}

			// std::cout << "pop: " << topSym << std::endl;
			symbolStack.pop();
			nodeStack.pop();
			auto pro = parserTable[topSym][name].get_bodys()[0];
			for (int i = pro.size() - 1; i >= 0; --i) {
				// std::cout << "push: " << pro[i] << std::endl;
				symbolStack.push(pro[i]);

				auto node = new AST::node(pro[i]);
				topNode->add_child(node);
				nodeStack.push(node);
			}
		}

		topSym = symbolStack.top();
		topNode = nodeStack.top();
	}
	return true;
}
