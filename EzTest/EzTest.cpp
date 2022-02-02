// MyRegex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<sstream>
#include<vector>
#include<memory>
#include<tuple>
using namespace std;

struct A
{
	shared_ptr<A>subtree;
	shared_ptr<A>subtree2;
};
struct B:A
{
	shared_ptr<B>subtree;

};
struct C:B
{
	string str;
	auto go() { cout << "I am C" << endl; }
};

vector< unique_ptr<C> >uptrPool;
vector<shared_ptr<C>> fun()
{
	C c;
	c.str = "this is C";
	vector<shared_ptr<C>> v;
	return vector<shared_ptr<C>>{make_shared<C>(c)};
}
int main()
{
	auto getC=fun();
	getC[0]->go();
	cout << getC[0]->str;
}

