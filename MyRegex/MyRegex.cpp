// MyRegex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<sstream>
#include"Syntax.h"
#include"CzxRegex.h"
/*
* 每个Syntax及其子类都要维护一个 DataMap 字典，来存储其数据
* 

*/

int main()
{
    vector<string>vCS{ "[^\\^]" };
    for (auto& i : vCS) {
        CzxRegex re{ i };
        auto get = re.getCharacterSet();
        auto cs = CharacterSet(i);
        cout << (get == cs) << endl;
    }
    return 0;

}
