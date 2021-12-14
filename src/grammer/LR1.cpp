#include <algorithm>
#include <set>
#include "LR1.h"

LR1::LR1()
{
	int counter = 0;
	int production_num = productions.size();
	shared_ptr<ExProduction> exprPtr = std::make_shared<ExProduction>();
	exprPtr->set_header(productions[0]->get_header());
	exprPtr->append_body(productions[0]->get_bodys()[0]);
	exprPtr->set_dotPos(0);
	exprPtr->set_tail(Grammer::Symbol("#"));
	exprPtr->set_hash();
	exProductions.push_back(exprPtr);

	for (int i = 0; i < production_num; i++)
	{
		auto bodys_now = productions[i]->get_bodys();
		int body_num = bodys_now.size();
		for (int j = 0; j < body_num; j++)
		{
			auto body_now = bodys_now[j];
			int symbol_num = body_now.size();
			for (int k = 0; k < symbol_num + 1; k++)
			{
				for (auto it : terminalSet)
				{
					shared_ptr<ExProduction> exprPtr = std::make_shared<ExProduction>();
					exprPtr->set_header(productions[i]->get_header());
					exprPtr->append_body(body_now);
					exprPtr->set_dotPos(k);
					exprPtr->set_tail(it);
					exprPtr->set_hash();
					exProductions.push_back(exprPtr);
				}
				for (auto it : nonTerminalSet)
				{
					shared_ptr<ExProduction> exprPtr = std::make_shared<ExProduction>();
					exprPtr->set_header(productions[i]->get_header());
					exprPtr->append_body(body_now);
					exprPtr->set_dotPos(k);
					exprPtr->set_tail(it);
					exprPtr->set_hash();
					exProductions.push_back(exprPtr);
				}
			}
		}
	}
	generate_normal_family();
}

std::ostream& operator << (std::ostream& out, ExProduction expr) {
	out << expr.get_header().get_name() << " -> ";
	auto body = expr.get_bodys()[0];
	for (int i = 0; i < (int)body.size(); i++) {
		if (i == expr.get_dotPos())
			out << " ¡ð ";
		out << body[i].get_name() << " ";
	}
	if (body.size() == expr.get_dotPos())
		out << " ¡ð ";
	out << " || ";
	out << expr.get_tail();
	out << std::endl;
	return out;
}

std::set<Grammer::Symbol> LR1::get_first_set(Grammer::Symbol sym, bool epsilon_control) {
	std::set<Grammer::Symbol> firstSet;

	/// rule1:
	/// for terminal symbol, 
	/// it's first set is itself
	if (sym.get_type() == Grammer::SymbolType::TERMINAL) {

		firstSet.insert(sym);
		return firstSet;
	}

	/// rule2:
	/// for non terminal symbol,
	/// A->XYZ, 
	/// if X=>epsilon, then first(A).insert(first(X)), check Y
	/// if XYZ=>epsilon, then first(A).insert(epsilon)
	int n = productions.size();
	for (int i = 0; i < n; ++i) {
		if (productions[i]->get_header() != sym)
			continue;

		for (auto body : productions[i]->get_bodys()) {
			bool epsilon = true;

			for (auto it : body) {
				auto tmpSet = get_first_set(it, true);

				if (!tmpSet.count(std::string("¦Å"))) {
					epsilon = false;
				}

				/// merge
				firstSet.insert(tmpSet.begin(), tmpSet.end());

				if (!epsilon)
					break;
			}

			if (epsilon)
				firstSet.insert(std::string("¦Å"));
		}
	}

	return firstSet;
}


std::set<Grammer::Symbol> LR1::get_first_set_for_string(std::vector<Grammer::Symbol> sym_vec) {
	std::set<Grammer::Symbol> firstSet;

	int n = sym_vec.size();
	std::set<Grammer::Symbol> firstSet_sym;
	bool epsilon = true;
	for (int i = 0; i < n && epsilon; i++)
	{
		firstSet_sym = get_first_set(sym_vec[i], false);
		if (!firstSet_sym.count(std::string("¦Å"))) {
			epsilon = false;
		}
		else
		{
			epsilon = true;
			firstSet_sym.erase(std::string("¦Å"));
		}
		firstSet.insert(firstSet_sym.begin(), firstSet_sym.end());
	}

	if (epsilon)
		firstSet.insert(std::string("¦Å"));

	return firstSet;
}


std::vector<ExProduction> LR1::get_closure(std::vector<ExProduction> closure)
{
	bool control = true;
	int visited[1000] = { 0 };
	std::map<ExProduction, int> dupCheck;
	while (control)
	{
		control = false;
		std::vector<ExProduction> add;
		std::map<ExProduction, int> visitedCheck;
		int i = 0;
		for (auto it : closure)
			visitedCheck[it] = 1;
		for (auto it : closure)
		{
			if (dupCheck[it])
				continue;
			else
				dupCheck[it] = 1;
			auto body = it.get_bodys()[0];
			int pos = it.get_dotPos();
			if (pos < (int)body.size() && body[pos].get_type() == Grammer::SymbolType::N_TERMINAL)
			{
				Grammer::Symbol sym_obj = body[pos];
				for (auto it2 : exProductions)
				{
					if (it2->get_header() == sym_obj && it2->get_dotPos() == 0)
					{
						std::vector<Grammer::Symbol> next_set;
						for (int i = pos + 1; i < (int)body.size(); i++)
							next_set.push_back(body[i]);
						next_set.push_back(it.get_tail());
						auto first_set = get_first_set_for_string(next_set);
						ExProduction temp;
						temp.append_body(it2->get_bodys()[0]);
						temp.set_header(sym_obj);
						temp.set_dotPos(0);
						for (auto sym : first_set)
						{
							temp.set_tail(sym);
							temp.set_hash();
							if (!visitedCheck[temp])
							{
								visitedCheck[temp] = 1;
								add.push_back(temp);
							}
						}
					}
				}
			}
		}
		if (add.size() > 0)
		{
			for (auto it : add)
				closure.push_back(it);
			control = true;
		}
	}
	return closure;
}

std::vector<ExProduction> LR1::get_go(std::vector<ExProduction> expr_vec, Grammer::Symbol sym)
{
	std::vector<ExProduction> go;
	for (auto it : expr_vec)
	{
		auto body = it.get_bodys()[0];
		int pos = it.get_dotPos();
		if (pos < (int)body.size() && body[pos] == sym)
		{
			ExProduction temp;
			temp.append_body(body);
			temp.set_header(it.get_header());
			temp.set_tail(it.get_tail());
			temp.set_dotPos(pos + 1);
			temp.set_hash();
			go.push_back(temp);
		}
	}
	go = get_closure(go);
	return go;
}

std::vector<std::vector<ExProduction> > LR1::generate_normal_family()
{
	std::cout << "generating parser table...";
	int count = 0;
	std::vector<ExProduction> start;
	std::map<std::vector<ExProduction>, int> dupCheck;
	std::map<std::vector<ExProduction>, int> setIndex;
	start.push_back(*exProductions[0]);
	start = get_closure(start);
	normal_family.push_back(start);
	setIndex[start] = ++count;
	bool control = true;
	while (control)
	{
		std::cout << "...";
		control = false;
		std::vector<std::vector<ExProduction> > add;
		for (auto item_set : this->normal_family)
		{
			if (dupCheck[item_set])
				continue;
			else
				dupCheck[item_set] = 1;
			State new_state;
			new_state.state_exproductions = item_set;
			for (auto it : item_set)
			{
				if (it.get_dotPos() == it.get_bodys()[0].size())
				{
					if (it.get_header() == get_start_symbol())
					{
						new_state.edge[Grammer::Symbol("#")] = 0;
						new_state.type[Grammer::Symbol("#")] = 1;
					}
					else
					{
						int s;
						for (s = 0; s < (int)new_state.state_exproductions.size(); s++)
							if (new_state.state_exproductions[s] == it)
								break;
						new_state.edge[it.get_tail()] = s;
						new_state.type[it.get_tail()] = 2;
					}
				}
			}
			for (auto it : terminalSet)
			{
				auto new_go = get_go(item_set, it);
				if (new_go.size() > 0)
				{
					if (!setIndex[new_go])
					{
						setIndex[new_go] = ++count;
						add.push_back(new_go);
					}
					int s = setIndex[new_go] - 1;
					new_state.edge[it] = s;
					new_state.type[it] = 3;
				}
			}
			for (auto it : nonTerminalSet)
			{
				auto new_go = get_go(item_set, it);
				if (new_go.size() > 0)
				{
					if (!setIndex[new_go])
					{
						setIndex[new_go] = ++count;
						add.push_back(new_go);
					}
					int s = setIndex[new_go] - 1;
					new_state.edge[it] = s;
					new_state.type[it] = 4;

				}
			}

			parse_table.push_back(new_state);
		}
		if (add.size() > 0)
		{
			for (auto it : add)
				this->normal_family.push_back(it);
			control = true;
		}
	}
	std::cout << std::endl;
	return this->normal_family;
}

std::vector<State>  LR1::get_parser_table()
{
	return this->parse_table;
}