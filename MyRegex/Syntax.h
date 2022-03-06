#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<map>
#include"Error.h"
using namespace std;
enum class ClassType {
	Null,
	Syntax,
	CharacterSet,
	Series,
	Parallel,
	Repeat,
	Border,
	Function
};
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
	virtual void pushSubTree(ClassType ct, shared_ptr<Syntax> s);
	virtual void printSubTree(ostream& os,int Tab = 1);
	operator bool() const { return *this != NULL; }
protected:
	vector<tuple<ClassType,shared_ptr<Syntax>>>subTree;
	ClassType Type = ClassType::Syntax;
};

class CharacterSet :public Syntax
{
public:
	CharacterSet() :Syntax() { Type = ClassType::CharacterSet; }
	CharacterSet(string _charList) { 
		Type = ClassType::CharacterSet; 
		CharList = _charList;
	}
	string CharList="";
	bool Oppsite = false;
};

class Series :public Syntax
{
public:
	Series() :Syntax() { Type = ClassType::Series; }
	vector<tuple<ClassType, shared_ptr<Syntax>>> content;

};

class Parallel :public Syntax
{
public:
	Parallel() :Syntax() { Type = ClassType::Parallel; }
	vector<tuple<ClassType, shared_ptr<Syntax>>> content;
	//Parallel(vector<tuple<shared_ptr<void>, ClassType>> _subExp) :subExp(_subExp) {}
	//vector<tuple<shared_ptr<void>, ClassType>> subExp;
};

class Repeat :public Syntax
{
public:
	Repeat() :Syntax() { Type = ClassType::Repeat;}
	void setRepeatType(int minRepeatTimes, int maxRepeatTimes, int ifUnbounded);
	void setContent(tuple<ClassType, shared_ptr<Syntax>>content);
	tuple<ClassType, shared_ptr<Syntax>>content;
	int minRepeatTimes=0;
	int maxRepeatTimes=0;//-1 stand for unbounded
	bool ifUnbounded=false;
	/*
	Repeat(ClassType _subExpType, shared_ptr<void> sPtr,
		int _minRepeatTimes, int _maxRepeatTimes, bool _ifUnbounded) {
		Type = ClassType::Repeat;
		subExp = {sPtr,_subExpType};
		minRepeatTimes = _minRepeatTimes;
		maxRepeatTimes = _maxRepeatTimes;
		ifUnbounded = _ifUnbounded;
	}
	tuple<shared_ptr<void>, ClassType> subExp;
	*/
};


// Border class

//class Border:public Syntax
//{
//public:
//	Border() :Syntax() { Type = ClassType::Border; }
//protected:
//	string LeftOrRight = "";
//};
//
//class LeftBorder :Border
//{
//public:
//	LeftBorder() :Border() { LeftOrRight = "Left"; }
//};
//
//class RightBorder :Border
//{
//public:
//	RightBorder() :Border() { LeftOrRight = "Right"; }
//};



// Function Class
//enum class FunctionType;
/*
class Function :Syntax
{
public:
	Function() :Syntax() { Type = ClassType::Function; }
	Function(FunctionType _funcType, FunctionDescribe _funcDiscribe, string _name) :
		funcType(_funcType), funcDiscribe(_funcDiscribe),name(_name) { Type = ClassType::Function; }
	enum class FunctionType
	{
		FrontPreCheck,
		BackCPreheck,
		AnonymousCapture,
		Normal
	};
	enum class FunctionDescribe
	{
		NamedCapture,
		NamedCheak,
		Null
	};
	FunctionType funcType;
	FunctionDescribe funcDiscribe;
	string name;
};
*/

