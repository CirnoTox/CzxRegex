#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<map>
#include"Error.h"
using namespace std;
/*
* 判断过程中
	判断其类型，并new其类型
*	
*/
class Syntax
{
	friend bool operator==(const Syntax& ls, const Syntax& rs);
	friend bool operator!=(const Syntax& ls, const Syntax& rs);
	friend bool operator==(const Syntax& ls, int null);
	friend bool operator!=(const Syntax& ls, int null);
	friend bool operator==(const Syntax& ls, bool b);
public:
	Syntax() = default;
	virtual void printData(ostream& os);
	virtual void pushSubTree(Syntax s);
	virtual void printSubTree(ostream& os,int Tab = 1);
	virtual void insertDataMap(string key, string value);
	operator bool() const { return *this != NULL; }

protected:
	enum class ClassType {
		NUL,
		Syntax,
		CharacterSet,
		Series,
		Parallel,
		Repeat,
		Border,
		Function
	};
	ClassType Type = ClassType::Syntax;
	//TODO: DataMap全部换成枚举类
	//map<string, string>DataMap{};
	//vector<Syntax>Subtree{};
};

class CharacterSet :public Syntax
{
public:
	CharacterSet() :Syntax() { Type = ClassType::CharacterSet; }
	CharacterSet(string _charList) { 
		Type = ClassType::CharacterSet; 
		CharList = _charList;
	}
	string CharList;
};

class Series :public Syntax
{
public:
	Series() :Syntax() { Type = ClassType::Series; }
	Series(vector<tuple<shared_ptr<void>, ClassType>> _subExp) :subExp(_subExp) {}
	vector<tuple<shared_ptr<void>, ClassType>> subExp;

};

class Parallel :public Syntax
{
public:
	Parallel() :Syntax() { Type = ClassType::Parallel; }
	Parallel(vector<tuple<shared_ptr<void>, ClassType>> _subExp) :subExp(_subExp) {}
	vector<tuple<shared_ptr<void>, ClassType>> subExp;
};

class Repeat :protected Syntax
{
public:
	Repeat() :Syntax() { Type = ClassType::Repeat;}
	Repeat(ClassType _subExpType, shared_ptr<void> sPtr,
		int _minRepeatTimes, int _maxRepeatTimes, bool _ifUnbounded) {
		Type = ClassType::Repeat;
		subExp = {sPtr,_subExpType};
		minRepeatTimes = _minRepeatTimes;
		maxRepeatTimes = _maxRepeatTimes;
		ifUnbounded = _ifUnbounded;
	}
	tuple<shared_ptr<void>, ClassType> subExp;
	int minRepeatTimes=0;
	int maxRepeatTimes=0;
	bool ifUnbounded=false;
};

class Border:public Syntax
{
public:
	Border() :Syntax() { Type = ClassType::Border; }
protected:
	string LeftOrRight = "";
};

class LeftBorder :Border
{
public:
	LeftBorder() :Border() { LeftOrRight = "Left"; }
};

class RightBorder :Border
{
public:
	RightBorder() :Border() { LeftOrRight = "Right"; }
};

class Function :Syntax
{
public:
	Function() :Syntax() { Type = ClassType::Function; }
	Function(string _func, string _discribe, string _name) :func(_func),discribe(_discribe),name(_name) {}
	string func;
	string discribe;
	string name;
};

