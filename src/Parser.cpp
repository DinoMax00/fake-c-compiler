#include <stack>

#include "Parser.h"

bool Parser::next() {
	++curToken;
	if (curToken == tokenList.end())
		return false;
	return true;
}

int Parser::build_ast_tree() {
	std::cout << "building tree..." << std::endl;

	std::stack<Grammer::Symbol> symbolStack;
	std::stack<int> stateStack;
	std::stack<AST::node*> nodeStack;

	/// init stacks
	symbolStack.push(Grammer::Symbol("#"));
	stateStack.push(0);

	while (!symbolStack.empty()) {
		int state_cur = stateStack.top();

		Grammer::Symbol sym_cur = curToken->get_name();
		int type_action = parserTable[state_cur].type[sym_cur];
		int edge_next = parserTable[state_cur].edge[sym_cur];
		//写法不一定鲁棒
		if (type_action == 1)
		{
			//成功
			sym_cur = parserTable[state_cur].state_exproductions[edge_next].get_header();
			auto node = new AST::node(sym_cur);
			for (int i = 0; i < (int)parserTable[state_cur].state_exproductions[edge_next].get_bodys()[0].size(); i++)
			{
				symbolStack.pop();
				stateStack.pop();
				auto child = nodeStack.top();
				node->add_child(child);
				nodeStack.pop();
			}
			root = node;
			return true;
		}
		else if (type_action == 2)
		{
			sym_cur = parserTable[state_cur].state_exproductions[edge_next].get_header();
			auto node = new AST::node(sym_cur);
			for (int i = 0; i < (int)parserTable[state_cur].state_exproductions[edge_next].get_bodys()[0].size(); i++)
			{
				symbolStack.pop();
				stateStack.pop();
				auto child = nodeStack.top();
				node->add_child(child);
				nodeStack.pop();
			}
			nodeStack.push(node);
			symbolStack.push(sym_cur);
			state_cur = stateStack.top();
			type_action = parserTable[state_cur].type[sym_cur];
			edge_next = parserTable[state_cur].edge[sym_cur];
			if (type_action == 4)
				stateStack.push(edge_next);
			else
			{
				std::cout << "ERROR" << std::endl;
				exit(1);
			}
		}
		else if (type_action == 3)
		{
			stateStack.push(edge_next);
			symbolStack.push(sym_cur);
			auto node = new AST::node(sym_cur);
			node->set_name(*curToken);
			nodeStack.push(node);
			next();
		}
		else
		{
			std::cout << sym_cur << " " << type_action << " " << "ERROR" << std::endl;
			exit(1);
		}
	}
	return false;
}


/*
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

*/