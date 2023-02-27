#include <iostream>
#include "OpFirst.h"
#include "lex.h"
using namespace std;

int main()
{
    init();
    setGrammar();//设置文法
    genFirstvt();//生成firstvt集
    genLastvt();//生成lastvt集
    showFirstvt();//显示firstvt集
    showLastvt();//显示lastvt集
    genOpFirstMatrix();
    showOpFirstMatrix();
    FILE* fp = fopen("opFirst_input.txt", "r");//将词法分析结果作为语法分析的输入
    if (opFirst(fp))//调用算符优先分析函数
        cout << "此语句符合语法规则" << endl;
    else
        cout << "此语句不符合语法规则！" << endl;
    return 0;
}