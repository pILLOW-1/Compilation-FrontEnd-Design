#pragma once
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <stack>
#define MAX_NUM 100
#define EPSILON '$'//空符号
using namespace std;
#pragma warning(disable:4996)	

//规定文法的开始符号是S
//G[S]: S→V = E
//E→TX
//X→ATX | ε
//T→FY
//Y→MFY | ε
//F→(E) | i
//A→ + | -
//M→ * |/
//V→i

//产生式
typedef struct node {
	char left;//产生式左部
	string right;//产生式右部
}NODE;

//通过非终结符号与终结符号连接的查询相应产生式
typedef struct query {
	char non_end;//非终结符号
	char end;//终结符号
}Query;

//仿函数
class compareQuery {
public:
	bool operator()(const Query& a, const Query& b) const {
		return a.non_end < b.non_end || (a.non_end == b.non_end && a.end < b.end);//升序
	}
};

//LL(1)分析表
typedef map<Query, string, compareQuery> LL1_table;

//通过非终结符号找到其所有产生式右部
typedef unordered_map<char, set<string>> NODES;

//通过非终结符号找到first集合和follow集合
typedef map<char, set<char>> Set;

typedef struct tuple {
	int id;
	string word;
}WORD;//词法分析的二元组结构体

extern unordered_map<int, string> convertion;

//传入中间文件，判断语句是否合法
bool LL_1(FILE* fp);

void setGrammar();//设置文法NODES
void showGrammar();
void genFirst();//生成first集
void genFollow();//生成follow集
void genLL1Table();//生成LL1分析表
void showLL1Table();//显示LL1分析表

void showFirst();//显示first集
void showFollow();//显示follow集
void showLL1Table();//显示LL1分析表
