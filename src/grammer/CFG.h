#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <memory>

using std::shared_ptr;

/// define a CFG class
namespace Grammer {

	enum class SymbolType {
		UNKNOWN,
		TERMINAL,
		N_TERMINAL
	};

	/// Symbol��
	/// ����ʽ�����еķ��Ŷ���������ͣ�ͨ������get�����õ��������ֺ����͡�
	/// �������ʽ������<xxx> ��ô����Ϊ�Ƿ��ս��
	/// ���в���ʽ����GrammerRules.h��
	/// ������==��!=�����������ֱ�Ӻ�Symbol����string�Ƚ��Ƿ����

	/// grammer symbols
	class Symbol {
		std::string name;
		SymbolType type = SymbolType::UNKNOWN;

	public:
		Symbol() {}
		Symbol(std::string _name): name(_name) {
			if (name[0] == '<' && name[_name.size()-1] == '>')
				type =  SymbolType::N_TERMINAL;
			else 
				type =  SymbolType::TERMINAL;
		}

		/// get functions
		std::string get_name() { return name; }
		SymbolType get_type() { return type; }
		friend std::ostream& operator <<(std::ostream& out, Symbol sym);

		/// set functions
		void set_name(std::string _name) { name = _name; }
		void set_type(SymbolType _type) { type = _type; }

		/// check equal
		bool operator == (const Symbol& rhs) const {
			return name == rhs.name;
		}
		bool operator == (const std::string& rhs) const {
			return name == rhs;
		}
		bool operator != (const Symbol& rhs) const {
			return name != rhs.name;
		}
		bool operator != (const std::string& rhs) const {
			return name != rhs;
		}
		/// needed to use set
		bool operator < (const Symbol& rhs) const {
			return name < rhs.name;
		}
	};

	/// �����һ������ʽ���Ұ벿�֣�����һϵ��Symbol��ɵ�vector

	/// rhs of a production
	typedef std::vector<Symbol> ProductionBody;

	/// ����ʽ����
	/// ������벿��header���Ұ벿��
	/// ��Ϊ֮��ϲ���ʱ�����A->abc|ef|gf, һ��A���Ƶ���������Ұ벿�֣�������vector����Щ�Ұ벿��(bodys)
	/// �������������������Կ���ֱ��cout << xxx; ����debug

	/// A->abc
	class Production {
		/// lhs and rhs
		/// and a rhs may contain many ProductionBodys like A->a|b
		Symbol header;
		std::vector<ProductionBody> bodys;
	
	public:
		Production() {}
		Production(Symbol _header) : header(_header) {}
		friend std::ostream& operator <<(std::ostream& out, Production sym);
		Symbol get_header() const { return header; }
		std::vector<ProductionBody>& get_bodys() { return bodys; }
		std::vector<ProductionBody> get_bodys_const() const { return bodys; }
		const std::vector<ProductionBody>& get_bodys_const() { return bodys; }
		void set_header(Symbol _header) { header = _header; }
		/// add a rule
		void append_body(ProductionBody body) { bodys.push_back(body); }

		/// merge body, like A->a merge A->b
		Production& operator += (const Production rhs) {
			if (header != rhs.header) {
				return *this;
			}
			
			for (auto& it : rhs.bodys) {
				bodys.push_back(it);
			}

			return *this;
		}
	};

	/// �������޹��ķ�
	/// �����ս�� ���ս�� ����ʽ ��ʼ�ķ���
	/// �ڹ��캯�����ϲ�����A->xxx A->yyy���������ķ���� A->xxx|yyy
	/// ���Ի�������initialProductions�������˺ϲ�ǰ�����в���ʽ�������ķ�������ʱ��һ��һ���жϡ�
	/// remove_recursive����������ݹ飬��������ķ�����productions��

	class CFG {
	protected:
		/// V T P S
		/// S->Ab, A->a
		std::set<Symbol> terminalSet; // {a,b}
		std::set<Symbol> nonTerminalSet; // {S, A}
		std::vector<shared_ptr<Production> > productions; // {S->Ab, A->a}
		std::vector<shared_ptr<Production> > initialProductions; 
		Symbol startSymbol; // {S}

	public:
		CFG();
		/// print the grammer
		friend std::ostream& operator <<(std::ostream& out, const CFG& cfg);
		std::set<Symbol> get_terminalSet(){	return terminalSet;}
		std::set<Symbol> get_nonTerminalSet() { return nonTerminalSet; }
		void merge_productions();
		CFG& remove_recursive();

		void test() {
			using std::cout;
			using std::endl;
			cout << remove_recursive() << endl;
			cout << "Terminal Set:" << endl;
			for (auto it : terminalSet) cout << it.get_name() << endl;
			cout << endl;
			cout << "nTerminal Set:" << endl;
			for (auto it : nonTerminalSet) cout << it.get_name() << endl;
		}
	};

}

