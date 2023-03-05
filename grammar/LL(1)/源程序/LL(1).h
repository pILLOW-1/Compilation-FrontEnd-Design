#pragma once
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <stack>
#define MAX_NUM 100
#define EPSILON '$'//�շ���
using namespace std;
#pragma warning(disable:4996)	

//�涨�ķ��Ŀ�ʼ������S
//G[S]: S��V = E
//E��TX
//X��ATX | ��
//T��FY
//Y��MFY | ��
//F��(E) | i
//A�� + | -
//M�� * |/
//V��i

//����ʽ
typedef struct node {
	char left;//����ʽ��
	string right;//����ʽ�Ҳ�
}NODE;

//ͨ�����ս�������ս�������ӵĲ�ѯ��Ӧ����ʽ
typedef struct query {
	char non_end;//���ս����
	char end;//�ս����
}Query;

//�º���
class compareQuery {
public:
	bool operator()(const Query& a, const Query& b) const {
		return a.non_end < b.non_end || (a.non_end == b.non_end && a.end < b.end);//����
	}
};

//LL(1)������
typedef map<Query, string, compareQuery> LL1_table;

//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
typedef unordered_map<char, set<string>> NODES;

//ͨ�����ս�����ҵ�first���Ϻ�follow����
typedef map<char, set<char>> Set;

typedef struct tuple {
	int id;
	string word;
}WORD;//�ʷ������Ķ�Ԫ��ṹ��

extern unordered_map<int, string> convertion;

//�����м��ļ����ж�����Ƿ�Ϸ�
bool LL_1(FILE* fp);

void setGrammar();//�����ķ�NODES
void showGrammar();
void genFirst();//����first��
void genFollow();//����follow��
void genLL1Table();//����LL1������
void showLL1Table();//��ʾLL1������

void showFirst();//��ʾfirst��
void showFollow();//��ʾfollow��
void showLL1Table();//��ʾLL1������
