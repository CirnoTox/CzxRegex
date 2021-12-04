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
	void printSubTree(ostream& os,int Tab = 1);
	void insertDataMap(string key, string value);
	operator bool() const { return *this != NULL; }

protected:
	string Type = "Syntax";
	map<string, string>DataMap{};
	vector<Syntax>Subtree{};
};

class CharacterSet :public Syntax
{
public:
	CharacterSet() :Syntax() { Type = "CharacterSet"; }
	CharacterSet(string _charList) { 
		Type = "CharacterSet"; this->insertDataMap("CharList",_charList);
	}

};

class Series :public Syntax
{
public:
	Series() :Syntax() { Type = "Series"; }

};

class Parallel :public Syntax
{
public:
	Parallel() :Syntax() { Type = "Parallel"; }
};

class Repeat :public Syntax
{
public:
	Repeat() :Syntax() { Type = "Repeat"; }
	/*
	int maxTime = 0;
	int minTime = 0;
	bool unlimited = true;
	*/
};

class Border:public Syntax
{
public:
	Border() :Syntax() { Type = "Border"; }
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
	Function() :Syntax() { Type = "Function"; }
	Function(string _func, string _discribe, string _name) :func(_func),discribe(_discribe),name(_name) {}
	string func;
	string discribe;
	string name;
};

