// MyRegex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<sstream>
#include"Syntax.h"
#include"CzxRegex.h"
int main()
{
	string s = "avc";
	int b = 5;
	switch (b)
	{
		++b;
	case 5:
		cout << "5" << endl;
		
	case 6:
		cout << "6" << endl;

	case 999:
		cout << 999 << endl;
	default:
		++b;
		cout << "b:" << b << endl;
		break;
	}
	return 0;

}
