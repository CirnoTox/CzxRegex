- [简介](#简介)
- [文件组织](#文件组织)
  - [Syntax](#syntax)
    - [类](#类)
    - [判断优先级](#判断优先级)
    - [字符集合](#字符集合)
    - [并联(Alternation)串联(Concatenation)](#并联alternation串联concatenation)
    - [重复(Repetition)](#重复repetition)
    - [表达式引用(Expression reference)](#表达式引用expression-reference)
    - [正向预查](#正向预查)
    - [反向预查](#反向预查)
    - [匿名捕获](#匿名捕获)
    - [命名捕获](#命名捕获)
    - [命名检查](#命名检查)
    - [边界](#边界)
    - [非贪婪重复](#非贪婪重复)
  - [CzxRegex](#czxregex)
  - [Error](#error)



# 简介
项目进程
1. 正则表达式的语法分析 即将完成
   
TODO：根据纯正则表达式生成FA

TODO：扩展正则表达式生成NFA

TODO：NFA压缩

TODO：贪婪匹配

TODO：测试与优化

# 文件组织
1. Syntax：表达式语法定义
2. CzxRegex：正则表达式
3. Error：异常处理

## Syntax

### 类
Syntax为基类，其余类都为Syntax的派生类。
Syntax类有成员：
1. DataMap，用来存储属性值
2. SubTree，用来存储子树
3. Type，用来表示本对象类型
有函数：用来处理对象属性
1. virtual void printData(ostream& os);
2. virtual void pushSubTree(Syntax s);
3. void printSubTree(ostream& os,int Tab = 1);
4. void insertDataMap(string key, string value);

### 判断优先级
优先并联然后串联

### 字符集合
|  内容   | 含义  |
|  ----  | ----  |
| A  | "A" |
| [a-z]  | lowercase letters |
|[a-zA-Z0-9_]|lowercase letters, uppercase letters, digits and dash|
|[^a-zA-Z]|not letters|
|`\w`|[a-zA-Z0-9_]|
|`\W`|[^a-zA-Z0-9_]|
|`\s`|space|
|`\S`|[^ ] not space|
|`\d`|[0-9]|
|`\D`|[^0-9]|
|`\t`|tab|
|`\(`|left parentheses|
1. Character range:例如`[a-z]`，"-"号两边的字符完全依照ASCII码顺序；
2. DSW character escape：例如`\S`，合法的DSW character escape在表格中已全部列出；
3. Identity escape：含有特殊含义的字符前要加上`\\`,它们包括：
{ `(`, `)`, `{`, `}`, `.`, `[`, `\`, `*`, `^`, `$`, `+`, `?`, `|` }
4. Character：单个字符

### 并联(Alternation)串联(Concatenation)
* 并联优先级高于串联
|  内容   | 含义  |
|  ----  | ----  |
|aC|串联，"aC"|
|[0-9][0-9][0-9]|串联，三个数字构成的字符串|
|\d\d\|a|串联，两个数字或一个数字和字母a|
|(a\|b)(c\|d)|串并联，字符集ac|ad|bc|bd|


### 重复(Repetition)
|  内容   | 含义  |
|  ----  | ----  |
|`ab{2,3}`|"abab"或"ababab"|
|`a{2,2}`|`a{2,2}`，匹配"aa"|
|`a{2,}`|`a{2,unbounded}`|
|`a*`|`a{0,unbounded}`|
|`a+`|`a{1,unbounded}`|
|`a?`|`a{0,1}`|

### 表达式引用(Expression reference)
用正则表达式表达一个IPv4下的IP地址:
25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9],我们为这个小表达式建立一个名字叫sec，捕获一次再重复三次，就可以将正则表达式写成：
`(?:<sec>25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])(.(?:<sec>)){3}`
* 为表达式命名的语法是(?:<名字>表达式)
* 使用命名的表达式的语法是(?:<名字>)
未来可能会加上Back reference

### 正向预查
语法：(?=纯正则表达式)。
代表这个地方一定要匹配所包含的表达式，但是在匹配结束之后指针并没有继续往后走。譬如我们需要在文章中搜索所有在98和2000前面的“Windows”，我们使用正则表达式Windows(?=98|2000)来完成这个任务。

### 反向预查
语法：(?!纯正则表达式)。
与正向预查相反，反向预查搜索所有不匹配的东西。譬如我们需要在文章中搜索所有不在98和2000前面的“Windows”，我们使用Windows(?!98|2000)来完成这个任务。

### 匿名捕获
语法:(?正则表达式)。
如果一个正则表达式的匹配路径走过这条路的话，那么括号内所匹配的字符串将会被装进一个列表里面。举个例子，我们需要把字符串123,456,78,90中的所有数字都拿出来，我们使用(?[0-9]+)(,(?[0-9]+))*来完成这个任务。
捕获到一个公共的vector<string>中。

### 命名捕获
语法:(?:<#名字>表达式)。
譬如我们想获得email地址的“@”两边的内容并装进两个不同的列表里面，我们使用(?:<#user>[a-zA-Z0-9_-.]+)@(?:<#host>[a-zA-Z0-9_-.]+)来完成这个任务。当这个正则表达式匹配了一个email地址之后，会分别把用户名和服务器装进user和host两个列表里面。
捕获到map<string,string>中

### 命名检查
语法:(?:<$名字>)。
举个例子，我们想检查一个字符串的“.”两边是否一致，譬如ab.ab、123.123、+-*/.+-*/等，我们可以使用(?:<#part>[^.]+).(?:<$part>)来完成这个任务。当字符串顺利到达“.”的时候，前边的部分就被放进了part表中。接下来他会检查接下来字符串是否跟表part中所保留的捕获一致。如果是ab.cd的话，part={ab}，因此匹配cd的时候就会失败。如果是cab.abc的话就会成功，而且能匹配到ab.ab。
`(?:<$part>)`等同于map[part]中的正则表达式

### 边界
我们使用`^`代表字符串开头，使用`$`代表字符串结尾。因此在上面的例子中，如果我们想匹配整个字符串而不是一部分的话，我们使用^(?:<#part>[^.]+).(?:<$part>)$来完成这个任务。这样的话ab.ab就会成功匹配，而cab.abc就不会成功匹配了（而不是找到子串ab.ab）。如果我们想表达”^”这个字符怎么办呢？答案是使用转义\^。正则表达式的转义字符很多，不过这个跟实现是没什么关系的，到时候特殊处理一下便是。

### 非贪婪重复
|  内容   | 含义  |
|  ----  | ----  |
|`a??`|`a{0,1}`,优先匹配`a{0}`|
在一个重复后加“?”代表让前面的重复次数尽可能少。

## CzxRegex

## Error