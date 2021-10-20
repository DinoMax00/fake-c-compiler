#include <iostream>
#include <fstream>

#include "Lexer.h"
int main(int argc, char* argv[]) {
	std::ifstream in("..\\..\\src\\test.txt");
	if (!in.is_open())
	{
		std::cerr << "Error opening file"; 
		exit(1);
	}
	Lexer lex(in);
	lex.test();
	return 0;
}
