#pragma once

#include "CFG.h"

/// 在Production的基础上添加了dot的位置以及要用到的那个符号 [A->.abc , x]
/// dotPos表示点在第dotPos个symbol之前
/// 就是LR1文法的项目
class ExProduction : public Grammer::Production {
	/// S -> .abc
	int dotPos;
	Grammer::Symbol tail;
};


/// LR1继承了CFG类 变量名都用驼峰 函数名都用下划线
/// 需要实现：
/// 1. 构造函数，生成加了点以后的扩充文法存在exProductions里
/// 2. 求一串符号的first集，改写一下LL1.cpp里面的就行
/// 3. 构造closure，自动机啥的，建图。
/// 4.构造action表 goto表，用map<symbol, map<symbol, xxx> 应该就行，可以参考一下ll1的分析表
class LR1 : public Grammer::CFG {
	std::vector<shared_ptr<ExProduction> > exProductions;

	std::set<Grammer::Symbol> get_first_set(std::vector<Grammer::Symbol>);
public:
	LR1();
};
