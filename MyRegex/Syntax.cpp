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
	os << string(enumType2Str[(int)Type]) << endl;
}

void Syntax::pushSubTree(ClassType ct, shared_ptr<Syntax> s)
{
	subTree.push_back(
		make_tuple(ct,s)
	);
	
}

void Syntax::printSubTree(ostream& os,int Tab)
{
	const char* enumType2Str[] = { "Syntax",
		"CharacterSet",
		"Series",
		"Parallel",
		"Repeat",
		"Border",
		"Function " };
	os << string(enumType2Str[(int)Type]) + " has sub tree:\n";
	for (auto &tree : subTree) {
		for (auto t = 0; t < Tab; ++t) {
			cout << "|   ";
		}
		auto ptr = get<shared_ptr<Syntax>>(tree).get();
		ptr->printSubTree(os, Tab + 1);
		if (!ptr->subTree.empty()) {
			ptr->printSubTree(os,Tab + 1);
		}
	}
}


bool operator==(const Syntax &ls,const Syntax &rs)
{
	return ls.Type == rs.Type &&
		ls.subTree == rs.subTree;
}

bool operator!=(const Syntax& ls, const Syntax& rs)
{
	return !(ls==rs);
}

bool operator==(const Syntax& ls, int null)
{
	if (null == NULL) {
		return ls.subTree.empty();
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

void Repeat::setRepeatType(int minRepeatTimes, int maxRepeatTimes, int ifUnbounded)
{
	this->maxRepeatTimes = maxRepeatTimes;
	this->minRepeatTimes = minRepeatTimes;
	this->ifUnbounded = ifUnbounded;
}

void Repeat::setContent(tuple<ClassType, shared_ptr<Syntax>> content)
{
	this->content = content;
}
