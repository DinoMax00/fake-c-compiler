#pragma once

#include <map>
#include "CFG.h"

/// 在Production的基础上添加了dot的位置以及要用到的那个符号 [A->.abc , x]
/// dotPos表示点在第dotPos个symbol之前
/// 就是LR1文法的项目
class ExProduction : public Grammer::Production {
	/// S -> .abc
	int dotPos;
	Grammer::Symbol tail;
public:
	friend std::ostream& operator <<(std::ostream& out, const ExProduction expr);
	bool operator == (const ExProduction& rhs) const {
		return (get_header() == rhs.get_header()) && (get_bodys_const()[0] == rhs.get_bodys_const()[0]) && (dotPos == rhs.dotPos) && (tail == rhs.tail);
	}
	//const会导致某些问题?
	int get_dotPos() { return dotPos; }
	void set_dotPos(int _dotPos) { dotPos = _dotPos; }
	Grammer::Symbol& get_tail() { return tail; }
	void set_tail(Grammer::Symbol _tail) { tail = _tail; }
};

class State {
public:
	std::vector<ExProduction> state_exproductions;
	std::map<Grammer::Symbol, int> edge;
	std::map<Grammer::Symbol, int> type;
	//acc r s GOTO
};

/// LR1继承了CFG类 变量名都用驼峰 函数名都用下划线
/// 需要实现：
/// 1. 构造函数，生成加了点以后的扩充文法存在exProductions里
/// 2. 求一串符号的first集，改写一下LL1.cpp里面的就行
/// 3. 构造closure，自动机啥的，建图。
/// 4.构造action表 goto表，用map<symbol, map<symbol, xxx> 应该就行，可以参考一下ll1的分析表
class LR1 : public Grammer::CFG {
	std::vector<shared_ptr<ExProduction> > exProductions;
	std::vector< std::vector<ExProduction> > normal_family;

	std::set<Grammer::Symbol> get_first_set(Grammer::Symbol, bool);
	std::set<Grammer::Symbol> get_first_set_for_string(std::vector<Grammer::Symbol>);
	std::vector<ExProduction> get_closure(std::vector<ExProduction>);
	std::vector<ExProduction> get_go(std::vector<ExProduction>, Grammer::Symbol);
	std::vector<std::vector<ExProduction>> generate_normal_family();
public:
	std::vector<State> get_parser_table();
	Grammer::Symbol get_start_symbol() { return Grammer::Symbol("Start"); }
	LR1();
	void test() {
		//for (auto i : exProductions)
		//	std::cout << *i;

		//for (auto it : nonTerminalSet) {
		//	std::vector<Grammer::Symbol> test;
		//	test.push_back(it);
		//	auto st = get_first_set_for_string(test);
		//	std::cout << "-->first" << it << std::endl;
		//	for (auto first_it : st) {
		//		std::cout << first_it << std::endl;
		//	}
		//}

		//for (auto i : exProductions)
		//{
		//	std::vector<ExProduction> closure;
		//	closure.push_back(*i);
		//	closure = get_closure(closure);
		//	std::cout << *i <<"  -->closure" << std::endl;
		//	for (auto j : closure)
		//		std::cout << j << std::endl;
		//}

		auto nf = generate_normal_family();
		//int i = 0;
		//for(auto it:nf)
		//{
		//	i++;
		//	std::cout << i << std::endl;
		//	for (auto expr : it)
		//	{
		//		std::cout << expr << std::endl;
		//	}
		//	std::cout << std::endl;
		//}
		auto table = get_parser_table();
		std::cout << nf.size() << std::endl;
		for (int i = 0; i < nf.size(); i++)
		{
			std::cout << i << "  ";
			for (auto it : terminalSet)
			{
				if (table[i].type[it] == 1)
					std::cout << it << " acc  ";
				else if (table[i].type[it] == 2)
					std::cout << it << " r" << table[i].edge[it] << "  ";
				else if (table[i].type[it] == 3)
					std::cout << it << " s" << table[i].edge[it] << "  ";
			}
			auto end = Grammer::Symbol("#");
			if (table[i].type[end] == 1)
				std::cout << end << " acc  ";
			else if (table[i].type[end] == 2)
				std::cout << end << " r" << table[i].edge[end] << "  ";
			else if (table[i].type[end] == 3)
				std::cout << end << " s" << table[i].edge[end] << "  ";
			for (auto it : nonTerminalSet)
			{
				if (table[i].type[it] == 4)
					std::cout << it << " GOTO" << table[i].edge[it] << "  ";
			}
			std::cout << std::endl;
		}
	}
};
