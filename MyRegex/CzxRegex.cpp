#include "CzxRegex.h"


CharacterSet CzxRegex::getCharacterSet()
{
    //����λ��
    auto Read = itPattern;
    if (Read == itPatternEnd)return CharacterSet();
    //�ַ��б�
    if (Is(Read,"[")) {
        if (Read == itPatternEnd) {//��ʶ��ĩβ�����ַ���ת���ַ�ƥ��ƥ��
            errorLog << "GetCharacterSet ERROR!Stream End!Need for char to match the \" ] \"";
            return CharacterSet();
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
                errorLog << "GetCharacterSet ERROR!Need for \"\\\" to match the \" ^ \"";
                return CharacterSet();
            }
            //������ַ��б��ж�����ת�����ȴû�ж���Ψ������Ӧת���ַ�����ô�쳣
            if (Is(Read, "\\")) {
                if (!Is(Read, "]")&& !Is(Read, "^")) {
                    errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
                    return CharacterSet();
                }
                continue;
            }
            Read++;
        }
        //�ַ��б�û��ƥ��������ţ��쳣
        if (!rangeEnd) {
            errorLog << "GetCharacterSet ERROR!None matched \" ] \"!";
            return CharacterSet();
        }
        auto len = Read - itPattern;
        itPattern = Read;
        CharacterSet resultCS(string(Read - len, Read));
        if (opposite)resultCS.insertDataMap("Oppsite", "True");
        return resultCS;
    }

    //�ַ�����
    if (Is(Read,"{")){
        if (Read == itPatternEnd) {//��ʶ��ĩβ�����ַ���ת���ַ�ƥ��ƥ��
            errorLog << "GetCharacterSet ERROR!Stream End!Need for char to match the \" } \"";
            return CharacterSet();
        }
        auto rangeEnd=false;
        for (;Read != itPatternEnd; Read++) {
            //������ַ��б��ж�����ת�����ȴû�ж���Ψһ����Ӧת���ַ���}������ô�쳣
            if (Is(Read, "\\")&&!Is(Read, "}")) {
                errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
                return CharacterSet();
            }
            //����"}"��������
            if (Is(Read,"}")) {
                rangeEnd = true;
                break;
            }
        }
        if (!rangeEnd) {
            errorLog << "GetCharacterSet ERROR!None matched \" } \"!";
            return CharacterSet();
        }
        auto len = Read - itPattern;
        itPattern = Read;
        return CharacterSet(string(Read-len,Read));
    }

    //����ھ��׶�����ת���ַ���б��
    //ƥ�䵥��ת���ַ�
    if (Is(Read,"\\")) {
        string escape{ "^?[]\\{}()" };
        if (Read == itPatternEnd) {//��ʶ��ĩβ�����ַ���ת���ַ�ƥ��ƥ��
            errorLog << "GetCharacterSet ERROR!Stream End!Need for char to match the \" \\ \"";
            return CharacterSet();
        }
        if (escape.find(*Read) ==string::npos) {//��������Ҫת����ַ�
            //��s,S,w,W,d,D,t�Ϳ���ƥ��
            vector<string>vs{ "s","S","w","W","d","D","t" };
            for (auto ch : vs) {
                if (Is(Read, ch) ) {
                itPattern = Read;
                    return CharacterSet("\\" + string(Read - 1, Read));
                }
            }
            //����û��ƥ����쳣
            itPattern = Read;
            errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
            return CharacterSet();
        }
        //����ƥ����Ҫת����ַ�
        Read++;
        return CharacterSet(string(Read-2,Read));
    }

    //ƥ�䵥�֣��������Ҫת����ַ�����ֱ�ӷ���˵������CharacterSet
    string escape{ "^?]}()*$|+" };// "{,],/" ��ǰ���Ѿ��������
    if (escape.find(*Read) != string::npos) {
        return CharacterSet();
    }
    //����ƥ��һ���ַ�
    itPattern = Read+1;
    return CharacterSet(string(itPattern-1,itPattern));
}

//TODO:Fix bugs
Series CzxRegex::getSeries()
{
    Series se;
    Syntax get;
    auto Read = itPattern;
    while ((get = getRepeat()) || (get = getCharacterSet())) {
        //��getRepeat
        se.pushSubTree(get);
    }
    if (Is(itPattern, "(")) {
        bool seriesEnd=false;
        while (((get = getSeries()) ||
            (Is(itPattern, "|") && (get = getParallel())))) {
            se.pushSubTree(get);
            if (Is(itPattern, ")")) {
                seriesEnd = true;
                break;
            }
        }
        if (!seriesEnd) {
            errorLog << "GetSeries Error!!!Not ended Series or Parallel:";
            return Series();
        }
    }
    if(itPattern == Read)return Series();//û��ƥ�䵽ʱ
    return se;
}

//TODO:ʵ��getParallel
Parallel CzxRegex::getParallel()
{
    return Parallel();
}

// TODO:getRepeat
Repeat CzxRegex::getRepeat()
{
    Repeat re;
    Syntax get;
    auto Read = itPattern;
    
    //TODO:����û��ȡ�������
    if ((get = getCharacterSet())||//��ȡ�� CharacterSet,��ȡ�� CharacterSet
        Is(itPattern, "(") && ((get = getSeries()) || (get = getParallel())) && Is(itPattern, ")")
        ) {
        re.pushSubTree(get);
        if (Is(itPattern, "*")) {
            //TODO:insertDataMap ���Ż�
            re.insertDataMap("0","unbounded", "true");
        }
        else if (Is(itPattern, "+")) {
            re.insertDataMap("1", "unbounded", "true");
        }
        else if (Is(itPattern, "?")) {
            re.insertDataMap( "0", "1", "false");
        }
        else if (Is(itPattern,"{")) {
            auto lambGetNum = [&]() {
                bool endGetNum = false;
                bool getComma = false;
                int pos = 0;
                vector<string>get;
                for (auto i = itPattern; i != itPatternEnd; ++i) {
                    if (*i == ',' ) {
                        get.push_back(string(itPattern + pos, i));
                        pos = i - itPattern + 1;
                        getComma = true;
                    }
                    if (*i == '}') {
                        get.push_back(string(itPattern + pos, i));
                        itPattern = i+1;//��ȡ��ϣ�Խ��
                        endGetNum = true;
                    }
                }
                bool num2BiggerOrEqual = true;
                if (get.size() > 1&& !get[1].empty()) {
                    num2BiggerOrEqual = stoi(get[0]) <= stoi(get[1])?true:false;
                }
                tuple<vector<string>, bool>t{ get,endGetNum&&getComma&& num2BiggerOrEqual&&!get.empty()};
                return t;
            };
            auto [nums, invalidSyntax] = lambGetNum();
            if (!invalidSyntax) {
                errorLog << "GetRepeat Error!Invalid Syntax!";
                return Repeat();
            }
            if (nums.size()>1) {
                re.insertDataMap(nums[0], nums[1],"false");
            }
            else {
                re.insertDataMap(nums[0], "unbounded", "true");
            }

        }
        else {//ƥ�䵽����Repeat,����Series��Parallel
            itPattern = Read;
            return Repeat();
        }
        return re;
    }

    if (false) {
        //TODO:ƥ�䲶������
        ;
    }

    //Repeat ƥ��ʧ��
    itPattern = Read;//����ʱ�޸�
    errorLog << "GetRepeat first char match error!\n";
    return Repeat();
}

LeftBorder CzxRegex::getLeftBorder()
{
    return LeftBorder();
}

RightBorder CzxRegex::getRightBorder()
{
    return RightBorder();
}

Function CzxRegex::getFunction()
{
    auto Read = itPattern;
    auto funcStart=Is(itPattern, "(") && Is(itPattern, "?");
    Function func;
    if (funcStart) {
        switch (*itPattern)
        {//����func Datamap
            itPattern++;
        case '!':
            ;
            break;
        case '=':
            ;
            break;
        case ':':
            if (Is(itPattern, "<$")) {
                //������鵥������
                break;
            }
            else if (Is(itPattern, "<#")) {

            }
            else {

            }
            if (Is(itPattern, ">")) {
                ;
            }
            break;
        default:
            //��������

            break;
        }
        
        //����������ʽ������
    }
    return Function();
}

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
