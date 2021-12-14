#pragma once

#include <vector>
#include <Windows.h>

#include "Token.h"
#include "grammer/CFG.h"
#include "json/CJsonObject.hpp"

#include <iostream>

/// color print
constexpr int RED = FOREGROUND_RED;
constexpr int YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
constexpr int GREEN = FOREGROUND_GREEN;
constexpr int BLUE = FOREGROUND_BLUE;
constexpr int WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

typedef std::tuple<std::string, std::string, std::string, std::string> midcode;
typedef std::pair<TokenKind, std::string> para;
typedef std::tuple<TokenKind, std::string, std::string> acpara;	//类型,名称，存储形式
class Semantic;

namespace AST {
	class node {
		Token name;
		Grammer::Symbol type;
		std::vector<node*> childs;

		TokenKind datatype; // 数据类型
		int pos;	// 符号表中位置
		int data; // 数据
		//place/data只能其一非空
		std::vector<midcode> midcode_set;	//中间代码

		void color_print(std::string str, int COLOR) {
			using namespace std;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | COLOR);
			cout << str;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
	public:
		node(Grammer::Symbol _type) : type(_type) {}
		void set_name(Token _name) { name = _name; }
		std::string get_name() { return type.get_name(); }
		void add_child(node* nd) { childs.insert(childs.begin(), nd); }
		//注意要始终在首位插入

		void print_space(std::vector<int>* deps, bool flg = false) {
			using std::cout;

			int cur = 0;
			for (int i = 1; cur < deps->size(); ++i) {
				if (i == (*deps)[cur]) {

					if (cur == deps->size() - 1) {
						if (flg) cout << "└─ ";
						else cout << "├─ ";
					}
					else
						cout << "│  ";
					++cur;
				}
				else
					cout << "   ";
			}
		}

		void print(int d = 0, std::vector<int>* deps = new std::vector<int>, bool flg = false) {
			using std::cout;
			using std::endl;
			print_space(deps, flg);

			if (childs.size()) {
				color_print(type.get_name(), GREEN);
				cout << endl;
			}
			else {
				color_print(name.get_tokenkind_name(), YELLOW);
				cout << ": " << name << endl;
			}

			if (flg && deps->size()) {
				deps->pop_back();
			}
			for (int i = childs.size() - 1; i >= 0; --i) {
				auto v = new std::vector<int>(*deps);
				v->push_back(d + 1);
				if (i == 0)
					childs[i]->print(d + 1, v, true);
				else
					childs[i]->print(d + 1, v);
			}
		}

		neb::CJsonObject json_print() {
			neb::CJsonObject js;

			if (childs.size()) {
				js.Add("name", type.get_name());
				js.AddEmptySubArray("children");
				for (int i = childs.size() - 1; i >= 0; --i) {
					js["children"].Add(childs[i]->json_print());
				}
			}
			else {
				js.Add("name", name.get_tokenkind_name());
				js.Add("value", name.get_data());
				js.Add("loc", "<" + std::to_string(name.get_row()) + "," + std::to_string(name.get_col()) + ">");
			}

			return js;
		}

		friend class Semantic;
	};
}


/// 变量符号
class vSymbol {
public:
	std::string name;	//变量名称
	TokenKind type;	//数据类型
	int size = 0;	//变量大小
	int offset = 0;	//偏移量
	int id = 0;	//标识id
	int pos = 0;	//对应的中间变量号
	std::string func = "";	//所处函数，可为空
};

/// 函数符号
class fSymbol {
public:
	std::string name;	//函数名称
	TokenKind type;	//函数返回值类型
	int entry = 0;	//函数入口
	std::vector<para> paras;	//形参列表
};

class Semantic
{
private:
	std::vector<vSymbol> symbol_table;	//变量表
	std::vector<fSymbol> function_table;	//函数表



	int cur_temp_id = 0; //中间变量序号
	int cur_label_id = 0; //跳转标签序号
	int cur_func_id = 0; //函数入口序号
	int cur_offset = 0; //偏移量
	fSymbol cur_function;	//当前函数

	std::vector<midcode> midcode_set;	//中间代码序列
	std::vector<para> para_table;	//形参暂存表
	std::vector<acpara> acpara_table;	//实参暂存表

public:
	/// 在变量表中查找变量
	vSymbol* findSymbol(std::string name, std::string func) {
		for (int i = 0; i < symbol_table.size(); i++) {
			if (symbol_table[i].name == name && symbol_table[i].func == func) {
				return &symbol_table[i];
			}
		}
		for (int i = 0; i < symbol_table.size(); i++) {
			if (symbol_table[i].name == name && symbol_table[i].func == "") {
				return &symbol_table[i];
			}
		}
		return nullptr;
	}
	/// 在函数表中查找函数
	fSymbol* findFunction(std::string name) {
		for (int i = 0; i < function_table.size(); i++) {
			if (function_table[i].name == name) {
				return &function_table[i];
			}
		}
		return nullptr;
	}

	/// 更新符号表
	int updateSymbolTable(vSymbol sym) {
		for (int i = 0; i < symbol_table.size(); i++)
		{
			if (symbol_table[i].name == sym.name && symbol_table[i].func == sym.func)
				return 0;
		}
		symbol_table.push_back(sym);
		return 1;
	}

	/// 更新函数表
	int updateFuncTable(fSymbol fs) {
		for (int i = 0; i < function_table.size(); i++)
		{
			//前提是不允许函数嵌套
			if (function_table[i].name == fs.name)
				return 0;
		}
		function_table.push_back(fs);
		return 1;
	}

	int getNewTemp() {
		++cur_temp_id;
		return cur_temp_id;
	}

	int getNewLabel() {
		++cur_label_id;
		return cur_label_id;
	}

	int getNewFuncLabel() {
		++cur_func_id;
		return cur_func_id;
	}

	void analyze(AST::node*);

	void print()
	{
		using std::cout;
		for (auto i : midcode_set)
		{
			int format = 10 - std::get<0>(i).size();
			cout << std::get<0>(i);
			for (int i = 0; i < format; i++)
				cout << " ";
			format = 10 - std::get<1>(i).size();
			cout << std::get<1>(i);
			for (int i = 0; i < format; i++)
				cout << " ";
			format = 10 - std::get<2>(i).size();
			cout << std::get<2>(i);
			for (int i = 0; i < format; i++)
				cout << " ";
			format = 10 - std::get<3>(i).size();
			cout << std::get<3>(i);
			for (int i = 0; i < format; i++)
				cout << " ";
			cout << std::endl;
		}
	}
};