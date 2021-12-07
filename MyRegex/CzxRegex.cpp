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
        for (; Read != itPatternEnd;) {
            //读到右方括号，正常结束
            if (Is(Read, "]")) {
                rangeEnd = true;
                break;
            }
            //读到没有转义的 ^
            if (Is(Read, "^")) {
                errorLog << "GetCharacterSet ERROR!Need for \"\\\" to match the \" ^ \"";
                return CharacterSet();
            }
            //如果在字符列表中读到了转义符，却没有读到唯二的相应转义字符，那么异常
            if (Is(Read, "\\")) {
                if (!Is(Read, "]")&& !Is(Read, "^")) {
                    errorLog << "GetCharacterSet ERROR!Need for char to match the \" \\ \"";
                    return CharacterSet();
                }
                continue;
            }
            Read++;
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
            //是s,S,w,W,d,D,t就可以匹配
            vector<string>vs{ "s","S","w","W","d","D","t" };
            for (auto ch : vs) {
                if (Is(Read, ch) ) {
                itPattern = Read;
                    return CharacterSet("\\" + string(Read - 1, Read));
                }
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
    string escape{ "^?]}()*$|+" };// "{,],/" 在前面已经处理过了
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
    while ((get = getRepeat()) || (get = getCharacterSet())) {
        //先getRepeat
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
    if(itPattern == Read)return Series();//没有匹配到时
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
    
    //TODO:测试没获取到的情况
    if ((get = getCharacterSet())||//获取到 CharacterSet,获取到 CharacterSet
        Is(itPattern, "(") && ((get = getSeries()) || (get = getParallel())) && Is(itPattern, ")")
        ) {
        re.pushSubTree(get);
        if (Is(itPattern, "*")) {
            //TODO:insertDataMap 可优化
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
                        itPattern = i+1;//读取完毕，越过
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
        else {//匹配到不是Repeat,不是Series或Parallel
            itPattern = Read;
            return Repeat();
        }
        return re;
    }

    if (false) {
        //TODO:匹配捕获的情况
        ;
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
    auto Read = itPattern;
    auto funcStart=Is(itPattern, "(") && Is(itPattern, "?");
    Function func;
    if (funcStart) {
        switch (*itPattern)
        {//处理func Datamap
            itPattern++;
        case '!':
            ;
            break;
        case '=':
            ;
            break;
        case ':':
            if (Is(itPattern, "<$")) {
                //命名检查单独处理
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
            //匿名捕获

            break;
        }
        
        //处理正则表达式并捕获
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
