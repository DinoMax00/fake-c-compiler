#pragma once

#include <string>
#include <vector>

#define END_SYMBOL "##"
constexpr int listLength = 100;
constexpr int tokenNum = 20;

std::string rules[listLength][tokenNum] = {
	//{
	//	"<Program>", "<Type>", "<ID>", "(", ")", "<Statement>", END_SYMBOL
	//},
	//{
	//	"<Type>", "int", END_SYMBOL
	//},
	//{
	//	"<Type>", "void", END_SYMBOL
	//},
	{
		"<Expr>", "-", "<Expr>", END_SYMBOL
	},
	{
		"<Expr>", "(", "<Expr>", ")", END_SYMBOL
	},
	{
		"<Expr>", "<Var>", "<ExprTail>", END_SYMBOL
	},
	{
		"<ExprTail>", "-", "<Expr>", END_SYMBOL
	},
	{
		"<ExprTail>", "¦Å", END_SYMBOL
	},
	{
		"<Var>", "id", "<VarTail>", END_SYMBOL
	},
	{
		"<VarTail>", "(", "<Expr>", ")", END_SYMBOL
	},
	{
		"<VarTail>", "¦Å", END_SYMBOL
	},
	{
		END_SYMBOL
	},
};

