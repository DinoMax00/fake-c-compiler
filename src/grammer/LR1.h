#pragma once

#include "CFG.h"

/// ��Production�Ļ����������dot��λ���Լ�Ҫ�õ����Ǹ����� [A->.abc , x]
/// dotPos��ʾ���ڵ�dotPos��symbol֮ǰ
/// ����LR1�ķ�����Ŀ
class ExProduction : public Grammer::Production {
	/// S -> .abc
	int dotPos;
	Grammer::Symbol tail;
};


/// LR1�̳���CFG�� �����������շ� �����������»���
/// ��Ҫʵ�֣�
/// 1. ���캯�������ɼ��˵��Ժ�������ķ�����exProductions��
/// 2. ��һ�����ŵ�first������дһ��LL1.cpp����ľ���
/// 3. ����closure���Զ���ɶ�ģ���ͼ��
/// 4.����action�� goto����map<symbol, map<symbol, xxx> Ӧ�þ��У����Բο�һ��ll1�ķ�����
class LR1 : public Grammer::CFG {
	std::vector<shared_ptr<ExProduction> > exProductions;

	std::set<Grammer::Symbol> get_first_set(std::vector<Grammer::Symbol>);
public:
	LR1();
};
