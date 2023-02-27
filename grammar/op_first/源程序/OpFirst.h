#pragma once
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <vector>
#define MAX_NUM 100
#define EPSILON '$'//�շ���
using namespace std;
#pragma warning(disable:4996)	

//�涨�ķ��Ŀ�ʼ������E
//G[E]:
//E->E+T
//E->E-T
//E->T
//T->T*F
//T->T/F
//T->F
//F->(E)
//F->i

//����ʽ
typedef struct node {
	char left;//����ʽ��
	string right;//����ʽ�Ҳ�
}NODE;

//ͨ���ս����1���ս����2���ӵĲ�ѯ���ȹ�ϵ
typedef struct query {
	char end1;//�ս����1
	char end2;//�ս����2
}Query;

//�º���
class compareQuery {
public:
	bool operator()(const Query& a, const Query& b) const {
		return a.end1 < b.end1 || (a.end1 == b.end1 && a.end2 < b.end2);//����
	}
};

//������Ⱦ���
typedef map<Query, int, compareQuery> OpFirstMatrix;

//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
typedef unordered_map<char, set<string>> NODES;

//ͨ�����ս�����ҵ�firstvt���Ϻ�lastvt����
typedef map<char, set<char>> Set;

typedef struct tuple {
	int id;
	string word;
}WORD;//�ʷ������Ķ�Ԫ��ṹ��

extern unordered_map<int, string> convertion;

//�����м��ļ����ж�����Ƿ�Ϸ�
bool opFirst(FILE* fp);

void setGrammar();//�����ķ�NODES

void genFirstvt();//����firstvt��
void genLastvt();//����lastvt��
void genOpFirstMatrix();//����������Ⱦ���
void showOpFirstMatrix();//��ʾ������Ⱦ���

void showFirstvt();//��ʾfirstvt��
void showLastvt();//��ʾlastvt��
