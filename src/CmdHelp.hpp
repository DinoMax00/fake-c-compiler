#pragma once

#include <iostream>
#include <iomanip>
#include <Windows.h>

#include "grammer/GrammerRules.h"
#include "Lexer.h"


void print_productions() {
	using std::cout;
	for (int i = 0; i < listLength; ++i) {
		if (rules[i][0] == END_SYMBOL)
			break;

		if (i && rules[i][0] == rules[i - 1][0]) {
			for (int j = 0; j < (int)(rules[i][0].length() + 4); ++j)
				putchar(' ');
			cout << "| ";
		}
		else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | BLUE);
			cout << std::endl << rules[i][0];
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
			cout << " -->  ";
		}
		for (int j = 1; j < tokenNum; ++j) {
			if (rules[i][j] == END_SYMBOL)
				break;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | YELLOW);
			cout << rules[i][j] << " ";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		cout << std::endl;
	}
}
int digit(int num)
{
	if (num == 0)
		return 1;
	int ans = 0;
	while (num > 0)
	{
		ans++;
		num = num / 10;
	}
	return ans;
}

const int width_action = 15;
void print_action(LR1* lr1) {
	using std::cout;
	std::vector<State> table = lr1->get_parser_table();
	for (int space = width_action; space > 0; space--)
		cout << " ";
	cout << "|";
	for (auto it : lr1->get_terminalSet())
	{
		cout << it;
		for (int space = width_action - it.get_name().size(); space > 0; space--)
			cout << " ";
		cout << "|";
	}
	cout << "#";
	for (int space = width_action - 1; space > 0; space--)
		cout << " ";
	cout << "|";
	cout << std::endl;
	for (int space = (width_action + 1) * (lr1->get_terminalSet().size() + 2); space > 0; space--)
		cout << "-";
	cout << std::endl;
	for (int i = 0; i < table.size(); i++)
	{
		cout << i;
		for (int space = width_action - digit(i); space > 0; space--)
			cout << " ";
		cout << "|";
		for (auto it : lr1->get_terminalSet())
		{
			if (table[i].type[it] == 1)
			{
				cout << "acc";
				for (int space = width_action - 3; space > 0; space--)
					cout << " ";
			}
			else if (table[i].type[it] == 2)
			{
				cout << "r" << table[i].edge[it];
				for (int space = width_action - 1 - digit(table[i].edge[it]); space > 0; space--)
					cout << " ";
			}
			else if (table[i].type[it] == 3)
			{
				cout << "s" << table[i].edge[it];
				for (int space = width_action - 1 - digit(table[i].edge[it]); space > 0; space--)
					cout << " ";
			}
			else
			{
				for (int space = width_action; space > 0; space--)
					cout << " ";
			}
			cout << "|";
		}
		auto end = Grammer::Symbol("#");
		if (table[i].type[end] == 1)
		{
			cout << "acc";
			for (int space = width_action - 3; space > 0; space--)
				cout << " ";
		}
		else if (table[i].type[end] == 2)
		{
			cout << "r" << table[i].edge[end];
			for (int space = width_action - 1 - digit(table[i].edge[end]); space > 0; space--)
				cout << " ";
		}
		else if (table[i].type[end] == 3)
		{
			cout << "s" << table[i].edge[end];
			for (int space = width_action - 1 - digit(table[i].edge[end]); space > 0; space--)
				cout << " ";
		}
		else
		{
			for (int space = width_action; space > 0; space--)
				cout << " ";
		}
		cout << "|";
		cout << std::endl;
		for (int space = (width_action + 1) * (lr1->get_terminalSet().size() + 2); space > 0; space--)
			cout << "-";
		cout << std::endl;
	}
}

const int width_goto = 30;
void print_goto(LR1* lr1) {
	using std::cout;
	std::vector<State> table = lr1->get_parser_table();
	for (int space = width_goto; space > 0; space--)
		cout << " ";
	cout << "|";
	for (auto it : lr1->get_nonTerminalSet())
	{
		cout << it;
		for (int space = width_goto - it.get_name().size(); space > 0; space--)
			cout << " ";
		cout << "|";
	}
	cout << std::endl;
	for (int space = (width_goto + 1) * (lr1->get_nonTerminalSet().size() + 1); space > 0; space--)
		cout << "-";
	cout << std::endl;
	for (int i = 0; i < table.size(); i++)
	{
		cout << i;
		for (int space = width_goto - digit(i); space > 0; space--)
			cout << " ";
		cout << "|";
		for (auto it : lr1->get_nonTerminalSet())
		{
			if (table[i].type[it] == 4)
			{
				cout << "GOTO" << table[i].edge[it];
				for (int space = width_goto - 4 - digit(table[i].edge[it]); space > 0; space--)
					cout << " ";
			}
			else
			{
				for (int space = width_goto; space > 0; space--)
					cout << " ";
			}
			cout << "|";
		}
		cout << std::endl;
		for (int space = (width_goto + 1) * (lr1->get_nonTerminalSet().size() + 1); space > 0; space--)
			cout << "-";
		cout << std::endl;
	}
}

void print_tokens(const char* filename, Lexer* lexer) {
	using std::cout;
	using std::endl;

	auto vec = lexer->get_token_list();
	for (auto& it : vec) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | BLUE);
		cout << std::setw(12) << std::setiosflags(std::ios::left) << it.get_tokenkind_name();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

		cout << std::setw(8) << std::setiosflags(std::ios::left) << std::string("id=") + std::to_string(it.get_id());

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | YELLOW);
		cout << std::setw(13) << std::setiosflags(std::ios::left) << std::string(" \"") + it.get_data() + "\"";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

		cout << " Loc=<" << filename << ":" << it.get_row() << ":" << it.get_col() << ">";
		cout << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	}
}

void print_tokens_json(const char* filename, Lexer* lexer) {
	using std::cout;
	using std::endl;

	auto vec = lexer->get_token_list();
	for (auto& it : vec) {
		cout << it.json_print() << endl;
	}
}
