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
	std::string function = ""; /// ���ں���id
	std::vector<int> dims; /// �����ά��
};

/// ������
class FunctionSymbol {
public:
	std::string name; /// �����ı�ʶ����
	TokenKind type; /// ����ֵ����
	std::string label; /// ��ڴ��ı�ǩ
	std::vector<param*> params; /// �β��б�
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
					/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!����Դ����������
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
			/// ��main����
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

			// ����ֵ����
			auto nFuncReturnType = sStack.back();
			sStack.pop_back();
			// �ǼǺ���
			auto f = new FunctionSymbol();
			f->name = shift_str[shift_str.size() - 4].get_data();
			f->type = nFuncReturnType.type;
			if (f->name == "main") {
				f->label = "main";
			}
			else {
				f->label = getNewFuncLabel();
			}

			/// ����formalParaList����¼�����б�
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
			sStack.clear(); // ���
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
				// statement.code��statementChain.code��˳���, ��ǰ��Ҫ�ں���ǰ��
				for (int i = n.stack[0].code.size() - 1; i >= 0; --i) {
					n.code.insert(n.code.begin(), n.stack[0].code[i]);
				}

				sStack.push_back(n);
			}
		}
	}
private:
	/// �ڷ��ű��в��ҷ���
	sSymbol* findSymbol(std::string name, std::string func) {
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

	/// ���º�����
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
