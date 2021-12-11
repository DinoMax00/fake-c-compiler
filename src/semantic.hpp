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

/// ����
class sNode {
public:
	std::string place = "";	/// ������ڵ��м����
	std::vector<code_block*> code; /// ���ݶ����������ɵ��м����
	std::vector<sNode> stack; /// ����հ����ʽ���õ���ʱջ

	std::string name = ""; /// ����ı�ʶ��
	TokenKind type; /// �ڵ�Я������������
	std::string data = ""; /// �ڵ�Я��������
	std::string arr_name = ""; /// ���������
	std::vector<int> dims; /// �����ά��
	TokenKind type; /// �ڵ����������
};

/// ���ű�
class sSymbol {
public:
	std::string name; /// ���ŵı�ʶ��
	TokenKind type; /// ����

	int size = 0; /// ռ���ֽ���
	int offset = 0; /// �ڴ�ƫ����

	std::string place = ""; /// ��Ӧ���м����
	int function = 0; /// ���ں���id
	std::vector<int> dims; /// �����ά��
};

/// ������
class FunctionSymbol {
public:
	std::string name; /// �����ı�ʶ����
	std::string type; /// ����ֵ����
	int label; /// ��ڴ��ı�ǩ
	std::vector<std::string> params; /// �β��б�
};

// �������
class Semantic {
private:
	std::vector<sNode> sStack; /// �������ջ
	std::set<sSymbol*> symbol_table; /// ���ű�
	std::set<FunctionSymbol*> func_table; /// ������

	int cur_temp_id = 0; /// �м���������
	int cur_label_id = 0; /// ����label������
	int cur_func_id = 0; /// ��ǰfunc���
	int cur_offset = 0; /// ��ǰƫ����
	FunctionSymbol cur_func_symbol; /// ��ǰ����

	std::vector<code_block*> mid_code; /// ���ɵ��м����
	bool semantic_rst = true;
	
public:
	Semantic() {

	}

	/// ִ���������
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
					/// �ض���
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

					/// ���ǳ���
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
	/// �ڷ��ű��в��ҷ���
	sSymbol* findSymbol(std::string name, int func) {
		for (auto s : symbol_table) {
			if (s->name == name && s->function == func) {
				return s;
			}
		}
		return nullptr;
	}

	/// ���·��ű�
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
