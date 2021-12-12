#include "Syntax.h"

void Syntax::printData(ostream&os)
{
	
	const char*enumType2Str[] = {"Syntax",
		"CharacterSet",
		"Series",
		"Parallel",
		"Repeat",
		"Border",
		"Function "};
	os << string(enumType2Str[(int)Type]) + ":\n";
	for (auto &str : DataMap) {
		os << str.first << " ";
		os << str.second;
		os << endl;
	}
}

void Syntax::pushSubTree(Syntax s)
{
	Subtree.push_back(s);
	stringstream strs;
	s.printData(strs);
	insertDataMap("SubTree"+to_string(DataMap.size()), strs.str());
}

void Syntax::printSubTree(ostream& os,int Tab)
{
	for (auto i : Subtree) {
		for (auto t = 0; t < Tab; ++t) {
			cout << "|   ";
		}
		i.printSubTree(os, Tab + 1);
		if (!i.Subtree.empty()) {
			i.printSubTree(os,Tab + 1);
		}
	}
}

void Syntax::insertDataMap(string key, string value)
{
	DataMap[key]=value;
}

bool operator==(const Syntax &ls,const Syntax &rs)
{
	return ls.Type == rs.Type &&
		ls.DataMap == rs.DataMap &&
		ls.Subtree == rs.Subtree;
}

bool operator!=(const Syntax& ls, const Syntax& rs)
{
	return !(ls==rs);
}

bool operator==(const Syntax& ls, int null)
{
	if (null == NULL) {
		return ls.DataMap.empty() && ls.Subtree.empty();
	}
	return false;
}

bool operator!=(const Syntax& ls, int null)
{
	return !(ls == null);
}

bool operator==(const Syntax& s, bool b)
{
	return s==NULL;
}

