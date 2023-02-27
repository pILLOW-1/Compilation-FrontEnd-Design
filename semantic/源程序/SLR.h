#pragma once
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <stack>
#define BEGIN 'Q'//�ķ���ʼ����
#define MAX_NUM 100
#define EPSILON '$'//�շ���
using namespace std;
#pragma warning(disable:4996)	

//�涨�ķ��Ŀ�ʼ������S
//G[S]:
//�ع㣺Q->S
//S->V=E
//E->E+T
//E->E-T
//E->T
//T->T*F
//T->T/F
//T->F
//F->(E)
//F->i
//V->i

//����ʽ
typedef struct node {
	char left;//����ʽ��
	string right;//����ʽ�Ҳ�

	bool operator == (const struct node item) const {
		return left == item.left && right == item.right;
	}
}NODE;

//ͨ��״̬��������Ӳ�ѯSLR������Ԫ��
typedef struct query {
	int state;//״̬
	char ch;//����
}Query;

//SLR�������Ԫ��
typedef struct slr {
	char kind;//'r': ��Լ�� 's': �ƽ��� 'a': ����; 'e': ����
	int num;//��ʾ�õڼ�������ʽ��Լ���ƽ���״̬��,-1��ʾ������Ŀ��-2��ʾ����
}slr;

//����һ��C++ STL��֪ʶ��
//map��multimap �����������и��ݼ��Ĵ�С�Դ洢�ļ�ֵ�Խ�������set ����Ҳ����ˣ�
//ֻ���� set �����и���ֵ�Եļ� key ��ֵ value ����ȵģ�
//���� key ����Ҳ�͵ȼ�Ϊ���� value ����



//��Ŀ����Ԫ������
typedef struct stateElem {
	bool finish;//��ʾ�Ƿ�Ϊ��Լ��false��ʾ���ǣ�true��ʾ��
	int pos;//��ʾԲ���λ�ã�ȡֵ��0-gen.size(),��Ϊgen.size()ʱ����ʾ��������ǰԲ�����ٵķ���Ϊ��gen.right[pos]
	NODE gen;//����ʽ
	int No;//��ʾ�ڼ�������ʽ,��1��ʼ

	bool operator==(const stateElem& item)const {//���ں�����
		return gen == item.gen && pos == item.pos;
	}
	bool operator<(const stateElem& item)const {//Ϊ�˶�set��key��������
		return No < item.No || (No == item.No && pos < item.pos);
	}
	void operator=(const stateElem& item) {//��ֵ���������
		finish = item.finish;
		pos = item.pos;
		gen = item.gen;
		No = item.No;
	}
}stateElem;

//��Ŀ��
typedef struct state {
	int number;//��Ŀ���
	//�����ĸ���Ա��closure���������
	set<stateElem> elem;//��Ŀ
	set<char> chs;//������Ϻ���Ŀ�����ٵ����з���
	bool conflict1;//��һ���ͻ������Լ���ƽ����档��ʾ�Ƿ��г�ͻ��false:�޳�ͻ��true:�г�ͻ��
	bool conflict2;//�ڶ����ͻ�������Լ����
	bool canShift;//��ʾ�Ƿ��к����Ŀ��true��ʾ�У�false��ʾû�С�������Ŀ��finish����һ����Ϊfalse��canShift=true,����Ϊfalse.
	bool hasReduce;//��ʾ�Ƿ��й�Լʽ��true��ʾ�У�false��ʾû��
	bool operator==(const state& item)const {
		return elem == item.elem;
	}
}state;

//��Ԫʽ
typedef struct siYuanShi {
	char op;
	char arg1;
	char arg2;
	char res;
}siYuanShi;

//���ű�
typedef map<char, deque<char>> Symbol;

//�º���
class compareQuery {
public:
	bool operator()(const Query& a, const Query& b) const {
		return a.state < b.state || (a.state == b.state && a.ch < b.ch);//����
	}
};

//SLR(1)�������̣����ɳ�̬��I0 -> ...DFA -> SLR_table -> SLR -> ��Ԫʽ�����ɣ�

//SLR������, ͨ��Query�ҷ�����Ԫ��
typedef map<Query, slr, compareQuery> SLR_table;

//��Ŀ���淶��DFA��ͨ��Query����Ŀ�����
typedef map<Query, int, compareQuery> DFA;

//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
typedef unordered_map<char, set<string>> NODES;

//ͨ�����ս�����ҵ�first���Ϻ�follow����
typedef map<char, set<char>> Set;

typedef struct tuple {
	int id;
	string word;
}WORD;//�ʷ������Ķ�Ԫ��ṹ��

extern unordered_map<int, string> convertion;

void setGrammar();//�����ķ�NODES
void genFirst();//����first��
void genFollow();//����follow��
void showFirst();//��ʾfirst��
void showFollow();//��ʾfollow��
void showSLRTable();//��ʾSLR������
void showStates();//��ʾ��Ŀ��
void showGenerations();//��ʾ����ʽ

void genDFA();//����DFA
bool genSLRTable();//����SLR������,����true��ʾ���ķ�ΪSLR(1)�ķ�
bool SLR(FILE* fp);//SLR����
void showSiYuanShi();//��ʾ��Ԫʽ