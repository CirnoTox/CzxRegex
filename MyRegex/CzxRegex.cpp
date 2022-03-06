#include "CzxRegex.h"


Result CzxRegex::getCharacterSet()
{
    //保存位置
    auto Read = itPattern;
    if (Read == itPatternEnd)return Result{};
    //字符列表
    if (Is(Read,"[")) {
        if (Read == itPatternEnd) {//已识别到末尾，无字符与转义字符匹配匹配
            return Result{};
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
                return Result{};
            }
            //如果在字符列表中读到了转义符，却没有读到唯二的相应转义字符，那么异常
            if (Is(Read, "\\")) {
                if (!Is(Read, "]")&& !Is(Read, "^")) {
                    return Result{};
                }
                continue;
            }
            Read++;
        }
        //字符列表没有匹配的右括号，异常
        if (!rangeEnd) {
            return Result{};
        }
        auto len = Read - itPattern;
        itPattern = Read;
        auto sptrCS = make_shared<CharacterSet>(string(Read - len, Read));
        if (opposite)sptrCS.get()->Oppsite = true;
        return Result{ true,ClassType::CharacterSet,sptrCS };
    }

    //字符集合
    if (Is(Read,"{")){
        if (Read == itPatternEnd) {//已识别到末尾，无字符与转义字符匹配匹配
            return Result();
        }
        auto rangeEnd=false;
        for (;Read != itPatternEnd; Read++) {
            //如果在字符列表中读到了转义符，却没有读到唯一的相应转义字符“}”，那么异常
            if (Is(Read, "\\")&&!Is(Read, "}")) {
                return Result();
            }
            //读到"}"正常结束
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

    //如果在句首读到了转义字符反斜杠
    //匹配单个转义字符
    if (Is(Read,"\\")) {
        string escape{ "^?[]\\{}()" };
        if (Read == itPatternEnd) {//已识别到末尾，无字符与转义字符匹配匹配
            return Result();
        }
        if (escape.find(*Read) ==string::npos) {//读到不需要转义的字符
            //是s,S,w,W,d,D,t就可以匹配
            vector<string>vs{ "s","S","w","W","d","D","t" };
            for (auto ch : vs) {
                if (Is(Read, ch) ) {
                    itPattern = Read;
                    auto sptrCS = make_shared<CharacterSet>("\\" + string(Read - 1, Read));
                    return Result{ true,ClassType::CharacterSet,sptrCS };
                }
            }
            //否则没有匹配项，异常
            //itPattern = Read;
            return Result{};
        }
        //正常匹配需要转义的字符
        Read++;
        itPattern = Read;
        auto sptrCS = make_shared<CharacterSet>(string(Read - 2, Read));
        return Result{true,ClassType::CharacterSet,sptrCS};
    }

    //匹配单字，如果是需要转义的字符，就直接返回说明不是CharacterSet
    string escape{ "^?]}()*$|+" };// "{,],/" 在前面已经处理过了
    if (escape.find(*Read) != string::npos) {
        return Result{"Get an escape-needed CharacterSet"};
    }
    //正常匹配一个字符
    itPattern = Read+1;
    auto sptrCS = make_shared<CharacterSet>(string(itPattern - 1, itPattern));
    return Result{ true,ClassType::CharacterSet,sptrCS };
}

//matchParen返回从itPattern开始之后位置的第一对括号
auto CzxRegex::matchParen()
{
    if (*itPattern != '(')return make_tuple(itPattern,itPattern);
    auto Read = itPattern;
    auto beg = itPattern;
    auto count = 1;
    auto firstParen=find(itPattern, itPatternEnd, '(');
    if (firstParen == itPatternEnd)return make_tuple(itPattern, itPattern);
    for (beg=firstParen+1; count != 0 && beg != itPatternEnd; beg++) {//结束后指向最远匹配的括号的下一个字符
        if (*beg == '(')
            count++;
        else if (*beg == ')')
            count--;
        else;
        if (count == 0)return make_tuple(firstParen, beg);
    }
    //if (count != 0) {
    //    //没匹配完整
    //    itPattern = Read;
    //    return make_tuple(itPattern, itPattern);
    //}
    return make_tuple(itPattern, itPattern);
}

Result CzxRegex::getRepeat()
{
    Repeat re;
    Result getResult;
    auto Read = itPattern;

    getResult = getCharacterSet();
    if (!getResult.ifMatch) {
        auto paren = matchParen();
        if (get<0>(paren) == get<1>(paren))return Result{ false,ClassType::Syntax,nullptr };
        itPattern++;
        getResult = getSeries();
        itPattern++;
    }
    if (getResult.ifMatch) {
        auto tResult = getResult.tResult;
        auto type = get<ClassType>(tResult);
        if (type == ClassType::CharacterSet) {
            re.setContent(tResult);
        }
        if (Is(itPattern, "*")) {
            //TODO:insertDataMap 可优化
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
                        itPattern = i + 1;//读取完毕，越过
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
        else {
            //是CharacterSet
            return Result{ true,tResult };
        }
    }
    if (!getResult.ifMatch) {
        return getResult;
    }
    //TODO:测试没获取到的情况
    //TODO:Repeat其他的情况
    if (itPattern==Read) {
        //没捕获到任何Repeat情况
        return Result{ false,ClassType::Syntax,nullptr };
    }
    auto sptrRepeat = make_shared<Repeat>(re);
    return Result{true,ClassType::Repeat,sptrRepeat };
}

//TODO:Fix bugs
Result CzxRegex::getSeries()
{
    Series se;
    auto Read = itPattern;
    size_t seriesCount = 0;

    
    for (; itPattern < itPatternEnd;) {
        if (auto getResult = getRepeat(); getResult.ifMatch == true) {
            se.content.push_back(getResult.tResult);
            seriesCount++;
        }
        else if (auto getResult = getCharacterSet(); getResult.ifMatch == true) {
            se.content.push_back(getResult.tResult);
            seriesCount++;
        }
        else if (auto paren = matchParen(); get<0>(paren) != get<1>(paren)) {
            if (auto getResult = getSeries(); getResult.ifMatch == true) {
                se.content.push_back(getResult.tResult);
                seriesCount++;
            }
            else if (auto getResult = getParallel(); getResult.ifMatch == true) {
                se.content.push_back(getResult.tResult);
                seriesCount++;
            }
        }
        else {
            break;
        }
    }
    if (seriesCount == 0) {
        return Result{ "Series matched null." };
    }
    if (seriesCount == 1) {
        return Result{ true,se.content[0] };
    }
    return Result{ true,ClassType::Series,make_shared<Series>(se) };


    //auto seriesEnd = get<1>(matchParen());//将指向最远匹配的括号
    //auto seriesCount = 0;
    //if (itPattern == seriesEnd) {
    //    //相等时循环获取到结束
    //    Result getResult{true,ClassType::Series,nullptr};
    //    //for循环中如果上一次循环match到了就继续match
    //    for (;itPattern<itPatternEnd&& getResult.ifMatch == true; ) {
    //        if (getResult = getRepeat(); getResult.ifMatch == true) {
    //            se.content.push_back(getResult.tResult);
    //            seriesCount++;
    //            continue;
    //        }
    //        if (getResult = getCharacterSet(); getResult.ifMatch == true) {
    //            se.content.push_back(getResult.tResult); 
    //            seriesCount++;
    //        }
    //    }
    //    //无括号情况应该匹配到结束，itPattern!=itPatternEnd则异常
    //    if (itPattern != itPatternEnd) {
    //        return Result{ string("") + "Invalid Syntax. Pos: " + *itPattern };
    //    }
    //}
    //else 
    //{
    //    for (; itPattern < seriesEnd; seriesCount++) {//不相等时循环获取到seriesEnd
    //        if (auto getResult = getRepeat(); getResult.ifMatch == true) {
    //            se.content.push_back(getResult.tResult);
    //            continue;
    //        }
    //        if (auto getResult = getCharacterSet(); getResult.ifMatch == true) {
    //            se.content.push_back(getResult.tResult);
    //        }
    //    }
    //    //itPattern将指向最远匹配的括号
    //    itPattern++;//越过括号
    //}
    //if (seriesCount == 0) {
    //    return Result{"Series matched null."};
    //}
    //if (seriesCount == 1) {
    //    return Result{ true,se.content[0] };
    //}
    //return Result{true,ClassType::Series,make_shared<Series>(se)};
}


//TODO:实现getParallel
Result CzxRegex::getParallel()
{
    Parallel se;
    auto Read = itPattern;
    size_t parallelCount = 0;


    for (; itPattern < itPatternEnd;) {
        if (auto getResult = getRepeat(); getResult.ifMatch == true) {
            se.content.push_back(getResult.tResult);
            parallelCount++;
        }
        else if (auto getResult = getCharacterSet(); getResult.ifMatch == true) {
            se.content.push_back(getResult.tResult);
            parallelCount++;
        }
        else if (auto paren = matchParen(); get<0>(paren) != get<1>(paren)) {
            if (auto getResult = getSeries(); getResult.ifMatch == true) {
                se.content.push_back(getResult.tResult);
                parallelCount++;
            }
            else if (auto getResult = getParallel(); getResult.ifMatch == true) {
                se.content.push_back(getResult.tResult);
                parallelCount++;
            }
        }
        else {
            break;
        }
        if (Is(itPattern, "|")) {
            continue;
        }
        break;
    }
    if (parallelCount == 0) {
        return Result{ "Series matched null." };
    }
    if (parallelCount == 1) {
        return Result{ true,se.content[0] };
    }
    return Result{ true,ClassType::Parallel,make_shared<Parallel>(se) };
}




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
//        {//处理func Datamap
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
//                //命名检查单独处理
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
//            //匿名捕获
//
//            break;
//        }
//        
//        //处理正则表达式并捕获
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
