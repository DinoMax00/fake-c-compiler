#pragma once

#include <map>
#include "CFG.h"

/// ��Production�Ļ����������dot��λ���Լ�Ҫ�õ����Ǹ����� [A->.abc , x]
/// dotPos��ʾ���ڵ�dotPos��symbol֮ǰ
/// ����LR1�ķ�����Ŀ
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
	//const�ᵼ��ĳЩ����?
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

/// LR1�̳���CFG�� �����������շ� �����������»���
/// ��Ҫʵ�֣�
/// 1. ���캯�������ɼ��˵��Ժ�������ķ�����exProductions��
/// 2. ��һ�����ŵ�first������дһ��LL1.cpp����ľ���
/// 3. ����closure���Զ���ɶ�ģ���ͼ��
/// 4.����action�� goto����map<symbol, map<symbol, xxx> Ӧ�þ��У����Բο�һ��ll1�ķ�����
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
