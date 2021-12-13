#include <vector>
#include <set>
#include <utility>
#include <string>
#include <tuple>
#include "AST.h"


void Semantic::analyze(AST::node* cur)
{
	auto header = cur->type;
	if (header == "<Program>")
		//"<Program>", "<Declare_String>"
	{
		for (auto i : cur->childs)
			analyze(i);
	}
	else if (header == "<Declare_String>")
	{
		if (cur->childs.size() == 1)
			//"<Declare_String>", "<Declare>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
		else if (cur->childs.size() == 2)
			//"<Declare_String>", "<Declare>", "<Declare_String>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
	}
	else if (header == "<Declare>")
	{
		if (cur->childs[0]->type == "<Variable_Declare>")
			//"<Declare>", "int","identifier", "<Variable_Declare>"
		{
			for (auto i : cur->childs)
				analyze(i);
			vSymbol vs;
			vs.name = cur->childs[1]->name.get_data();
			vs.type = cur->childs[0]->name.get_kind();	//可能需改
			vs.size = 4;
			vs.offset = cur_offset;
			cur_offset += vs.size;	//作用?
			vs.pos = getNewTemp();
			vs.func = "";
			updateSymbolTable(vs);
		}
		else if (cur->childs[0]->type == "<Func_Declare>")
			//"<Declare>", "int/void","identifier", "<Func_Declare>"
		{
			fSymbol fs;
			fs.name = cur->childs[1]->name.get_data();
			fs.type = cur->childs[0]->name.get_kind();
			if (fs.name == "main")
				fs.entry = 0;
			else
				fs.entry = getNewFuncLabel();
			if (fs.name != "main")
			{
				std::string func_entry = std::string("L") + std::to_string(fs.entry);
				midcode_set.push_back(midcode(func_entry, ":", "", ""));
			}
			para_table.clear();
			this->cur_function.name = fs.name;
			for (auto i : cur->childs)
				analyze(i);
			for (auto i : para_table)
				fs.paras.push_back(i);
			updateFuncTable(fs);
			para_table.clear();
			this->cur_function.name = "";
		}
	}
	else if (header == "<Variable_Declare>")
		//"<Variable_Declare>", ";"
	{
		for (auto i : cur->childs)
			analyze(i);
	}
	else if (header == "<Func_Declare>")
		//"<Func_Declare>",  "(","<Formal_Para>", ")", "<Statement_Blocks>"
	{
		for (auto i : cur->childs)
			analyze(i);
	}
	else if (header == "<Formal_Para>")
	{
		if (cur->childs.size() == 1 && cur->childs[0]->type == "<Para_List>")
			//"<Formal_Para>",  "<Para_List>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
		else if (cur->childs.size() == 1 && cur->childs[0]->type == "void")
			//"<Formal_Para>",  "void"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
	}
	else if (header == "<Para_List>")
	{
		if (cur->childs.size() == 1)
			//"<Para_List>",  "<Para>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
		else if (cur->childs.size() == 3)
			//"<Para_List>",  "<Para>",",","<Para_List>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
	}
	else if (header == "<Para>")
		//"<Para>",  "int","<ID>"
	{
		for (auto i : cur->childs)
			analyze(i);
		TokenKind type = cur->childs[0]->name.get_kind();
		std::string name = cur->childs[1]->name.get_data();
		para_table.push_back(para(type, name));
	}
	else if (header == "<Statement_Blocks>")
	{
		if (cur->childs.size() == 3)
			//"<Statement_Blocks>", "{","<Statement_String>","}"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
		else if (cur->childs.size() == 4)
			//"<Statement_Blocks>", "{", "<Inner_Declare>",  "<Statement_String>","}"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
	}
	else if (header == "<Inner_Declare>")
	{
		if (cur->childs.size() == 1)
			//"<Inner_Declare>", "<Inner_Variable_Declare>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
		else if (cur->childs.size() == 2)
			//"<Inner_Declare>", "<Inner_Variable_Declare>", "<Inner_Declare>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
	}
	else if (header == "<Inner_Variable_Declare>")
		//"<Inner_Variable_Declare>", "int", "<ID>",";"
	{
		for (auto i : cur->childs)
			analyze(i);
		vSymbol vs;
		vs.name = cur->childs[1]->name.get_data();
		vs.type = cur->childs[0]->name.get_kind();	//可能需改
		vs.size = 4;
		vs.offset = cur_offset;
		cur_offset += vs.size;
		vs.pos = getNewTemp();
		vs.func = this->cur_function.name;
		updateSymbolTable(vs);
	}
	else if (header == "<Statement_String>")
	{
		if (cur->childs.size() == 1)
			//"<Statement_String>", "<Statement>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
		else if (cur->childs.size() == 2)
			//"<Statement_String>", "<Statement>", "<Statement_String>"
		{
			for (auto i : cur->childs)
				analyze(i);
		}
	}
	else if (header == "<Statement>")
		//"<Statement>", "<Statement_xx>"
	{
		for (auto i : cur->childs)
			analyze(i);
	}
	else if (header == "<Statement_Assignment>")
		//"<Statement_Assignment>", "<ID>", "=", "<Expr>", ";"
	{
		for (auto i : cur->childs)
			analyze(i);
		std::string arg1;
		if (cur->childs[2]->data)
			arg1 = std::to_string(cur->childs[2]->data);
		else
			arg1 = "T" + std::to_string(cur->childs[2]->pos);
		std::string result;
		vSymbol* var = findSymbol(cur->childs[0]->name.get_data(), this->cur_function.name);
		if (var)
			result = "T" + std::to_string(var->pos);
		midcode_set.push_back(midcode(":=", arg1, "", result));
	}
	else if (header == "<Statement_Return>")
	{
		//"<Statement_Return>", "return",";"
		if (cur->childs.size() == 3)
			//"<Statement_Return>", "return", "<Expr>", ";"
		{
			for (auto i : cur->childs)
				analyze(i);
			std::string value_return;
			if (cur->childs[1]->pos)
				value_return = std::string("T") + std::to_string(cur->childs[1]->pos);
			else if (cur->childs[1]->data)
				value_return = std::to_string(cur->childs[1]->data);
			midcode_set.push_back(midcode(":=", value_return, "", "EAX"));
			//假设通过EAX寄存器返回值
		}
		midcode_set.push_back(midcode("return", "", "", ""));
	}
	else if (header == "<Statement_While>")
		//"<Statement_While>", "while", "(","<Expr>",  ")", "<Statement_Blocks>"
	{
		analyze(cur->childs[2]);
		std::string condition = std::string("T") + std::to_string(cur->childs[2]->pos);
		std::string jump_begin = std::string("L") + std::to_string(getNewLabel());
		std::string jump_true = std::string("L") + std::to_string(getNewLabel());
		std::string jump_false = std::string("L") + std::to_string(getNewLabel());
		midcode_set.push_back(midcode(jump_begin, ":", "", ""));
		midcode_set.push_back(midcode("jnz", condition, "", jump_true));
		midcode_set.push_back(midcode("j", "", "", jump_false));
		midcode_set.push_back(midcode(jump_true, ":", "", ""));
		analyze(cur->childs[4]);
		midcode_set.push_back(midcode("j", "", "", jump_begin));
		midcode_set.push_back(midcode(jump_false, ":", "", ""));
	}
	else if (header == "<Statement_If>")
	{
		//"<Statement_If>", "if", "(","<Expr>",  ")", "<Statement_Blocks>"
		std::string condition = std::string("T") + std::to_string(cur->childs[2]->pos);
		std::string jump_true = std::string("L") + std::to_string(getNewLabel());
		std::string jump_false = std::string("L") + std::to_string(getNewLabel());
		analyze(cur->childs[2]);
		midcode_set.push_back(midcode("jnz", condition, "", jump_true));
		midcode_set.push_back(midcode("j", "", "", jump_false));
		midcode_set.push_back(midcode(jump_true, ":", "", ""));
		analyze(cur->childs[4]);
		midcode_set.push_back(midcode(jump_false, ":", "", ""));
		if (cur->childs.size() == 7)
			//"<Statement_If>", "if", "(","<Expr>",  ")", "<Statement_Blocks>","else", "<Statement_Blocks>"
		{
			analyze(cur->childs[6]);
		}
	}
	else if (header == "<Expr>")
	{
		if (cur->childs.size() == 1)
			//"<Expr>", "<Expr_Plus>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = cur->childs[0]->pos;
			cur->data = cur->childs[0]->data;
		}
		else if (cur->childs.size() == 3)
			//"<Expr>", "<Expr_Plus>", operator ,"<Expr>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = getNewTemp();
			std::string op = cur->childs[1]->name.get_data();
			std::string arg1, arg2;
			if (cur->childs[0]->pos)
				arg1 = std::string("T") + std::to_string(cur->childs[0]->pos);
			else
				arg1 = std::to_string(cur->childs[0]->data);
			if (cur->childs[2]->pos)
				arg2 = std::string("T") + std::to_string(cur->childs[2]->pos);
			else
				arg2 = std::to_string(cur->childs[2]->data);
			std::string result = std::string("T") + std::to_string(cur->pos);
			midcode_set.push_back(midcode(op, arg1, arg2, result));
		}
	}
	else if (header == "<Expr_Plus>")
	{
		if (cur->childs.size() == 1)
			//"<Expr_Plus>", "<Term>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = cur->childs[0]->pos;
			cur->data = cur->childs[0]->data;
		}
		else if (cur->childs.size() == 3)
			//"<Expr_Plus>", "<Term>",operator,"<Expr_Plus>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = getNewTemp();
			std::string op = cur->childs[1]->name.get_data();
			std::string arg1, arg2;
			if (cur->childs[0]->pos)
				arg1 = std::string("T") + std::to_string(cur->childs[0]->pos);
			else
				arg1 = std::to_string(cur->childs[0]->data);
			if (cur->childs[2]->pos)
				arg2 = std::string("T") + std::to_string(cur->childs[2]->pos);
			else
				arg2 = std::to_string(cur->childs[2]->data);
			std::string result = std::string("T") + std::to_string(cur->pos);
			midcode_set.push_back(midcode(op, arg1, arg2, result));
		}
	}
	else if (header == "<Term>")
	{
		if (cur->childs.size() == 1)
			//"<Term>", "<Factor>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = cur->childs[0]->pos;
			cur->data = cur->childs[0]->data;
		}
		else if (cur->childs.size() == 3)
			//"<Term>", "<Factor>", operator ,"<Term>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = getNewTemp();
			std::string op = cur->childs[1]->name.get_data();
			std::string arg1, arg2;
			if (cur->childs[0]->pos)
				arg1 = std::string("T") + std::to_string(cur->childs[0]->pos);
			else
				arg1 = std::to_string(cur->childs[0]->data);
			if (cur->childs[2]->pos)
				arg2 = std::string("T") + std::to_string(cur->childs[2]->pos);
			else
				arg2 = std::to_string(cur->childs[2]->data);
			std::string result = std::string("T") + std::to_string(cur->pos);
			midcode_set.push_back(midcode(op, arg1, arg2, result));
		}
	}
	else if (header == "<Factor>")
	{
		if (cur->childs.size() == 1 && cur->childs[0]->type == "num")
			//"<Factor>", "num"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = 0;
			cur->data = std::stoi(cur->childs[0]->name.get_data());
		}
		else if (cur->childs.size() == 1 && cur->childs[0]->type == "<ID>")
			//"<Factor>", "<ID>"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = cur->childs[0]->pos;
			cur->data = cur->childs[0]->data;
		}
		else if (cur->childs.size() == 3)
			//"<Factor>", "(","<Expr>",")"
		{
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = cur->childs[1]->pos;
			cur->data = cur->childs[1]->data;
		}
		else if (cur->childs.size() == 4)
			//"<Factor>", "<ID>","(", "<acPara_List>", ")"
		{
			acpara_table.clear();
			for (auto i : cur->childs)
				analyze(i);
			for (auto i : acpara_table)
			{
				std::string p = std::get<2>(i);
				midcode_set.push_back(midcode("Push", p, "", ""));
			}
			acpara_table.clear();
		}
	}
	else if (header == "<acPara_List>")
	{
		if (cur->childs.size() == 1)
			//"<acPara_List>", "<Expr>"
		{
			for (auto i : cur->childs)
				analyze(i);
			TokenKind type = cur->childs[0]->datatype;
			std::string name = "";
			std::string arg;
			if (cur->childs[0]->data)
				arg = std::to_string(cur->childs[0]->data);
			else
				arg = "T" + std::to_string(cur->childs[0]->pos);
			acpara_table.push_back(acpara(type, name, arg));
		}
		else if (cur->childs.size() == 3)
			//"<acPara_List>", "<Expr>",",","<acPara_List>"
		{
			for (auto i : cur->childs)
				analyze(i);
			TokenKind type = cur->childs[0]->datatype;
			std::string name = "";
			std::string arg;
			if (cur->childs[0]->data)
				arg = std::to_string(cur->childs[0]->data);
			else
				arg = "T" + std::to_string(cur->childs[0]->pos);
			acpara_table.push_back(acpara(type, name, arg));
		}
	}
	else if (header == "<ID>")
		//"<ID>", "identifier"
	{
		//不能在这里查变量，因为id有2种用法
		for (auto i : cur->childs)
			analyze(i);
	}
}