#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>


#include "grammer/LR1.h"
#include "Parser.h"
#include "CmdHelp.hpp"
#include "Lexer.h"

int main(int argc, char* argv[]) {

	if (argc == 1) {
		std::cout << "OPTIONS:\n" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--grammer" << "-show the productions we use." << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--action" << "-show the LR(1) action table." << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--goto" << "-show the LR(1) goto table." << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--tokens" << "-show the tokens after lexer" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--tokens-json" << "-show the tokens in json-format after lexer" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--ast" << "-show the ast-tree after parser" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--ast-json" << "-show the ast-tree in json-format after parser" << std::endl;

	}
	else if (argc == 2) {
		auto opt = argv[1];
		if (strcmp(opt, "--grammer") == 0) {
			print_productions();
			return 0;
		}
		else if (strcmp(opt, "--action") == 0) {
			return 0;
		}
		else if (strcmp(opt, "--goto") == 0) {
			return 0;
		}
	}
	else if (argc == 3) {
		auto opt = argv[1];
		auto path = argv[2];

		std::ifstream in(path);
		if (!in.is_open())
		{
			std::cerr << "Error opening file";
			exit(1);
		}

		Lexer lex(in);
		if (strcmp(opt, "--tokens") == 0) {
			print_tokens(path, &lex);
			return 0;
		}

		if (strcmp(opt, "--tokens-json") == 0) {
			print_tokens_json(path, &lex);
			return 0;
		}

		Parser par(lex.get_token_list());
		par.build_ast_tree();
		
		if (strcmp(opt, "--ast") == 0) {
			par.print();
			return 0;
		}

		if (strcmp(opt, "--ast-json") == 0) {
			par.json_print();
			return 0;
		}
	}
	
	return 0;
}
