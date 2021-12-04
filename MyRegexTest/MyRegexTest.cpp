#include "pch.h"
#include "CppUnitTest.h"
#include "../MyRegex/CzxRegex.h"
#include "../MyRegex/CzxRegex.cpp"
#include "../MyRegex/Syntax.h"
#include "../MyRegex/Syntax.cpp"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MyRegexTest
{
    TEST_CLASS(MyRegexTest)
    {
    public:
        //TODO:写Test_GetCharacterSet
        //TODO:明确正则表达式中的所有功能符号
        TEST_METHOD(Test_GetCharacterSet)
        {
            CzxRegex re{ "{s\\}}" };
            auto get = re.getCharacterSet();
            CharacterSet cs("{ s\\} }");
            Assert::IsTrue(get==cs);  
        }
        /*
        TEST_CLASS(MyRegexTest)
	    {
	    public:

            TEST_METHOD(TestMethod11) 
            {
                Syntax s;
                Series ser1;
                Series ser2;
                Series ser3;
                CharacterSet cs;
                ser1.pushSubTree(ser3);
                ser1.pushSubTree(ser2);
                ser1.pushSubTree(cs);
                s.pushSubTree(ser1);
                s.pushSubTree(cs);
                s.pushSubTree(ser2);
                s.printData();
                s.printSubTree();
            }
            TEST_METHOD(TestMethod12)
		    {
                string pattern = "\((? : <#markup>\w + )(\s + (? : <#attribute>\w + ) = (? : <#value>”[^ ”] + ”)) *\)";
                CzxRegex re{ pattern };
                auto sy = re.BuildPatternTree();
                sy.printData();
                sy.printSubTree();
		    }
	    };
        */
    };
}
