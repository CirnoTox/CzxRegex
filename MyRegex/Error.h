#pragma once
#include<stack>
#include<string>
#include<sstream>
using namespace std;
class Error
{
	friend istream& operator>>(istream& is, Error& e);
	friend ostream& operator<<(ostream& os, Error& e);
	friend Error& operator<<(Error& e,string str);
public:
	Error() = default;
	~Error() = default;
	stack<string> sErrorLog;
};

