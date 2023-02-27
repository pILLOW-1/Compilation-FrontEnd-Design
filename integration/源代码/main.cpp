#include <iostream>
#include <fstream>
#include "lex.h"
//#include "SLR.h"
#include "../SLR/SLR.h"
using namespace std;
#pragma warning(disable:4996)

int main()
{
    //词法分析
    FILE* fp = fopen("SLR.txt", "r");//输入源程序
    FILE* res = fopen("SLR_input.txt", "w");//输出中间文件
    while (!endFile) {
        scan(fp, res);
        word.clear();//每识别出一个单词，将上一次的清空
    }
    fclose(fp);
    fclose(res);

    //语法分析、语义分析、生成四元式
    init();
    setGrammar();//设置文法
    genFirst();//生成first集
    genFollow();//生成follow集
    showFirst();//显示first集
    showFollow();//显示follow集
    showGenerations();
    genDFA();//生成DFA
    showStates();//显示项目集
    genSLRTable();//生成SLR(1)分析表
    showSLRTable();//显示SLR(1)分析表
    FILE* input = fopen("SLR_input.txt", "r");//读取中间文件
    SLR(input);//调用SLR分析函数
    if (matchGram) {
        cout << "此语句符合语法规则" << endl;
        cout << "四元式如下所示：" << endl;
        showSiYuanShi();//显示四元式
    }
    else
        cout << "此语句不符合语法规则！" << endl;
    
    fclose(input);
    return 0;
}