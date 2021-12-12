// MyRegex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<sstream>
#include<vector>
#include<memory>
#include<tuple>
using namespace std;
//class visitor;
//class classA {
//public:
//	classA() = default;
//	classA(visitor _v) :v(_v) {}
//	string data = "A";
//	visitor v;
//};
//class classB {
//public:
//	classB(visitor _v) :v(_v) {}
//	string data = "B";
//	visitor v;
//
//};
//class classC {
//public:
//	classC(visitor _v) :v(_v) {}
//	string data = "C";
//	visitor v;
//};
//class visitor {
//public:
//	auto changeData() {
//
//	}
//};

enum class switchType {
	Int, Double, string, vector
};

class subTree {
public:
	vector<tuple<shared_ptr<void>,switchType>> subTree;
};
int main()
{
	/*shared_ptr<void> spVoid= make_shared<double>(3.2);
	double* iptr = static_cast<double*>(spVoid.get());
	cout << *iptr << endl;*/
	//shared_ptr<int> spInt=spVoid;
	subTree s;
	s.subTree.push_back({ make_shared<int>(1),switchType::Int });
	s.subTree.push_back({ make_shared<double>(1.1),switchType::Double });
	s.subTree.push_back({ make_shared<string>("111"),switchType::string });
	for (auto i : s.subTree) {
		auto type = get<1>(i);
		auto vptr = get<shared_ptr<void>>(i);

		switch (type)
		{
		case switchType::Double:
		{
			double* iptr = static_cast<double*>(vptr.get());
			cout << *iptr << endl;
			break;
		}
		case switchType::Int:
		{
			int* iptr = static_cast<int*>(vptr.get());
			cout << *iptr << endl;
			break;
		}
		case switchType::string:
		{
			string* iptr = static_cast<string*>(vptr.get());
			cout << *iptr << endl;
			break;
		}
		default:
			break;
		}
	}

}

