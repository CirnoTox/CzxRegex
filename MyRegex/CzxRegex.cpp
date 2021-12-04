#include "CzxRegex.h"


CharacterSet CzxRegex::getCharacterSet()
{
    //保存位置
    auto Read = itPattern;
    if (Read == itPatternEnd)return CharacterSet();
    //字符列表
    if (Is(Read,"[")) {
        if (Read == itPatternEnd) {//已识别到末尾，无字符与转义字符匹配匹配
            errorLog << "GetCharacterSet ERROR!Stream End!Need for char to match the \" ] \"";
            return CharacterSet();
        }
        bool opposite = false;
        bool rangeEnd = false;
        //取反标志
        if (Is(Read, "^"))opposite = true;
        for (; Read != itPatternEnd; Read++) {
            //读到右方括号，正常结束
            if (Is(Read, "]")) {
                rangeEnd = true;
                break;
            }
            //如果在字符列表中读到了转义符，却没有读到唯二的相应转义字符，那么异常
            if (Is(Read, "\\")) {
                if (!Is(Read, "]")&& !Is(Read, "^")) {
                    errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
                    return CharacterSet();
                }
            }
        }
        //字符列表没有匹配的右括号，异常
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

    //字符集合
    if (Is(Read,"{")){
        if (Read == itPatternEnd) {//已识别到末尾，无字符与转义字符匹配匹配
            errorLog << "GetCharacterSet ERROR!Stream End!Need for char to match the \" } \"";
            return CharacterSet();
        }
        auto rangeEnd=false;
        for (;Read != itPatternEnd; Read++) {
            //如果在字符列表中读到了转义符，却没有读到唯一的相应转义字符“}”，那么异常
            if (Is(Read, "\\")&&!Is(Read, "}")) {
                errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
                return CharacterSet();
            }
            //读到"}"正常结束
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

    //如果在句首读到了转义字符反斜杠
    //匹配单个转义字符
    if (Is(Read,"\\")) {
        string escape{ "^?[]\\{}()" };
        if (Read == itPatternEnd) {//已识别到末尾，无字符与转义字符匹配匹配
            errorLog << "GetCharacterSet ERROR!Stream End!Need for char to match the \" \\ \"";
            return CharacterSet();
        }
        if (escape.find(*Read) ==string::npos) {//读到不需要转义的字符
            //是s或w就可以匹配
            if (Is(Read, "s")|| Is(Read, "w")) {
                itPattern = Read;
                return CharacterSet("\\"+string(Read-1,Read));
            }
            //否则没有匹配项，异常
            itPattern = Read;
            errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
            return CharacterSet();
        }
        //正常匹配需要转义的字符
        Read++;
        return CharacterSet(string(Read-2,Read));
    }

    //匹配单字，如果是需要转义的字符，就直接返回说明不是CharacterSet
    string escape{ "^?]}()" };// "{,],/" 在前面已经处理过了
    if (escape.find(*Read) != string::npos) {
        return CharacterSet();
    }
    //正常匹配一个字符
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
        if (get=getRepeat()) {//先getRepeat
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
            itPattern = Read;//错误时修改
            return Series();
        }
    }

    return se;
}

//TODO:实现getParallel
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
    //如果获取到 CharacterSet
    if (get = getCharacterSet()) {
        re.pushSubTree(get);
        if (Is(itPattern, "*")) {
            re.insertDataMap("Repeat_Times:",">=0");
        }
        else if (Is(itPattern, "+")) {
            re.insertDataMap("Repeat_Times:", ">=1");
        }
        else {
            itPattern = Read;//错误时修改
            errorLog << "GetRepeats  operation error!\n";
            return Repeat();
        }
        return re;
    }
    //TODO:测试没获取到的情况
    //没获取到
    if (Is(itPattern, "(")&&((get = getSeries()) || (get = getParallel()))) {
        re.pushSubTree(get);
        if (Is(itPattern, "*")) {
            re.insertDataMap("Repeat_Times:", ">=0");
        }
        else if (Is(itPattern, "+")) {
            re.insertDataMap("Repeat_Times:", ">=1");
        }
        else {
            itPattern = Read;//错误时修改
            errorLog << "GetRepeats  operation error!\n";
            return Repeat();
        }
        return re;
    }
    //Repeat 匹配失败
    itPattern = Read;//错误时修改
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

CzxRegex::CzxRegex(string _pattern) :pattern(_pattern)
{
    itPattern = pattern.begin();
    itPatternBegin = pattern.begin();
    itPatternEnd = pattern.end();
}
CzxRegex::CzxRegex(string _pattern, string _matchStr) 
    :pattern(_pattern), matchStr(_matchStr) {
    itPattern = pattern.begin();
    itPatternBegin = pattern.begin();
    itPatternEnd = pattern.end();
}
bool CzxRegex::Is(string::iterator& it,string Text)
{
    size_t len = Text.size();
    /*保存参数*/
    auto Read = it;
    /*过滤空格*/
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
