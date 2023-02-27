#include "OpFirst.h"
#include "lex.h"

Set firstvt, lastvt;//firstvt集、lastvt集
NODES grammar;//文法集合
vector<char> opFirst_stack;//分析栈
OpFirstMatrix matrix;//算符优先矩阵
set<char> nonTerminate, Terminate;//非终结符号集, 终结符号集
WORD current = { -1,"" };

WORD getCurrent(FILE* fp) {
	if (feof(fp))
		return { -1,"" };
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
	for (; pos < len; pos++)
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

//1). U -> b... 或 U -> Vb..., b 属于 firstvt(U)
//2). U -> V..., 则firstvt(V) 在 firstvt(U)中
void genFirstvt() {//生成firstvt集
	bool change = true;//作为firstvt集是否还增大的标志
	while (change) {
		change = false;
		for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
			char left = it->first;//产生式左部
			for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
				string right = *vit;//产生式右部
				int len = right.size();//右部长度
				if (Terminate.find(right[0]) != Terminate.end()) {//右部首字符属于终结符号
					if (firstvt[left].find(right[0]) == firstvt[left].end()) {//不在才加入
						firstvt[left].insert(right[0]);
						change = true;
					}
				}
				else if (len >= 2 && Terminate.find(right[1]) != Terminate.end()) {//右部首字符属于非终结符号，第二个属于终结符号
					if (firstvt[left].find(right[1]) == firstvt[left].end()) {//不在才加入
						firstvt[left].insert(right[1]);
						change = true;
					}
				}
				else{//右部首字符属于非终结符号，第二个也属于非终结符号
					for (set<char>::iterator iter = firstvt[right[0]].begin(); iter != firstvt[right[0]].end(); ++iter) {
						if (firstvt[left].find(*iter) == firstvt[left].end()) {//不在left的firstvt集
							firstvt[left].insert(*iter);
							change = true;
						}
					}
				}
			}
		}
	}
}

//1). U -> ...a  or  U -> ...aQ
//2). U -> ...Q, lastvt(Q) -> lastvt(U)
void genLastvt() {//生成lastvt集
	bool change = true;
	while (change) {
		change = false;
		for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
			char left = it->first;//产生式左部
			for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
				string right = *vit;//产生式右部
				int len = right.size();//右部长度
				if (Terminate.find(right[len - 1]) != Terminate.end()) {//U -> ...a
					if (lastvt[left].find(right[len - 1]) == lastvt[left].end()) {
						lastvt[left].insert(right[len - 1]);
						change = true;
					}
				}
				else if ((len >= 2 && nonTerminate.find(right[len-2]) != nonTerminate.end()) || (len == 1 && nonTerminate.find(right[len - 1]) != nonTerminate.end())) {//U -> ...Q
					for (set<char>::iterator iter = lastvt[right[len - 1]].begin(); iter != lastvt[right[len - 1]].end(); ++iter) {
						if (lastvt[left].find(*iter) == lastvt[left].end()) {
							lastvt[left].insert(*iter);
							change = true;
						}
					}
				}
				else if(len >= 2){
					if (lastvt[left].find(right[len - 2]) == lastvt[left].end()) {//U -> ...aQ
						lastvt[left].insert(right[len - 2]);
						change = true;
					}
				}
			}
		}
	}
}

//规定文法开始符号为E
//Query的两个end1和end2，若end1 < end2, 则优先矩阵的元素为-1
//	                   若end1 = end2, 0
//	                   若end1 > end2, 1
void genOpFirstMatrix() {//生成算符优先矩阵
	//首先处理#，# < firstvt(E), lastvt(E) > #
	for (set<char>::iterator it = firstvt['E'].begin(); it != firstvt['E'].end(); ++it) {
		Query q;
		q.end1 = '#';
		q.end2 = *it;
		matrix[q] = -1;
	}
	for (set<char>::iterator it = lastvt['E'].begin(); it != lastvt['E'].end(); ++it) {
		Query q;
		q.end1 = *it;
		q.end2 = '#';
		matrix[q] = 1;
	}
	Query q = { '#','#' };
	matrix[q] = 0;
	for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
		char left = it->first;
		for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
			string right = *vit;//产生式右部
			int len = right.size();//右部长度
			for (int i = 0; i < len; ++i) {
				if (Terminate.find(right[i]) != Terminate.end()) {
					if (i + 1 < len && (Terminate.find(right[i + 1]) != Terminate.end())) {//U -> ...ab...
						Query q = { right[i],right[i + 1] };
						matrix[q] = 0;
					}
					else if (i + 1 < len && (nonTerminate.find(right[i + 1]) != nonTerminate.end())) {//U -> ...aA...
						for (set<char>::iterator iter = firstvt[right[i + 1]].begin(); iter != firstvt[right[i + 1]].end(); ++iter) {
							Query q = { right[i],*iter };
							matrix[q] = -1;
						}
						if (i + 2 < len && (Terminate.find(right[i + 2]) != Terminate.end())) {//U -> ...aAb...
							Query q = { right[i],right[i+2] };
							matrix[q] = 0;
						}
					}
					if (i > 0 && (nonTerminate.find(right[i - 1]) != nonTerminate.end())) {//U -> ...Ab...
						for (set<char>::iterator it = lastvt[right[i-1]].begin(); it != lastvt[right[i-1]].end(); ++it) {
							Query q;
							q.end1 = *it;
							q.end2 = right[i];
							matrix[q] = 1;
						}
					}
				}
			}
		}
	}
}

void showOpFirstMatrix() {//显示算符优先矩阵
	cout << "算符优先矩阵如下：" << endl;
	cout << "\t";
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it) {
		cout << *it << "\t";
	}
	cout << '#' << endl;
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it) {
		cout << *it << "\t";
		for (set<char>::iterator vit = Terminate.begin(); vit != Terminate.end(); ++vit) {
			Query q = { *it, *vit };
			if (matrix.find(q) != matrix.end()) {
				int res = matrix[q];
				if (res == 0)
					cout << '=';
				else if (res == -1)
					cout << '<';
				else cout << '>';
			}
			cout << '\t';
		}
		//处理#
		Query q = { *it, '#' };
		if (matrix.find(q) != matrix.end())
			cout << '>';
		cout << endl;
	}
	cout << '#' << "\t";
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it) {
		Query q = { '#',*it };
		if (matrix.find(q) != matrix.end())
			cout << '<';
		cout << '\t';
	}
	cout << '=' << endl;
}

void showFirstvt() {//显示firstvt集
	cout << "firstvt集如下:" << endl;
	for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {
		cout << *it << "的firstvt集: ";
		for (set<char>::iterator vit = firstvt[*it].begin(); vit != firstvt[*it].end(); ++vit)
			cout << *vit << "  ";
		cout << endl;
	}
}

void showLastvt() {//显示lastvt集
	cout << "lastvt集如下:" << endl;
	for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {
		cout << *it << "的lastvt集: ";
		for (set<char>::iterator vit = lastvt[*it].begin(); vit != lastvt[*it].end(); ++vit)
			cout << *vit << "  ";
		cout << endl;
	}
}

//匹配最左素短语和某个产生式右部
bool match(string leftsuDuanYu, string right) {
	if (leftsuDuanYu.size() != right.size())
		return false;
	int len = leftsuDuanYu.size();
	for (int i = 0; i < len; ++i) {
		if (Terminate.find(leftsuDuanYu[i]) != Terminate.end()) {//当前最左素短语的符号为终结符号
			if (Terminate.find(right[i]) == Terminate.end() || leftsuDuanYu[i] != right[i])//不是相等的终结符号
				return false;
		}
		else {
			if (nonTerminate.find(right[i]) == nonTerminate.end())//不是对应的非终结符号
				return false;
		}
	}
	return true;
}

//归约最左素短语
char reduce(string leftsuDuanYu) {
	for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
		char left = it->first;
		for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
			string right = *vit;
			if (match(leftsuDuanYu, right))
				return left;
		}
	}
	return '0';//找不到归约的符号
}

//传入中间文件，判断语句是否合法
bool opFirst(FILE* fp) {
	opFirst_stack.push_back('#');
	char topEnd = '#';//当前栈中最靠前的终结符号(归约串的最右边的终结符号)
	int topEndPos = 0;
	char inputCh;//输入符号
	char reduceCh;//归约符号

	current = getCurrent(fp);

	while (!feof(fp)) {
		if (current.id == -1)
			break;
		if (current.id == ID) {
			inputCh = current.word[0];
			if (inputCh != 'i') {
				cout << "出现未定义标识符!" << endl;
				return false;
			}

		}
		else inputCh = convertion[current.id][0];
		Query q = { topEnd,inputCh };
		if (matrix.find(q) == matrix.end())
			return false;
		int priorRes = matrix[q];//优先级比较结果
		if (priorRes <= 0) {//topEnd 的优先级不大于输入符号的优先级
			opFirst_stack.push_back(inputCh);//将输入符号入栈
			topEnd = inputCh;//更新topEnd
			topEndPos = opFirst_stack.size() - 1;
			current = getCurrent(fp);//输入串后移一位
		}
		else {//topEnd 的优先级大于输入符号的优先级
			//首先确定归约串
			while (matrix[{topEnd, inputCh}] == 1) {
				if (topEnd == opFirst_stack.back()) {//归约串就是topEnd
					string toBeReduced = "";
					toBeReduced += topEnd;
					reduceCh = reduce(toBeReduced);//归约符号
					if (reduceCh == '0')
						return false;
					//更新topEnd及topEndPos
					topEndPos--;
					while (topEndPos >= 0 && Terminate.find(opFirst_stack[topEndPos]) == Terminate.end())
						topEndPos--;
					if (topEndPos == -1)
						topEndPos = 0;
					topEnd = opFirst_stack[topEndPos];
					opFirst_stack.pop_back();//去除被归约的符号
					opFirst_stack.push_back(reduceCh);//推入归约符号
				}
				else {
					int nextPos = topEndPos;
					for (int i = topEndPos - 1; i >= 0; i--)
						if (i == 0 || (Terminate.find(opFirst_stack[i]) != Terminate.end() && matrix[{opFirst_stack[i], opFirst_stack[nextPos]}] == -1)) {
							string toBeReduced = "";//待归约串
							int len = opFirst_stack.size();
							for (int k = i + 1; k < len; ++k)
								toBeReduced += opFirst_stack[k];
							reduceCh = reduce(toBeReduced);
							if (reduceCh == '0')
								return false;
							//更新topEnd
							topEndPos = i;
							topEnd = opFirst_stack[topEndPos];
							for (int t = len - 1; t != i; t--)//删除被归约串
								opFirst_stack.pop_back();
							opFirst_stack.push_back(reduceCh);
							break;
						}
						else if(Terminate.find(opFirst_stack[i]) != Terminate.end() ){
							nextPos = i;
						}
				}
			}
			opFirst_stack.push_back(inputCh);
			topEnd = inputCh;//更新topEnd
			topEndPos = opFirst_stack.size() - 1;
			if (current.id == EOF)
				break;
			current = getCurrent(fp);//输入串后移一位	
		}
	}
	return true;
}