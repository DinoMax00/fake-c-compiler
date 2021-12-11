#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <tuple>

#include "Token.h"
#include "Grammer/CFG.h"

typedef std::tuple<std::string, std::string, std::string, std::string> code_block;

/// 语句块
class sNode {
public:
	std::string place = "";	/// 语句块入口的中间变量
	std::vector<code_block*> code; /// 传递而来或者生成的中间代码
	std::vector<sNode> stack; /// 翻译闭包表达式所用的临时栈

	std::string name = ""; /// 语句块的标识符
	TokenKind type; /// 节点携带的数据类型
	std::string data = ""; /// 节点携带的数据
	std::string arr_name = ""; /// 数组的名字
	std::vector<int> dims; /// 数组的维度
	TokenKind type; /// 节点的数据类型
};

/// 符号表
class sSymbol {
public:
	std::string name; /// 符号的标识符
	TokenKind type; /// 类型

	int size = 0; /// 占用字节数
	int offset = 0; /// 内存偏移量

	std::string place = ""; /// 对应的中间变量
	int function = 0; /// 所在函数id
	std::vector<int> dims; /// 数组的维度
};

/// 函数表
class FunctionSymbol {
public:
	std::string name; /// 函数的标识符、
	std::string type; /// 返回值类型
	int label; /// 入口处的标签
	std::vector<std::string> params; /// 形参列表
};

// 语义分析
class Semantic {
private:
	std::vector<sNode> sStack; /// 语义分析栈
	std::set<sSymbol*> symbol_table; /// 符号表
	std::set<FunctionSymbol*> func_table; /// 函数表

	int cur_temp_id = 0; /// 中间变量名序号
	int cur_label_id = 0; /// 当期label入口序号
	int cur_func_id = 0; /// 当前func序号
	int cur_offset = 0; /// 当前偏移量
	FunctionSymbol cur_func_symbol; /// 当前函数

	std::vector<code_block*> mid_code; /// 生成的中间代码
	bool semantic_rst = true;
	
public:
	Semantic() {

	}

	/// 执行语义分析
	void analyze(Grammer::Production prod, std::vector<Token> shift_str) {
		auto nt = prod.get_header();
		auto r = prod.get_bodys()[0];

		/// program -> declarationChain
		if (nt.is("<program>")) {
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();

			for (auto node : n.stack) {
				for (auto code : node.code) {
					n.code.push_back(code);
				}
			}

			mid_code = n.code;
			sStack.push_back(n);
		}

		/// block -> { statementChain }
		/// statement->declaration | ifStatement | iterStatement | returnStatement | assignStatement
		else if (nt.is_one_of("statement", "block")) {
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();

			sStack.push_back(n);
		}

		/// declarationChain -> $ | declaration declarationChain
		else if (nt.is("declarationChain")) {
			sNode n;
			/// declaration declarationChain
			if (r.size() == 2) {
				n = sStack.back();
				sStack.pop_back();
				n.stack.insert(n.stack.begin(), n.stack.back());
				n.stack.pop_back();
			}
			n.name = nt.get_name();
			sStack.push_back(n);
		}

		/// typeSpecifier -> int | void
		else if (nt.is("typeSpecifier")) {
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();

			n.type = shift_str.back().get_kind();
			sStack.push_back(n);
		}

		/// declaration -> typeSpecifier id ; | completeFunction | typeSpecifier id arrayDeclaration ;
		else if (nt.is("declaration")) {
			/// -> typeSpecifier id ;
			if (r.size() == 3) {
				auto n = sStack.back();
				sStack.pop_back();
				n.name = nt.get_name();
				auto defType = n.type;
				auto defName = shift_str[shift_str.size() - 2].get_data();
				auto s = findSymbol(defName, cur_func_symbol.label);
				if (s) {
					/// 重定义
					int row = shift_str[shift_str.size() - 2].get_row();
					int col = shift_str[shift_str.size() - 2].get_col();
					std::cout << "ERROR: redefined variable ! At row " << row << ", col " << col << "." << std::endl;
					exit(0);
				}
				else {
					s = new sSymbol();
				}

				if (n.place == "") {
					s->name = defName;
					s->place = getNewTemp();
					s->type = defType;
					s->function = cur_func_symbol.label;
					s->size = 4;
					s->offset = cur_offset;
					cur_offset += s->size;
					updateSymbolTable(s);

					/// 不是常数
					if (n.data != "") {
						auto code = new code_block(":=", n.data, "_", s->place);
						n.code.push_back(code);
					}
				}
				else {
					s->name = defName;
					s->place = n.place;
					s->type = defType;
					s->function = cur_func_id;
					s->size = 4;
					s->offset = cur_offset;
					cur_offset += s->size;
					updateSymbolTable(s);
					//for (auto code : n.code) {
					//	n.code.sta
					//}
				}
			}
		}
	}
private:
	/// 在符号表中查找符号
	sSymbol* findSymbol(std::string name, int func) {
		for (auto s : symbol_table) {
			if (s->name == name && s->function == func) {
				return s;
			}
		}
		return nullptr;
	}

	/// 更新符号表
	void updateSymbolTable(sSymbol* sym) {
		for (auto s : symbol_table) {
			if (s->name == sym->name && s->function == sym->function) {
				symbol_table.erase(s);
				break;
			}
		}
		symbol_table.insert(sym);
		return;
	}

	std::string getNewTemp() {
		++cur_temp_id;
		return "t" + std::to_string(cur_temp_id);
	}

	std::string getNewLabel() {
		++cur_label_id;
		return "l" + std::to_string(cur_label_id);
	}

	std::string getNewFuncLabel() {
		++cur_func_id;
		return "f" + std::to_string(cur_func_id);
	}
};
