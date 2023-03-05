#include "lex.h"
#include "LL(1).h"

Set first, follow;//first集、follow集
NODES grammar;//文法集合
LL1_table table;//LL1分析表
stack<char> LL1_stack;//分析栈
set<char> nonTerminate,Terminate;//非终结符号集, 终结符号集
WORD current = { -1,"" };

WORD getCurrent(FILE* fp) {
	if (feof(fp))
		return current;
	current.word.clear();
	fseek(fp, 1, 1);
	fscanf(fp, "%d", &current.id);
	fseek(fp, 1, 1);
	char ch = fgetc(fp);
	while (ch != ')') {
		current.word += ch;
		ch = fgetc(fp);
	}
	fseek(fp, 2, 1);//windows下回车占2个字节
	return current;
}


//分解产生式，以->为界
NODE splitGeneration(string generation) {
	string right;
	int len = generation.size();
	int pos = 0;
	for(; pos<len;pos++)
		if (generation[pos] == '>') {
			pos++;
			break;//此时找到->后的首个非空字符
		}
	right = generation.substr(pos, len - pos);
	return { generation[0],right };
}



void setGrammar() {//设置文法NODES, 同时收集非终结符号与终结符号
	cout << "请输入文法(中间不要带空格)：" << endl;
	string generation;
	NODE gram;
	while (cin >> generation) {
		gram = splitGeneration(generation);//首先分解产生式
		grammar[gram.left].insert(gram.right);//再将此产生式加入产生规则
		nonTerminate.insert(gram.left);//收集非终结符号
		int rightLen = gram.right.size();//产生式右部长度
		for (int i = 0; i < rightLen; i++)
			if (!((gram.right)[i] >= 'A' && (gram.right)[i] <= 'Z'))//不是大写字母就是终结符号
				Terminate.insert(gram.right[i]);
	}
}

void genFirst() {//生成first集
	bool change = true;//作为first集是否还增大的标志
	int i = 0;
	//非终结符号求first集
	while (change) {//只要first集还增大，就循环
		change = false;//初始化为false
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//遍历非终结符号集，找出产生式
			set<string> generations = grammar[*it];//通过非终结符号找到其所有产生式右部
			for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//遍历这个非终结符号的所有产生式
				string right = *vit;//产生式右部
				int rightlen = right.size();
				char left = *it;//产生式左部
				if (Terminate.find(right[0]) != Terminate.end()) {// x -> aa, 首字符属于终结符号集
					if (first[left].find(right[0]) == first[left].end()) {//如果不在first集才加入
						first[left].insert(right[0]);//将首字母加入follow(left)
						change = true;
					}
				}
				else if (nonTerminate.find(right[0]) != nonTerminate.end()) {//x -> y1y2...yk, 首字符属于非终结符号集
	                i = 0;
					for (; i < rightlen; ++i) {//遍历右部的每个字符
						if (Terminate.find(right[i]) != Terminate.end()) {//属于终结符号
							if (first[left].find(right[i]) == first[left].end()) {//如果不在first集才加入
								first[left].insert(right[i]);//将首字母加入first(left)
								change = true;
							}
							break;//遇到终结符号就退出
						}
						else {
							bool forward = false;//若当前非终结符号的first集没有epsilon，则停止向后扫描
							if (first[right[i]].find(EPSILON) != first[right[i]].end())
								forward = true;
							for (set<char>::iterator iter = first[right[i]].begin(); iter != first[right[i]].end(); ++iter) {
								if (first[left].find(*iter) == first[left].end() && *iter != EPSILON) {//如果不在first集并且不为epsilon才加入
									first[left].insert(*iter);//将首字母加入first(left)
									change = true;
								}
							}
							if (!forward)
								break;//停止向后扫描
						}
					}
					if (i == rightlen)//此时y1y2...yk的first集均含有epsilon
						first[left].insert(EPSILON);//向left的first集加入epsilon
				}
			}
		}
	}
	//终结符号求first集
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it)
			first[*it].insert(*it);
}

void genFollow() {//生成follow集
	bool change = true;//作为first集是否还增大的标志

	follow['S'].insert('#');//规定文法的开始符号是S
	while (change) {
		change = false;//初始化为false
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//遍历非终结符号集，找出产生式
			set<string> generations = grammar[*it];//通过非终结符号找到其所有产生式右部
			for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//遍历这个非终结符号的所有产生式
				string right = *vit;//产生式右部
				int rightlen = right.size();
				char left = *it;//产生式左部
				for (int i = 0; i < rightlen; i++) {
					if (i < rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB...
						for (set<char>::iterator iter = first[right[i + 1]].begin(); iter != first[right[i + 1]].end(); ++iter) {
							if (follow[right[i]].find(*iter) == follow[right[i]].end() && *iter != EPSILON) {//如果不在follow集并且不为epsilon才加入
								follow[right[i]].insert(*iter);
								change = true;
							}
						}	
						if (first[right[i + 1]].find('$') != first[right[i + 1]].end()) {//$在right[i+1]的first集
							for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
								if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//如果不在follow集才加入
									follow[right[i]].insert(*iter);
									change = true;
								}
							}
						}
					}
					else if (i == rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB
						for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
							if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//如果不在follow集才加入
								follow[right[i]].insert(*iter);
								change = true;
							}
						}
					}
				}
			}
		}
	}
}

void genLL1Table() {//生成LL1分析表
	for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {//遍历文法
		
		for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {//遍历某一个非终结符号的所有右部
			char right = (*vit)[0];//右部的第一个字符
			for (char c : first[right]) {
				Query q;
				q.non_end = it->first;
				q.end = c;
				if (c != '$') {
					if (table.find(q) == table.end())
						//table[q] = *vit;
						table.insert(make_pair(q, *vit));
				}
				else {
					for (set<char>::iterator iter = Terminate.begin(); iter != Terminate.end(); ++iter) {
						if (*iter != '$' && follow[it->first].find(*iter) != follow[it->first].end()) {
							q.end = *iter;
							if (table.find(q) == table.end())
								table.insert(make_pair(q, *vit));
						}
					}
					//单独判断#号
					if (follow[it->first].find('#') != follow[it->first].end()) {
						q.end = '#';
						if (table.find(q) == table.end())
							table.insert(make_pair(q, *vit));
					}
				}
			}
		}
	}
}

void showFirst() {//显示非终结符号的first集
	for (Set::iterator it = first.begin(); it != first.end(); ++it) {

			cout << it->first << "的first集为：";
			for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
				cout << *vit << " ";
			cout << endl;
	}	
}

void showFollow() {//显示follow集
	for (Set::iterator it = follow.begin(); it != follow.end(); ++it) {
		if (nonTerminate.find(it->first) != nonTerminate.end()) {
			cout << it->first << "的follow集为：";
			for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
				cout << *vit << " ";
			cout << endl;
		}
	}
}

void showGrammar() {
	for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
		for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
			cout << it->first << " -> " << *vit << endl;
		}
	}
}

void showLL1Table() {//显示LL1分析表
	cout << "LL1分析表如下所示：" << endl;
	cout << "\t";
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it) {
		if(*it!='$')
			cout << *it << "\t";
	}
	cout << '#' << endl;

	for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {
		cout << *it << "\t";
		Query q;
		for (set<char>::iterator vit = Terminate.begin(); vit != Terminate.end(); ++vit) {
			q = { *it,*vit };
			if (*vit != '$' && table.find(q) != table.end())
				cout << *it << " -> " << table[q] << "\t";
			else if(*vit != '$')
				cout << "\t";
		}
		q = { *it,'#' };
		if (table.find(q) != table.end())
			cout << *it << " -> " << table[q];
		cout << endl;
	}
}

//LL(1)分析
bool LL_1(FILE* fp) {
	char c;

	LL1_stack.push('#');
	LL1_stack.push('S');
	current = getCurrent(fp);
	
	while (!feof(fp)) {
		if (current.id == -1)
			break;
		char non_end = LL1_stack.top();
		if (Terminate.find(non_end) != Terminate.end() || non_end == '#') {//栈顶符号是终结符号或#
			if (current.id == ID) {//标识符
				if (current.word[0] == non_end) {
					LL1_stack.pop();
					current = getCurrent(fp);//输入串向前推进
				}
				else {
					cout << "出现未定义的变量" << current.word[0] << endl;
					return false;
				}
			}
			else if (non_end == convertion[current.id][0]) {
				if (non_end == '#')
					return true;
				LL1_stack.pop();
				current = getCurrent(fp);//输入串向前推进
			}
			else return false;
		}
		else {
			Query q;
			q.non_end = non_end;
			if (current.id == ID)
				q.end = current.word[0];
			else q.end = convertion[current.id][0];
			if (table.find(q) == table.end())
				return false;
			else {
				LL1_stack.pop();
				string right = table[q];
				if (right != "$") {
					for (int i = right.size() - 1; i >= 0; i--)
						LL1_stack.push(right[i]);
				}
			}
		}
	}
	return true;
}