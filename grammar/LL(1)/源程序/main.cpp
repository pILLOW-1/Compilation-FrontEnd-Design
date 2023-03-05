#include <iostream>
#include "LL(1).h"
#include "lex.h"
using namespace std;

int main()
{
    init();
    setGrammar();//设置文法
    genFirst();//生成first集

    genFollow();
    showFirst();
    showFollow();
    //showGrammar();
    genLL1Table();
    showLL1Table();
    FILE* fp = fopen("LL(1)_input2.txt", "r");//将词法分析结果作为语法分析的输入
    if (LL_1(fp))//调用LL1分析函数
        cout << "此语句符合语法规则" << endl;
    else
        cout << "此语句不符合语法规则！" << endl;
    return 0;
}


