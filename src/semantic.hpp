#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <tuple>
#include <assert.h>
#include "Token.h"
#include "Grammer/CFG.h"

typedef std::tuple<std::string, std::string, std::string, std::string> code_block;
typedef std::tuple<std::string, TokenKind, std::string> param;

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
	std::string function = ""; /// 所在函数id
	std::vector<int> dims; /// 数组的维度
};

/// 函数表
class FunctionSymbol {
public:
	std::string name; /// 函数的标识符、
	TokenKind type; /// 返回值类型
	std::string label; /// 入口处的标签
	std::vector<param*> params; /// 形参列表
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
		else if (nt.is_one_of("<statement>", "<block>")) {
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();

			sStack.push_back(n);
		}

		/// declarationChain -> $ | declaration declarationChain
		else if (nt.is("<declarationChain>")) {
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
		else if (nt.is("<typeSpecifier>")) {
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();

			n.type = shift_str.back().get_kind();
			sStack.push_back(n);
		}

		/// declaration -> typeSpecifier id ; | completeFunction | typeSpecifier id arrayDeclaration ;
		else if (nt.is("<declaration>")) {
			auto n = sNode();
			/// -> typeSpecifier id ;
			if (r.size() == 3) {
				n = sStack.back();
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
					/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!这里源代码有问题
					//for (auto code : n.code) {
					//	n.code.sta
					//}
				}
			}
			/// -> typeSpecifier id arrayDeclaration;
			else if (r.size() == 4) {

			}
			/// -> completeFunction
			else if (r.size() == 1) {
				n = sStack.back();
				sStack.pop_back();
				n.name = nt.get_name();
			}
			sStack.push_back(n);
		}

		/// completeFunction -> declareFunction block
		else if (nt.is("<completeFunction>")) {
			auto n = sStack.back(); /// block
			sStack.pop_back();

			auto nDefine = sStack.back(); /// declareFunction
			sStack.pop_back();

			n.name = nt.get_name();
			std::vector<code_block*> codeTmp;
			codeTmp.push_back(new code_block(nDefine.data, ":", "_", "_"));

			/// params
			for (int i = 0; i < nDefine.stack.size(); ++i) {
				auto node = nDefine.stack[i];
				codeTmp.push_back(new code_block("pop", "_", std::to_string(4 * i), node.place));
			}

			if (nDefine.stack.size()) {
				codeTmp.push_back(new code_block("-", "fp", std::to_string(4 * nDefine.stack.size()), "fp"));
			}

			std::reverse(codeTmp.begin(), codeTmp.end());
			for (auto code : codeTmp) {
				n.code.insert(n.code.begin(), code);
			}

			auto code_end = n.code.back();
			/// 非main函数
			if (std::get<0>(*code_end)[0] != 'l') {
				auto label = std::get<0>(*code_end);
				n.code.pop_back();
				for (int i = 0; i < n.code.size(); ++i) {
					auto code = n.code[i];
					if (std::get<3>(*code) == label) {
						n.code.erase(n.code.begin() + i);
					}
				}
			}

			sStack.push_back(n);
		}

		/// declareFunction -> typeSpecifier id ( formalParaList )
		else if (nt.is("<declareFunction>")) {
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();

			// 返回值类型
			auto nFuncReturnType = sStack.back();
			sStack.pop_back();
			// 登记函数
			auto f = new FunctionSymbol();
			f->name = shift_str[shift_str.size() - 4].get_data();
			f->type = nFuncReturnType.type;
			if (f->name == "main") {
				f->label = "main";
			}
			else {
				f->label = getNewFuncLabel();
			}

			/// 搜索formalParaList表，记录参数列表
			for (auto arg : n.stack) {
				auto s = new sSymbol();
				s->name = arg.data;
				s->place = arg.place;
				s->type = arg.type;
				s->function = f->label;
				s->size = 4;
				s->offset = cur_offset;
				cur_offset += s->size;

				updateSymbolTable(s);

				f->params.push_back(new param(arg.data, arg.type, arg.place));
			}

			n.data = f->label;
			updateFuncTable(f);
			sStack.clear(); // 清空
			cur_func_symbol = *f;
			sStack.push_back(n);
		}

		/// formalParaList -> $ | para | para, formalParaList | void
		else if (nt.is("<formalParaList>")) {
			auto n = sNode();
			/// formalParaList -> para , formalParaList
			if (r.size() == 3) {
				n = sStack.back();
				sStack.pop_back();
				n.name = nt.get_name();
				n.stack.insert(n.stack.begin(), sStack.back());
			}
			/// $ | void
			else if (r.size() == 1 && (r[0].is_one_of("$", "void"))) {
				n.name = nt.get_name();
			}
			else if (r.size() == 1 && r[0].is("<para>")) {
				n.stack.insert(n.stack.begin(), sStack.back());
				n.name = nt.get_name();
			}
			sStack.push_back(n);
		}

		/// para -> typeSpecifier id
		else if (nt.is("<para>")) {
			assert(r.size() == 2);
			auto n = sStack.back();
			sStack.pop_back();
			n.name = nt.get_name();
			n.place = getNewTemp();
			n.data = shift_str.back().get_data();
			sStack.push_back(n);
		}

		/// statementChain -> $ | statement statementChain
		else if (nt.is("<statementChain>")) {
			assert(r.size() <= 2);
			if (r.size() == 1) {
				auto n = sNode();
				n.name = nt.get_name();
				sStack.push_back(n);
			}
			else if (r.size() == 2) {
				auto n = sStack.back();
				sStack.pop_back();
				n.stack.insert(n.stack.begin(), sStack.back());
				n.name = nt.get_name();
				// statement.code，statementChain.code是顺序的, 但前者要在后者前面
				for (int i = n.stack[0].code.size() - 1; i >= 0; --i) {
					n.code.insert(n.code.begin(), n.stack[0].code[i]);
				}

				sStack.push_back(n);
			}
		}
	}
private:
	/// 在符号表中查找符号
	sSymbol* findSymbol(std::string name, std::string func) {
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

	/// 更新函数表
	void updateFuncTable(FunctionSymbol* fs) {
		for (auto f : func_table) {
			if (f->name == fs->name) {
				func_table.erase(f);
				break;
			}
		}
		func_table.insert(fs);
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
