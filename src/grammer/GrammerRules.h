#pragma once

#include <string>
#include <vector>

#define END_SYMBOL "##"
constexpr int listLength = 100;
constexpr int tokenNum = 20;

static std::string rules[listLength][tokenNum] = {
	/*
	{
		"<Program>", "<Declare_String>", END_SYMBOL
	},
	{
		"<Declare_String>", "<Declare>", END_SYMBOL
	},
	{
		"<Declare_String>", "<Declare>", "<Declare_String>", END_SYMBOL
	},
	{
		"<Declare>", "int","identifier", "<Declare_Type>", END_SYMBOL
	},
	{
		"<Declare>", "void","identifier", "<Func_Declare>", END_SYMBOL
	},
	{
		"<Declare_Type>", "<Variable_Declare>", END_SYMBOL
	},
	{
		"<Declare_Type>", "<Func_Declare>", END_SYMBOL
	},
	{
		"<Variable_Declare>", ";", END_SYMBOL
	},
	{
		"<Func_Declare>",  "(","<Formal_Para>", ")", "<Statement_Blocks>", END_SYMBOL
	},
	{
		"<Formal_Para>",  "<Para_List>", END_SYMBOL
	},
	{
		"<Formal_Para>",  "void", END_SYMBOL
	},
	{
		"<Para_List>",  "<Para>", END_SYMBOL
	},
	{
		"<Para_List>",  "<Para>",",","<Para_List>", END_SYMBOL
	},
	{
		"<Para>",  "int","<ID>", END_SYMBOL
	},
	{
		"<Statement_Blocks>", "{", "<Inner_Declare>",  "<Statement_String>","}", END_SYMBOL
	},
	{
		"<Statement_Blocks>", "{","<Statement_String>","}", END_SYMBOL
	},
	{
		"<Inner_Declare>", "<Inner_Variable_Declare>",END_SYMBOL
	},
	{
		"<Inner_Declare>", "<Inner_Variable_Declare>", "<Inner_Declare>",END_SYMBOL
	},
	{
		"<Inner_Variable_Declare>", "int", "<ID>",";", END_SYMBOL
	},
	{
		"<Statement_String>", "<Statement>", END_SYMBOL
	},
	{
		"<Statement_String>", "<Statement>", "<Statement_String>", END_SYMBOL
	},
	{
		"<Statement>", "<Statement_If>", END_SYMBOL
	},
	{
		"<Statement>", "<Statement_While>", END_SYMBOL
	},
	{
		"<Statement>", "<Statement_Return>", END_SYMBOL
	},
	{
		"<Statement>", "<Statement_Assignment>", END_SYMBOL
	},
	{
		"<Statement_Assignment>", "<ID>", "=", "<Expr>", ";", END_SYMBOL
	},
	{
		"<Statement_Return>", "return",";", END_SYMBOL
	},
	{
		"<Statement_Return>", "return", "<Expr>", ";",END_SYMBOL
	},
	{
		"<Statement_While>", "while", "(","<Expr>",  ")", "<Statement_Blocks>",END_SYMBOL
	},
	{
		"<Statement_If>", "if", "(","<Expr>",  ")", "<Statement_Blocks>",END_SYMBOL
	},
	{
		"<Statement_If>", "if", "(","<Expr>",  ")", "<Statement_Blocks>","else", "<Statement_Blocks>",END_SYMBOL
	},

	{
		"<Expr>", "<Expr_Plus>",END_SYMBOL
	},
	{
		"<Expr>", "<Expr_Plus>","<","<Expr>",END_SYMBOL
	},
	{
		"<Expr>", "<Expr_Plus>","<=","<Expr>",END_SYMBOL
	},
	{
		"<Expr>", "<Expr_Plus>",">","<Expr>",END_SYMBOL
	},
	{
		"<Expr>", "<Expr_Plus>",">=","<Expr>",END_SYMBOL
	},
	{
		"<Expr>", "<Expr_Plus>","==","<Expr>",END_SYMBOL
	},
	{
		"<Expr>", "<Expr_Plus>","!=","<Expr>",END_SYMBOL
	},
	{
		"<Expr_Plus>", "<Term>",END_SYMBOL
	},
	{
		"<Expr_Plus>", "<Term>","+","<Expr_Plus>",END_SYMBOL
	},
	{
		"<Expr_Plus>", "<Term>","-","<Expr_Plus>",END_SYMBOL
	},
	{
		"<Term>", "<Factor>",END_SYMBOL
	},
	{
		"<Term>", "<Factor>","*","<Term>",END_SYMBOL
	},
	{
		"<Term>", "<Factor>","/","<Term>",END_SYMBOL
	},
	{
		"<Factor>", "num",END_SYMBOL
	},
	{
		"<Factor>", "(","<Expr>",")",END_SYMBOL
	},
	{
		"<Factor>", "<ID>",END_SYMBOL
	},
	{
		"<Factor>", "<ID>","(", "<acPara_List>", ")", END_SYMBOL
	},
	{
		"<acPara_List>", "<Expr>", END_SYMBOL
	},
	{
		"<acPara_List>", "<Expr>",",","<acPara_List>", END_SYMBOL
	},
	{
		"<ID>", "identifier", END_SYMBOL
	},
	*/
	//{
	//"<Program>", "<Type>", "<ID>", "(", ")", "<Statement_Blocks>", END_SYMBOL
	//},
	//{
	//	"<Type>", "int", END_SYMBOL
	//},
	//{
	//	"<Type>", "void", END_SYMBOL
	//},
	//{
	//	"<ID>", "identifier", END_SYMBOL
	//},
	//{
	//	"<Statement_Blocks>", "{", "<Inner_Declare>", "}", END_SYMBOL
	//},
	//{
	//	"<Inner_Declare>", "int", "<ID>", ";", END_SYMBOL
	//},
	{
		"<program>", "<declarationChain>", END_SYMBOL
	},
	{
		"<declarationChain>", "$", END_SYMBOL
	},
	{
		"<declarationChain>", "<declaration>", "<declarationChain>", END_SYMBOL
	},
	{
		"<typeSpecifier>", "int", END_SYMBOL
	},
	{
		"<typeSpecifier>", "void", END_SYMBOL
	},
	{
		"<declaration>", "<typeSpecifier>", "id", ";", END_SYMBOL
	},
	{
		"<declaration>", "<completeFunction>", END_SYMBOL
	},
	{
		"<completeFunction>", "<declareFunction>", "<block>", END_SYMBOL
	},
	{
		"<declareFunction>", "<typeSpecifier>", "id", "(", "<formalParaList>", ")", END_SYMBOL
	},
	{
		"<formalParaList>", "$", END_SYMBOL
	}, 
	{
		"<block>", "{", "<statementChain>", "}", END_SYMBOL
	},
	{
		"<statementChain>", "$", END_SYMBOL
	},
	{
		"<statementChain>", "<returnStatement>", END_SYMBOL
	},
	{
		"<returnStatement>", "return", ";", END_SYMBOL
	},
	{
		END_SYMBOL
	},
};

