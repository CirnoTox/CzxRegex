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
				"a","B","1","_","[125]" ,"\\w","[^123]","[^(]","[^\\^]" ,"\\(","\\\\","\\?",
				"(",//�����������Զ�������˵������CS
				"^","*","$", "|", "+",//ͬ��
				"[^12^]","\\y","["
			};
			vector<string>vErrorCS;
			vector<string>vErrorCSNeeded{ 
				"(","^","*","$", "|", "+","[^12^]","\\y","["
			};
			for (auto& i : vCS) {
				CzxRegex re{ i };
				auto getre = re.getCharacterSet();
				//auto get = re.getCharacterSet().vsptrSyntax[0].get();
				//auto cast = static_cast<CharacterSet*>(get);
				if (!getre.ifMatch) {
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

		TEST_METHOD(Test_Repeat_CharacterSet)
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
			for (auto& i : vRepeat) {
				CzxRegex re{ i };
				auto get = re.getRepeat();
				if (!get.ifMatch) {
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
		TEST_METHOD(Test_Repeat_Series)
		{
			vector<string>v{
				//��ʶ���
				"(a+s\\d)*",
				//Ӧ���޷�ʶ�������
				
			};
			vector<string>vError;
			vector<string>vErrorNeeded{};
			for (auto& i : v) {
				CzxRegex re{ i };
				auto get = re.getRepeat();
				if (!get.ifMatch) {
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

		TEST_METHOD(Test_Series)
		{
			vector<string>v{
				//��ʶ���
				"aB","\\w\\D","\\^[a-o2_9_]",
				"a+sd\\s+au*iofjwel","(a+s\\d)",
				//Ӧ���޷�ʶ�������
				"(",//ֻ�����������Զ�������˵������Series
				"^","*","$", "|", "+",//ͬ��
				"[^12^]","\\y","[",//Ӧ����characterSet�ı���
				"(abc","ajxi)",//��������ƥ��
			};
			vector<string>vError;
			vector<string>vErrorNeeded{ "(","^","*","$", "|", "+","[^12^]","\\y","[","(abc","ajxi)" };
			for (auto& i : v) {
				CzxRegex re{ i };
				auto get = re.getSeries();
				if (!get.ifMatch) {
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

		/*TEST_METHOD(Test_Parallel)
		{

		}*/
	};
}
