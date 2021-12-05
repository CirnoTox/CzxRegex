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
    for (auto i = 0; i < 2; ++i) {
        if (get=getRepeat()) {//��getRepeat
            se.pushSubTree(get);
        }
        else if (get = getCharacterSet()) {
            se.pushSubTree(get);
        }
        else if (Is(itPattern, "(") && ((get = getSeries()) || (get = getParallel()))) {
            se.pushSubTree(get);
        }
        else {
            errorLog << to_string(itPattern - itPatternBegin);
            errorLog<< "GetSeries Error!!!\t Error Position:";
            itPattern = Read;//����ʱ�޸�
            return Series();
        }
    }

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
    //�����ȡ�� CharacterSet
    if (get = getCharacterSet()) {
        re.pushSubTree(get);
        if (Is(itPattern, "*")) {
            re.insertDataMap("Repeat_Times:",">=0");
        }
        else if (Is(itPattern, "+")) {
            re.insertDataMap("Repeat_Times:", ">=1");
        }
        else {
            itPattern = Read;//����ʱ�޸�
            errorLog << "GetRepeats  operation error!\n";
            return Repeat();
        }
        return re;
    }
    //TODO:����û��ȡ�������
    //û��ȡ��
    if (Is(itPattern, "(")&&((get = getSeries()) || (get = getParallel()))) {
        re.pushSubTree(get);
        if (Is(itPattern, "*")) {
            re.insertDataMap("Repeat_Times:", ">=0");
        }
        else if (Is(itPattern, "+")) {
            re.insertDataMap("Repeat_Times:", ">=1");
        }
        else {
            itPattern = Read;//����ʱ�޸�
            errorLog << "GetRepeats  operation error!\n";
            return Repeat();
        }
        return re;
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
