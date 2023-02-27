#include <iostream>
#include "recursive_descent.h"
using namespace std;

int main()
{
    FILE* fp = fopen("recursive2_input.txt", "r");//将词法分析结果作为语法分析的输入
    if (recursive_descent(fp))//调用递归下降分析函数
        cout << "此语句符合语法规则" << endl;
    else
        cout << "此语句不符合语法规则！" << endl;
    return 0;
}


