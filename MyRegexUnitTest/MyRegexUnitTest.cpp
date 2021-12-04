#include "pch.h"
#include "CppUnitTest.h"
#include"../MyRegex/CzxRegex.h"
#include"../MyRegex/CzxRegex.cpp"
#include"../MyRegex/Syntax.h"
#include"../MyRegex/Syntax.cpp"
#include"../MyRegex/Error.h"
#include"../MyRegex/Error.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MyRegexUnitTest
{
	TEST_CLASS(MyRegexUnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			vector<string>vCS{ 
				"a","B","1","_","[125]" ,"\\w","[^123]","[^(]","[^\\^]" ,"\\(","\\\\",
				"(",//读到左括号自动结束，说明不是CS
				"^","*","$", "|", "+",//同上
				"[^12^]","\\y","["
			};
			vector<string>vErrorCS;
			vector<string>vErrorCSNeeded{ "[^12^]","\\y","[" };
			for (auto& i : vCS) {
				CzxRegex re{ i };
				auto get = re.getCharacterSet();
				auto cs = CharacterSet(i);
				if (!re.errorLog.sErrorLog.empty()) {
					vErrorCS.push_back(i);
				}
			}
			Logger::WriteMessage("异常CharacterSet:");
			for (auto i : vErrorCS) {
				Logger::WriteMessage(i.c_str());
				Logger::WriteMessage(",");
			}
			Assert::IsTrue(vErrorCS==vErrorCSNeeded);
		}
	};
}
