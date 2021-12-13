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

	/// Symbol类
	/// 产生式里所有的符号都是这个类型，通过两个get方法得到他的名字和类型。
	/// 如果产生式名字是<xxx> 那么就认为是非终结符
	/// 所有产生式都在GrammerRules.h里
	/// 重载了==和!=运算符，可以直接和Symbol或者string比较是否相等

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

	/// 这个是一条产生式的右半部分，就是一系列Symbol组成的vector

	/// rhs of a production
	typedef std::vector<Symbol> ProductionBody;

	/// 产生式类型
	/// 包括左半部分header和右半部分
	/// 因为之后合并的时候会有A->abc|ef|gf, 一个A能推导出来多个右半部分，所以用vector存这些右半部分(bodys)
	/// 重载了输出运算符，所以可以直接cout << xxx; 用来debug

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

	/// 上下文无关文法
	/// 包括终结符 非终结符 产生式 开始的符号
	/// 在构造函数里会合并类似A->xxx A->yyy这样两条文法变成 A->xxx|yyy
	/// 所以还定义了initialProductions，保存了合并前的所有产生式，用于文法分析的时候一条一条判断。
	/// remove_recursive用于消除左递归，消除后的文法存在productions里

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

