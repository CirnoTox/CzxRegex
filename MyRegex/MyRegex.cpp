// MyRegex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<sstream>
#include<vector>
using namespace std;
//#include"Syntax.h"
//#include"CzxRegex.h"

class base {
public:
	base() = default;
	vector<shared_ptr<void>>vv;
};
int main()
{
	base b;
	b.vv.push_back(make_shared<void>(3));
	b.vv.push_back(make_shared<void>(string("abc")));
	b.vv.push_back(make_shared<void>(vector<double>{1,3,2}));
	b;
}
