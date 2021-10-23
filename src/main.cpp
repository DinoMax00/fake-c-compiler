#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

#include "Lexer.h"
#include "grammer/LL1.h"
#include "Parser.h"

int main(int argc, char* argv[]) {
	std::ifstream in("..\\..\\src\\test.txt");
	if (!in.is_open())
	{
		std::cerr << "Error opening file"; 
		exit(1);
	}
	Lexer lex(in);
	lex.test();
	LL1 l;
	// l.test();
	Parser par(lex.get_token_list());
	par.build_ast_tree();
	std::cout << "success!" << std::endl;
	par.json_print();
	return 0;
}
