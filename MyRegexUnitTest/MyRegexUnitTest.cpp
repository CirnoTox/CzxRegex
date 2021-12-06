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
		
		TEST_METHOD(Test_CharacterSet)
		{
			vector<string>vCS{ 
				"a","B","1","_","[125]" ,"\\w","[^123]","[^(]","[^\\^]" ,"\\(","\\\\","\\?"
				"(",//�����������Զ�������˵������CS
				"^","*","$", "|", "+",//ͬ��
				"[^12^]","\\y","["
			};
			vector<string>vErrorCS;
			vector<string>vErrorCSNeeded{ "[^12^]","\\y","[" };
			for (auto& i : vCS) {
				CzxRegex re{ i };
				auto get = re.getCharacterSet();
				if (!re.errorLog.sErrorLog.empty()) {
					vErrorCS.push_back(i);
				}
			}
			Logger::WriteMessage("�쳣CharacterSet:");
			for (auto i : vErrorCS) {
				Logger::WriteMessage(i.c_str());
				Logger::WriteMessage(",");
			}
			Assert::IsTrue(vErrorCS==vErrorCSNeeded);
		}

		TEST_METHOD(Test_Repeat)
		{
			vector<string>vRepeat{
				"a+","a*","a?",
				"[abc]*","[abc]?",
				"\\w+","\\w*","\\w?","\\??",
				"a{2,2}","a{0,}","a{3,3}",
				//Error 
				"**","[*","??","^?","*?","$?", "|?", "+?",
				"a{0}","a{2}",//������
				"a{2,1}"
			};
			vector<string>vERepeat;
			vector<string>vERepeatNeeded{
				"**","[*","??","^?","*?","$?", "|?", "+?",
				"a{0}","a{2}",//������
				"a{2,1}"
			};
			for (auto & i : vRepeat) {
				CzxRegex re{ i };
				auto get = re.getRepeat();
				if (!re.errorLog.sErrorLog.empty()) {
					vERepeat.push_back(i);
				}
			}
			Logger::WriteMessage("�쳣CharacterSet:");
			for (auto i : vERepeat) {
				Logger::WriteMessage(i.c_str());
				Logger::WriteMessage(",");
			}
			Assert::IsTrue(vERepeat == vERepeatNeeded);
		}

		TEST_METHOD(Test_Series)
		{
			vector<string>v{
				"aB","\\w\\D","\\^[a-o2_9_]"
				"(",//ֻ�����������Զ�������˵������Series
				"^","*","$", "|", "+",//ͬ��
				"[^12^]","\\y","["//Ӧ����characterSet�ı���
				"(abc","ajxi)"
			};
			vector<string>vError;
			vector<string>vErrorNeeded{ "[^12^]","\\y","[","(abc","ajxi)" };
			for (auto& i : v) {
				CzxRegex re{ i };
				auto get = re.getCharacterSet();
				auto cs = CharacterSet(i);
				if (!re.errorLog.sErrorLog.empty()) {
					vError.push_back(i);
				}
			}
			Logger::WriteMessage("�쳣Series:");
			for (auto i : vError) {
				Logger::WriteMessage(i.c_str());
				Logger::WriteMessage(",");
			}
			Assert::IsTrue(vError == vErrorNeeded);
		}
		TEST_METHOD(Test_Parallel)
		{

		}
	};
}
