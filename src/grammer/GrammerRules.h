#pragma once

#include <string>
#include <vector>

#define END_SYMBOL "##"
constexpr int listLength = 100;
constexpr int tokenNum = 20;

static std::string rules[listLength][tokenNum] = {
	{
		"<Program>", "<Type>", "<ID>", "(", ")", "<Statement_Blocks>", END_SYMBOL
	},
	{
		"<Type>", "int", END_SYMBOL
	},
	{
		"<Type>", "void", END_SYMBOL
	},
	{
		"<ID>", "identifier", END_SYMBOL
	},
	{
		"<Statement_Blocks>", "{", "<Inner_Declare>", "}", END_SYMBOL
	},
	{
		"<Inner_Declare>", "int", "<ID>", ";", END_SYMBOL
	},
	//{
	//	"<Expr>", "-", "<Expr>", END_SYMBOL
	//},
	//{
	//	"<Expr>", "(", "<Expr>", ")", END_SYMBOL
	//},
	//{
	//	"<Expr>", "<Var>", "<ExprTail>", END_SYMBOL
	//},
	//{
	//	"<ExprTail>", "-", "<Expr>", END_SYMBOL
	//},
	//{
	//	"<ExprTail>", "¦Å", END_SYMBOL
	//},
	//{
	//	"<Var>", "id", "<VarTail>", END_SYMBOL
	//},
	//{
	//	"<VarTail>", "(", "<Expr>", ")", END_SYMBOL
	//},
	//{
	//	"<VarTail>", "¦Å", END_SYMBOL
	//},
	{
		END_SYMBOL
	},
};

