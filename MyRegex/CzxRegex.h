#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"Syntax.h"
using namespace std;
class CzxRegex
{
public:
	CzxRegex() = default;
	CzxRegex(string _pattern);
	CzxRegex(string _pattern, string _matchStr);
	bool RunRegex() {return false;}
	
	//TODO:使用递归下降法实现BuildPatternTree
	Syntax BuildPatternTree() {

		return Syntax();
	}

	string pattern;
	string::iterator itPattern;
	string::iterator itPatternBegin;
	string::iterator itPatternEnd;
	
	string matchStr;

	class Result
	{
	public:
		Result()=default;
		~Result()=default;

		bool eMatchPattern = false;
		bool eMatchStr = false;
		size_t ePos = 0;
		string eMassage = "";
	private:

	};
	Result result;
	Error errorLog;

//private:
	bool Is(string::iterator& it,string Text);
	CharacterSet getCharacterSet();
	Series getSeries();
	Parallel getParallel();
	Repeat getRepeat();
	LeftBorder getLeftBorder();
	RightBorder getRightBorder();
	Function getFunction();
	//智能指针 一个Syntax 识别完返回 
	Syntax* sptr=nullptr;


	string alphabet{
	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
	string operation{ "!\"#$%&'()*+,-./:;<=>?@_`{|}~" };
	string space{ " " };// /s
	string letter{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
};

