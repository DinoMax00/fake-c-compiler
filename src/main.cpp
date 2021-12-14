#define _CRT_SECURE_NO_WARNINGS

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
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--table" << "-show the LR(1) action & goto table." << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--tokens" << "-show the tokens after lexer" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--tokens-json" << "-show the tokens in json-format after lexer" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--ast" << "-show the ast-tree after parser" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--ast-json" << "-show the ast-tree in json-format after parser" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--ast-viz" << "-show the ast-tree visulization" << std::endl;
		std::cout << std::setw(15) << std::setiosflags(std::ios::left) << "--midcode" << "-generate the midcode" << std::endl;

	}
	else if (argc == 2) {
		auto opt = argv[1];
		if (strcmp(opt, "--grammer") == 0) {
			print_productions();
			return 0;
		}
		else if (strcmp(opt, "--table") == 0) {
			LR1 lr1;
			freopen("action.txt", "w", stdout);
			print_action(&lr1);

			fclose(stdout);
			freopen("goto.txt", "w", stdout);
			print_goto(&lr1);
			fclose(stdout);
			freopen("CON", "w", stdout);
			std::cout << "Done! See action.txt & goto.txt" << std::endl;
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
			std::cout << "Done! See ast.txt" << std::endl;
			freopen("ast.txt", "w", stdout);
			par.print();
			return 0;
		}

		if (strcmp(opt, "--ast-json") == 0) {
			std::cout << "Done! See ast-json.txt" << std::endl;
			freopen("ast-json.txt", "w", stdout);
			std::cout << par.json_print() << std::endl;
			return 0;
		}

		if (strcmp(opt, "--ast-viz") == 0) {
			std::cout << "Loading..." << std::endl;
			std::ofstream out(".\\ast-win32-x64\\resources\\app\\ast.json");
			if (out.is_open())
			{
				out << par.json_print();
				out.close();
			}
			system("start .\\ast-win32-x64\\ast.exe");
			std::cout << "Done!" << std::endl;
			return 0;
		}

		if (strcmp(opt, "--midcode") == 0) {
			std::cout << "semantic analyzing..." << std::endl;
			Semantic sem;
			sem.analyze(par.get_root());
			freopen("midcode.txt", "w", stdout);
			sem.print();
			fclose(stdout);
			freopen("CON", "w", stdout);
			sem.print();
			std::cout << "Done! See midcode.txt" << std::endl;
		}
	}

	return 0;
}
