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
				std::cout << sym_cur << " " << "row:" << (*curToken).get_row() << " " << "col:"<< (*curToken).get_col() << " " << "ERROR" << std::endl;
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
			std::cout << sym_cur << " " << "row:" << (*curToken).get_row() << " " << "col:" << (*curToken).get_col() << " " << "ERROR" << std::endl;
			exit(1);
		}
	}
	return false;
}