#include "CzxRegex.h"


Result CzxRegex::getCharacterSet()
{
    //����λ��
    auto Read = itPattern;
    if (Read == itPatternEnd)return Result{};
    //�ַ��б�
    if (Is(Read,"[")) {
        if (Read == itPatternEnd) {//��ʶ��ĩβ�����ַ���ת���ַ�ƥ��ƥ��
            return Result{};
        }
        bool opposite = false;
        bool rangeEnd = false;
        //ȡ����־
        if (Is(Read, "^"))opposite = true;
        for (; Read != itPatternEnd;) {
            //�����ҷ����ţ���������
            if (Is(Read, "]")) {
                rangeEnd = true;
                break;
            }
            //����û��ת��� ^
            if (Is(Read, "^")) {
                return Result{};
            }
            //������ַ��б��ж�����ת�����ȴû�ж���Ψ������Ӧת���ַ�����ô�쳣
            if (Is(Read, "\\")) {
                if (!Is(Read, "]")&& !Is(Read, "^")) {
                    return Result{};
                }
                continue;
            }
            Read++;
        }
        //�ַ��б�û��ƥ��������ţ��쳣
        if (!rangeEnd) {
            return Result{};
        }
        auto len = Read - itPattern;
        itPattern = Read;
        auto sptrCS = make_shared<CharacterSet>(string(Read - len, Read));
        if (opposite)sptrCS.get()->Oppsite = true;
        return Result{ true,ClassType::CharacterSet,sptrCS };
    }

    //�ַ�����
    if (Is(Read,"{")){
        if (Read == itPatternEnd) {//��ʶ��ĩβ�����ַ���ת���ַ�ƥ��ƥ��
            return Result();
        }
        auto rangeEnd=false;
        for (;Read != itPatternEnd; Read++) {
            //������ַ��б��ж�����ת�����ȴû�ж���Ψһ����Ӧת���ַ���}������ô�쳣
            if (Is(Read, "\\")&&!Is(Read, "}")) {
                return Result();
            }
            //����"}"��������
            if (Is(Read,"}")) {
                rangeEnd = true;
                break;
            }
        }
        if (!rangeEnd) {
            return Result();
        }
        auto len = Read - itPattern;
        itPattern = Read;
        auto sptrCS=make_shared<CharacterSet>(string(Read - len, Read));
        return Result{true,ClassType::CharacterSet,sptrCS};
    }

    //����ھ��׶�����ת���ַ���б��
    //ƥ�䵥��ת���ַ�
    if (Is(Read,"\\")) {
        string escape{ "^?[]\\{}()" };
        if (Read == itPatternEnd) {//��ʶ��ĩβ�����ַ���ת���ַ�ƥ��ƥ��
            return Result();
        }
        if (escape.find(*Read) ==string::npos) {//��������Ҫת����ַ�
            //��s,S,w,W,d,D,t�Ϳ���ƥ��
            vector<string>vs{ "s","S","w","W","d","D","t" };
            for (auto ch : vs) {
                if (Is(Read, ch) ) {
                    itPattern = Read;
                    auto sptrCS = make_shared<CharacterSet>("\\" + string(Read - 1, Read));
                    return Result{ true,ClassType::CharacterSet,sptrCS };
                }
            }
            //����û��ƥ����쳣
            itPattern = Read;
            return Result{};
        }
        //����ƥ����Ҫת����ַ�
        Read++;
        itPattern = Read;
        auto sptrCS = make_shared<CharacterSet>(string(Read - 2, Read));
        return Result{true,ClassType::CharacterSet,sptrCS};
    }

    //ƥ�䵥�֣��������Ҫת����ַ�����ֱ�ӷ���˵������CharacterSet
    string escape{ "^?]}()*$|+" };// "{,],/" ��ǰ���Ѿ��������
    if (escape.find(*Read) != string::npos) {
        return Result{};
    }
    //����ƥ��һ���ַ�
    itPattern = Read+1;
    auto sptrCS = make_shared<CharacterSet>(string(itPattern - 1, itPattern));
    return Result{ true,ClassType::CharacterSet,sptrCS };
}


Result CzxRegex::getRepeat()
{

    Repeat re;
    Result getResult;
    auto Read = itPattern;

    getResult = getCharacterSet();
    if (getResult.ifMatch) {
        auto tResult = getResult.tResult;
        auto type = get<ClassType>(tResult);
        if (type == ClassType::CharacterSet) {
            re.setContent(tResult);
        }
        if (Is(itPattern, "*")) {
            //TODO:insertDataMap ���Ż�
            re.setRepeatType(0, -1, true);
        }
        else if (Is(itPattern, "+")) {
            re.setRepeatType(1, -1, true);
        }
        else if (Is(itPattern, "?")) {
            re.setRepeatType(0, 1, false);
        }
        else if (Is(itPattern, "{")) {
            auto lambGetRepeatTimes = [&]() {
                bool endGetNum = false;
                bool getComma = false;
                int pos = 0;
                vector<string>get;
                for (auto i = itPattern; i != itPatternEnd; ++i) {
                    if (*i == ',') {
                        get.push_back(string(itPattern + pos, i));
                        pos = i - itPattern + 1;
                        getComma = true;
                    }
                    if (*i == '}') {
                        get.push_back(string(itPattern + pos, i));
                        itPattern = i + 1;//��ȡ��ϣ�Խ��
                        endGetNum = true;
                    }
                }
                bool num2BiggerOrEqual = true;
                if (get.size() > 1 && !get[1].empty()) {
                    num2BiggerOrEqual = stoi(get[0]) <= stoi(get[1]) ? true : false;
                
                }
                if (get.size() > 1&& get[1] == "") {
                    get.pop_back();
                }
                tuple<vector<string>, bool>t{ get,endGetNum && getComma && num2BiggerOrEqual && !get.empty() };
                return t;
            };
            auto [nums, invalidSyntax] = lambGetRepeatTimes();
            if (!invalidSyntax) {
                getResult.massage += "\tGetRepeat Error!Invalid Syntax!";
                getResult.ifMatch = false;
                return getResult;
            }
            if (nums.size() > 1) {

                re.setRepeatType(stoi(nums[0]), stoi(nums[1]), false);
            }
            else {
                re.setRepeatType(0, 1, false);
                re.setRepeatType(stoi(nums[0]), -1, true);
            }
        }
    }
    //TODO:����û��ȡ�������
    //TODO:Repeat���������
    if (itPattern==Read) {
        //û�����κ�Repeat���
        return Result{ false,ClassType::Syntax,nullptr };
    }
    auto sptrRepeat = make_shared<Repeat>(re);
    return Result{true,ClassType::Repeat,sptrRepeat };
}



//TODO:Fix bugs

//Result CzxRegex::getSeries()
//{
//    Series se;
//    Syntax get;
//    auto Read = itPattern;
//    while (/*(get = getRepeat()) ||*/ (get = getCharacterSet())) {
//        //��getRepeat
//        se.pushSubTree(get);
//    }
//    if (Is(itPattern, "(")) {
//        bool seriesEnd=false;
//        while (((get = getSeries()) ||
//            (Is(itPattern, "|") && (get = getParallel())))) {
//            se.pushSubTree(get);
//            if (Is(itPattern, ")")) {
//                seriesEnd = true;
//                break;
//            }
//        }
//        if (!seriesEnd) {
//            errorLog << "GetSeries Error!!!Not ended Series or Parallel:";
//            return Series();
//        }
//    }
//    if(itPattern == Read)return Series();//û��ƥ�䵽ʱ
//    return se;
//}


//TODO:ʵ��getParallel
//Result CzxRegex::getParallel()
//{
//    return Parallel();
//}




//
//LeftBorder CzxRegex::getLeftBorder()
//{
//    return LeftBorder();
//}
//
//RightBorder CzxRegex::getRightBorder()
//{
//    return RightBorder();
//}

//Function CzxRegex::getFunction()
//{
//    auto Read = itPattern;
//    auto funcStart=Is(itPattern, "(") && Is(itPattern, "?");
//    Function func;
//    string funcType;
//    if (funcStart) {
//        switch (*itPattern)
//        {//����func Datamap
//            itPattern++;
//        case '!':
//            
//            funcType = "BackCPreheck";
//            break;
//        case '=':
//            funcType = "FrontPreCheck"; 
//                
//            ;
//            break;
//        case ':':
//            if (Is(itPattern, "<$")) {
//                //������鵥������
//                break;
//            }
//            else if (Is(itPattern, "<#")) {
//
//            }
//            else {
//
//            }
//            if (Is(itPattern, ">")) {
//                ;
//            }
//            break;
//        default:
//            //��������
//
//            break;
//        }
//        
//        //����������ʽ������
//    }
//    return Function();
//}

CzxRegex::CzxRegex(string _pattern)
{
    pattern = _pattern;
    itPattern = pattern.begin();
    itPatternBegin = pattern.begin();
    itPatternEnd = pattern.end();
};
CzxRegex::CzxRegex(string _pattern, string _matchStr) 
    :pattern(_pattern), matchStr(_matchStr) {
    itPattern = pattern.begin();
    itPatternBegin = pattern.begin();
    itPatternEnd = pattern.end();
}
bool CzxRegex::Is(string::iterator& it,string Text)
{
    size_t len = Text.size();
    /*�������*/
    auto Read = it;
    /*���˿ո�*/
    while (Read != itPatternEnd && *Read == ' ')Read++;
    if (Read != itPatternEnd && string(Read, Read + len) == Text)
    {
        it = Read + len;
        return true;
    }
    else
    {
        return false;
    }
}
