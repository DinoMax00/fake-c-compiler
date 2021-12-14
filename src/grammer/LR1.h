#pragma once

#include <map>
#include "CFG.h"

/// 在Production的基础上添加了dot的位置以及要用到的那个符号 [A->.abc , x]
/// dotPos表示点在第dotPos个symbol之前
/// 就是LR1文法的项目
class ExProduction : public Grammer::Production {
	/// S -> .abc
	int dotPos = 0;
	std::string hash = "";
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
	void set_hash() { 
		hash = "";
		hash += this->get_header().get_name();
		for (auto sym : this->get_bodys()[0])
			hash += sym.get_name();
		hash += std::to_string(this->get_dotPos());
		hash += this->get_tail().get_name();
	}


	bool operator <(const ExProduction rhs) const
	{
		return this->hash < rhs.hash;
	}
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
	std::vector<std::vector<ExProduction> > normal_family;
	std::vector<State> parse_table;

	std::set<Grammer::Symbol> get_first_set(Grammer::Symbol, bool);
	std::set<Grammer::Symbol> get_first_set_for_string(std::vector<Grammer::Symbol>);
	std::vector<ExProduction> get_closure(std::vector<ExProduction>);
	std::vector<ExProduction> get_go(std::vector<ExProduction>, Grammer::Symbol);
	std::vector<std::vector<ExProduction> >  generate_normal_family();
public:
	std::vector<State>  get_parser_table();
	Grammer::Symbol get_start_symbol() { return startSymbol; }
	LR1();
	void test() {
	}
};
