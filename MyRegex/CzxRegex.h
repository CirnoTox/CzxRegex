#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"Syntax.h"
using namespace std;
class Result
{
public:
	Result() = default;
	Result(bool match, ClassType ct,shared_ptr<Syntax>sptrS) {
		ifMatch = match;
		tResult = { ct,sptrS };
	}
	Result(bool match, tuple<ClassType, shared_ptr<Syntax>>_tResult) {
		ifMatch = match;
		tResult = _tResult;
	}
	Result(string _massage,bool match=false) {
		massage = _massage;
		ifMatch = match;
	}
	~Result() = default;

	bool ifMatch = false;
	tuple<ClassType,shared_ptr<Syntax>>tResult;
	size_t pos = 0;
	string massage = "";
};
class CzxRegex
{
public:
	CzxRegex() = default;
	CzxRegex(string _pattern);
	CzxRegex(string _pattern, string _matchStr);
	bool RunRegex() {return false;}
	
	//TODO:ʹ�õݹ��½���ʵ��BuildPatternTree
	Syntax BuildPatternTree() {

		return Syntax();
	}

	string pattern;
	string::iterator itPattern;
	string::iterator itPatternBegin;
	string::iterator itPatternEnd;
	
	string matchStr;
	Result result;
	Error errorLog;
	
//private:
	//ʹ��subTree����
	bool Is(string::iterator& it,string Text);
	auto matchParen();
	Result getCharacterSet();
	Result getRepeat();
	Result getSeries();
	Result getParallel();
	//LeftBorder getLeftBorder();
	//RightBorder getRightBorder();
	//Function getFunction();
	//����ָ�� һ��Syntax ʶ���귵�� 
	Syntax* sptr=nullptr;


	string alphabet{
	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
	string operation{ "!\"#$%&'()*+,-./:;<=>?@_`{|}~" };
	string space{ " " };// /s
	string letter{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
};

