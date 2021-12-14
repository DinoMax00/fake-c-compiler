#include <vector>
#include <set>
#include <utility>
#include <string>
#include <tuple>
#include "AST.h"

#include <iostream>


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
		if (cur->childs.size() == 3)
			//"<Declare>", "int","identifier", ";"
		{
			for (auto i : cur->childs)
				analyze(i);
			vSymbol vs;
			vs.name = cur->childs[1]->name.get_data();
			vs.type = cur->childs[0]->name.get_kind();	//�������
			vs.size = 4;
			vs.offset = cur_offset;
			cur_offset += vs.size;	//����?
			vs.pos = getNewTemp();
			vs.func = "";
			if (!updateSymbolTable(vs))
			{
				std::cout << "ERROR: Variable " << vs.name << " Redefinition" << std::endl;
				exit(1);
			}
		}
		else if (cur->childs.size() == 6)
			//"<Declare>", "int/void","identifier","(",  "<Formal_Para>", ")", "<Statement_Blocks>"
		{
			para_table.clear();
			fSymbol fs;
			fs.name = cur->childs[1]->name.get_data();
			fs.type = cur->childs[0]->name.get_kind();
			if (fs.name == "main")
				fs.entry = 0;
			else
				fs.entry = getNewFuncLabel();
			if (fs.name != "main")
			{
				std::string func_entry = std::string("F") + std::to_string(fs.entry);
				midcode_set.push_back(midcode(func_entry + ":", "", "", ""));
			}
			else
			{
				midcode_set.push_back(midcode("main:", "", "", ""));
			}
			this->cur_function.name = fs.name;
			analyze(cur->childs[3]);
			for (auto i : para_table)
			{
				fs.paras.push_back(i);
				vSymbol vs;
				vs.name = i.second;
				vs.type = i.first;	//�������
				vs.size = 4;
				vs.offset = cur_offset;
				cur_offset += vs.size;	//����?
				vs.pos = getNewTemp();
				vs.func = this->cur_function.name;
				if (!updateSymbolTable(vs))
				{
					std::cout << "ERROR: Variable " << vs.name << " Redefinition" << std::endl;
					exit(1);
				}
				midcode_set.push_back(midcode("POP", "", "", "T" + std::to_string(vs.pos)));
			}
			if (!updateFuncTable(fs))
			{
				std::cout << "ERROR: Function " << fs.name << " Redefinition" << std::endl;
				exit(1);
			}
			para_table.clear();
			analyze(cur->childs[5]);
			this->cur_function.name = "";
		}
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
		std::string name = cur->childs[1]->childs[0]->name.get_data();
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
		vs.name = cur->childs[1]->childs[0]->name.get_data();
		vs.type = cur->childs[0]->name.get_kind();	//�������
		vs.size = 4;
		vs.offset = cur_offset;
		cur_offset += vs.size;
		vs.pos = getNewTemp();
		vs.func = this->cur_function.name;
		if (!updateSymbolTable(vs))
		{
			std::cout << "ERROR: Variable " << vs.name << " Redefinition" << std::endl;
			exit(1);
		}
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
		if (cur->childs[2]->pos)
			arg1 = "T" + std::to_string(cur->childs[2]->pos);
		else
			arg1 = std::to_string(cur->childs[2]->data);
		std::string result;
		vSymbol* var = findSymbol(cur->childs[0]->childs[0]->name.get_data(), this->cur_function.name);
		if (var != nullptr)
		{
			result = "T" + std::to_string(var->pos);
		}
		else
		{
			std::cout << "ERROR: Variable " << cur->childs[0]->childs[0]->name.get_data() << " Non-existent" << std::endl;
			exit(1);
		}
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
			//����ͨ��EAX�Ĵ�������ֵ
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
		midcode_set.push_back(midcode(jump_begin + ":", "", "", ""));
		midcode_set.push_back(midcode("jnz", condition, "", jump_true));
		midcode_set.push_back(midcode("j", "", "", jump_false));
		midcode_set.push_back(midcode(jump_true + ":", "", "", ""));
		analyze(cur->childs[4]);
		midcode_set.push_back(midcode("j", "", "", jump_begin));
		midcode_set.push_back(midcode(jump_false + ":", "", "", ""));
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
		midcode_set.push_back(midcode(jump_true + ":", "", "", ""));
		analyze(cur->childs[4]);
		midcode_set.push_back(midcode(jump_false + ":", "", "", ""));
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
			vSymbol* var = findSymbol(cur->childs[0]->childs[0]->name.get_data(), this->cur_function.name);
			if (var != nullptr)
			{
				cur->pos = var->pos;
			}
			else
			{
				std::cout << "ERROR: Variable " << cur->childs[0]->childs[0]->name.get_data() << " Non-existent" << std::endl;
				exit(1);
			}
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
			std::vector<acpara> acpara_table_backup;
			for (auto i : acpara_table)
				acpara_table_backup.push_back(i);
			acpara_table.clear();
			//�ݴ�ʵ�α��Դ���Ƕ��
			for (auto i : cur->childs)
				analyze(i);
			cur->pos = getNewTemp();
			std::string result = std::string("T") + std::to_string(cur->pos);
			std::string func_name = cur->childs[0]->childs[0]->name.get_data();
			auto f = findFunction(func_name);
			if (f != nullptr)
			{
				if (acpara_table.size() != f->paras.size())
				{
					std::cout << "ERROR: Parameter Mismatch in Function " << f->name << std::endl;
					exit(1);
				}
				for (auto i : acpara_table)
				{
					std::string p = std::get<2>(i);
					midcode_set.push_back(midcode("Push", p, "", ""));
				}
				std::string func_entry = "F" + std::to_string(f->entry);
				midcode_set.push_back(midcode("Call", func_entry, "", ""));
				midcode_set.push_back(midcode("Pop", "", "", result));
			}
			else
			{
				std::cout << "ERROR: Function " << func_name << " Non-existent" << std::endl;
				exit(1);
			}
			acpara_table.clear();
			for (auto i : acpara_table_backup)
				acpara_table.push_back(i);
		}
	}
	else if (header == "<acPara_List>")
	{
		if (cur->childs.size() == 1)
			//"<acPara_List>", "<Expr>"
		{
			analyze(cur->childs[0]);
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
			analyze(cur->childs[0]);
			TokenKind type = cur->childs[0]->datatype;
			std::string name = "";
			std::string arg;
			if (cur->childs[0]->data)
				arg = std::to_string(cur->childs[0]->data);
			else
				arg = "T" + std::to_string(cur->childs[0]->pos);
			acpara_table.push_back(acpara(type, name, arg));
			analyze(cur->childs[2]);
		}
	}
	else if (header == "<ID>")
		//"<ID>", "identifier"
	{
		//������������������Ϊid��2���÷�
		for (auto i : cur->childs)
			analyze(i);
	}
}