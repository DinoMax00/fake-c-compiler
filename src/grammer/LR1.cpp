#include <algorithm>
#include "LR1.h"

LR1::LR1()
{
	int production_num = productions.size();

	Grammer::Symbol start_new = Grammer::Symbol("<Start>");
	nonTerminalSet.insert(start_new);
	shared_ptr<ExProduction> exprPtr = std::make_shared<ExProduction>();
	Grammer::ProductionBody temp_body;
	temp_body.push_back(startSymbol);
	exprPtr->set_header(start_new);
	exprPtr->append_body(temp_body);
	exprPtr->set_dotPos(0);
	exprPtr->set_tail(Grammer::Symbol("#"));
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
					exProductions.push_back(exprPtr);
				}
				for (auto it : nonTerminalSet)
				{
					shared_ptr<ExProduction> exprPtr = std::make_shared<ExProduction>();
					exprPtr->set_header(productions[i]->get_header());
					exprPtr->append_body(body_now);
					exprPtr->set_dotPos(k);
					exprPtr->set_tail(it);
					exProductions.push_back(exprPtr);
				}
			}
		}
	}
	normal_family = generate_normal_family();
}

std::ostream& operator << (std::ostream& out, ExProduction expr) {
	out << expr.get_header().get_name() << " -> ";
	auto body = expr.get_bodys()[0];
	for (int i = 0; i < body.size(); i++) {
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
	while (control)
	{
		control = false;
		std::vector<ExProduction> add;
		for (auto it : closure)
		{
			auto body = it.get_bodys()[0];
			int pos = it.get_dotPos();
			if (pos < body.size() && body[pos].get_type() == Grammer::SymbolType::N_TERMINAL)
			{
				Grammer::Symbol sym_obj = body[pos];
				for (auto it2 : exProductions)
				{
					if (it2->get_header() == sym_obj && it2->get_dotPos() == 0)
					{
						std::vector<Grammer::Symbol> next_set;
						for (int i = pos + 1; i < body.size(); i++)
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
							bool check = false;
							for (auto check_it : closure)
							{
								if (check_it == temp)
								{
									check = true;
									break;
								}
							}
							for (auto check_it : add)
							{
								if (check_it == temp)
								{
									check = true;
									break;
								}
							}
							if (!check)
								add.push_back(temp);
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
		if (pos < body.size() && body[pos] == sym)
		{
			ExProduction temp;
			temp.append_body(body);
			temp.set_header(it.get_header());
			temp.set_tail(it.get_tail());
			temp.set_dotPos(pos + 1);
			go.push_back(temp);
		}
	}
	go = get_closure(go);
	return go;
}

std::vector< std::vector<ExProduction> > LR1::generate_normal_family()
{
	std::vector< std::vector<ExProduction> > normal_family;
	std::vector<ExProduction> start;
	start.push_back(*exProductions[0]);
	normal_family.push_back(get_closure(start));
	bool control = true;
	while (control)
	{
		control = false;
		std::vector< std::vector<ExProduction> > add;
		for (auto item_set : normal_family)
		{
			for (auto it : terminalSet)
			{
				auto new_go = get_go(item_set, it);
				if (new_go.size() > 0)
				{
					bool dup = false;
					for (auto check_it : normal_family)
					{
						if (check_it == new_go)
						{
							dup = true;
							break;
						}
					}
					for (auto check_it : add)
					{
						if (check_it == new_go)
						{
							dup = true;
							break;
						}
					}
					if (!dup)
						add.push_back(new_go);
				}
			}
			for (auto it : nonTerminalSet)
			{
				auto new_go = get_go(item_set, it);
				if (new_go.size() > 0)
				{
					bool dup = false;
					for (auto check_it : normal_family)
					{
						if (check_it == new_go)
						{
							dup = true;
							break;
						}
					}
					for (auto check_it : add)
					{
						if (check_it == new_go)
						{
							dup = true;
							break;
						}
					}
					if (!dup)
						add.push_back(new_go);
				}
			}
		}
		if (add.size() > 0)
		{
			for (auto it : add)
				normal_family.push_back(it);
			control = true;
		}
	}
	return normal_family;
}

int Search_set(std::vector< std::vector<ExProduction> > normal_family, std::vector<ExProduction> obj)
{
	int s;
	for (s = 0; s < normal_family.size(); s++)
	{
		if (obj == normal_family[s])
			break;
	}
	return s;
}
std::vector<State> LR1::get_parser_table()
{
	std::vector<State> parse_table;
	for (int i = 0; i < normal_family.size(); i++)
	{
		State new_state;
		new_state.state_exproductions = normal_family[i];
		auto item_set = normal_family[i];
		for (auto it : item_set)
		{
			if (it.get_dotPos() == it.get_bodys()[0].size())
			{
				if (it.get_header() == Grammer::Symbol("<Start>"))
				{
					new_state.edge[Grammer::Symbol("#")] = 0;
					new_state.type[Grammer::Symbol("#")] = 1;
				}
				else
				{
					int s;
					for (s = 0; s < new_state.state_exproductions.size(); s++)
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
				int s = Search_set(normal_family, new_go);
				if (s < normal_family.size())
				{
					new_state.edge[it] = s;
					new_state.type[it] = 3;
				}
			}
		}
		for (auto it : nonTerminalSet)
		{
			auto new_go = get_go(item_set, it);
			if (new_go.size() > 0)
			{
				int s = Search_set(normal_family, new_go);
				if (s < normal_family.size())
				{
					new_state.edge[it] = s;
					new_state.type[it] = 4;
				}
			}
		}
		parse_table.push_back(new_state);
	}
	return parse_table;
}