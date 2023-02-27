#pragma once
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <vector>
#define MAX_NUM 100
#define EPSILON '$'//空符号
using namespace std;
#pragma warning(disable:4996)	

//规定文法的开始符号是E
//G[E]:
//E->E+T
//E->E-T
//E->T
//T->T*F
//T->T/F
//T->F
//F->(E)
//F->i

//产生式
typedef struct node {
	char left;//产生式左部
	string right;//产生式右部
}NODE;

//通过终结符号1与终结符号2连接的查询优先关系
typedef struct query {
	char end1;//终结符号1
	char end2;//终结符号2
}Query;

//仿函数
class compareQuery {
public:
	bool operator()(const Query& a, const Query& b) const {
		return a.end1 < b.end1 || (a.end1 == b.end1 && a.end2 < b.end2);//升序
	}
};

//算符优先矩阵
typedef map<Query, int, compareQuery> OpFirstMatrix;

//通过非终结符号找到其所有产生式右部
typedef unordered_map<char, set<string>> NODES;

//通过非终结符号找到firstvt集合和lastvt集合
typedef map<char, set<char>> Set;

typedef struct tuple {
	int id;
	string word;
}WORD;//词法分析的二元组结构体

extern unordered_map<int, string> convertion;

//传入中间文件，判断语句是否合法
bool opFirst(FILE* fp);

void setGrammar();//设置文法NODES

void genFirstvt();//生成firstvt集
void genLastvt();//生成lastvt集
void genOpFirstMatrix();//生成算符优先矩阵
void showOpFirstMatrix();//显示算符优先矩阵

void showFirstvt();//显示firstvt集
void showLastvt();//显示lastvt集
