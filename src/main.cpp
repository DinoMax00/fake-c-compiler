#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

#include "Lexer.h"
#include "grammer/CFG.h"
#include "grammer/LL1.h"

int main(int argc, char* argv[]) {
	std::ifstream in("..\\..\\src\\test.txt");
	if (!in.is_open())
	{
		std::cerr << "Error opening file"; 
		exit(1);
	}
	Lexer lex(in);
	LL1 l;
	l.test();
	return 0;
}
