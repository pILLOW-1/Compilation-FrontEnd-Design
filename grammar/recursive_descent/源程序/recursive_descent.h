#pragma once
#include <iostream>
using namespace std;
#pragma warning(disable:4996)

//递归下降分析，返回为true表示符合语法规则
bool recursive_descent(FILE* fp);