#include <iostream>
#include <fstream>
#include "lex.h"
using namespace std;
#pragma warning(disable:4996)

int main()
{
    FILE* fp = fopen("test2.txt", "r");
    FILE* res = fopen("test2_res.txt", "w");
    while (!endFile) {
        scan(fp,res);
        word.clear();//每识别出一个单词，将上一次的清空
    }
    fclose(fp);
    fclose(res);
    return 0;
}

