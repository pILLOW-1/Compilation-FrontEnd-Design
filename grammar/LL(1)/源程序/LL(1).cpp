#include "lex.h"
#include "LL(1).h"

Set first, follow;//first����follow��
NODES grammar;//�ķ�����
LL1_table table;//LL1������
stack<char> LL1_stack;//����ջ
set<char> nonTerminate,Terminate;//���ս���ż�, �ս���ż�
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
	fseek(fp, 2, 1);//windows�»س�ռ2���ֽ�
	return current;
}


//�ֽ����ʽ����->Ϊ��
NODE splitGeneration(string generation) {
	string right;
	int len = generation.size();
	int pos = 0;
	for(; pos<len;pos++)
		if (generation[pos] == '>') {
			pos++;
			break;//��ʱ�ҵ�->����׸��ǿ��ַ�
		}
	right = generation.substr(pos, len - pos);
	return { generation[0],right };
}



void setGrammar() {//�����ķ�NODES, ͬʱ�ռ����ս�������ս����
	cout << "�������ķ�(�м䲻Ҫ���ո�)��" << endl;
	string generation;
	NODE gram;
	while (cin >> generation) {
		gram = splitGeneration(generation);//���ȷֽ����ʽ
		grammar[gram.left].insert(gram.right);//�ٽ��˲���ʽ�����������
		nonTerminate.insert(gram.left);//�ռ����ս����
		int rightLen = gram.right.size();//����ʽ�Ҳ�����
		for (int i = 0; i < rightLen; i++)
			if (!((gram.right)[i] >= 'A' && (gram.right)[i] <= 'Z'))//���Ǵ�д��ĸ�����ս����
				Terminate.insert(gram.right[i]);
	}
}

void genFirst() {//����first��
	bool change = true;//��Ϊfirst���Ƿ�����ı�־
	int i = 0;
	//���ս������first��
	while (change) {//ֻҪfirst�������󣬾�ѭ��
		change = false;//��ʼ��Ϊfalse
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//�������ս���ż����ҳ�����ʽ
			set<string> generations = grammar[*it];//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
			for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//����������ս���ŵ����в���ʽ
				string right = *vit;//����ʽ�Ҳ�
				int rightlen = right.size();
				char left = *it;//����ʽ��
				if (Terminate.find(right[0]) != Terminate.end()) {// x -> aa, ���ַ������ս���ż�
					if (first[left].find(right[0]) == first[left].end()) {//�������first���ż���
						first[left].insert(right[0]);//������ĸ����follow(left)
						change = true;
					}
				}
				else if (nonTerminate.find(right[0]) != nonTerminate.end()) {//x -> y1y2...yk, ���ַ����ڷ��ս���ż�
	                i = 0;
					for (; i < rightlen; ++i) {//�����Ҳ���ÿ���ַ�
						if (Terminate.find(right[i]) != Terminate.end()) {//�����ս����
							if (first[left].find(right[i]) == first[left].end()) {//�������first���ż���
								first[left].insert(right[i]);//������ĸ����first(left)
								change = true;
							}
							break;//�����ս���ž��˳�
						}
						else {
							bool forward = false;//����ǰ���ս���ŵ�first��û��epsilon����ֹͣ���ɨ��
							if (first[right[i]].find(EPSILON) != first[right[i]].end())
								forward = true;
							for (set<char>::iterator iter = first[right[i]].begin(); iter != first[right[i]].end(); ++iter) {
								if (first[left].find(*iter) == first[left].end() && *iter != EPSILON) {//�������first�����Ҳ�Ϊepsilon�ż���
									first[left].insert(*iter);//������ĸ����first(left)
									change = true;
								}
							}
							if (!forward)
								break;//ֹͣ���ɨ��
						}
					}
					if (i == rightlen)//��ʱy1y2...yk��first��������epsilon
						first[left].insert(EPSILON);//��left��first������epsilon
				}
			}
		}
	}
	//�ս������first��
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it)
			first[*it].insert(*it);
}

void genFollow() {//����follow��
	bool change = true;//��Ϊfirst���Ƿ�����ı�־

	follow['S'].insert('#');//�涨�ķ��Ŀ�ʼ������S
	while (change) {
		change = false;//��ʼ��Ϊfalse
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//�������ս���ż����ҳ�����ʽ
			set<string> generations = grammar[*it];//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
			for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//����������ս���ŵ����в���ʽ
				string right = *vit;//����ʽ�Ҳ�
				int rightlen = right.size();
				char left = *it;//����ʽ��
				for (int i = 0; i < rightlen; i++) {
					if (i < rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB...
						for (set<char>::iterator iter = first[right[i + 1]].begin(); iter != first[right[i + 1]].end(); ++iter) {
							if (follow[right[i]].find(*iter) == follow[right[i]].end() && *iter != EPSILON) {//�������follow�����Ҳ�Ϊepsilon�ż���
								follow[right[i]].insert(*iter);
								change = true;
							}
						}	
						if (first[right[i + 1]].find('$') != first[right[i + 1]].end()) {//$��right[i+1]��first��
							for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
								if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//�������follow���ż���
									follow[right[i]].insert(*iter);
									change = true;
								}
							}
						}
					}
					else if (i == rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB
						for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
							if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//�������follow���ż���
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

void genLL1Table() {//����LL1������
	for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {//�����ķ�
		
		for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {//����ĳһ�����ս���ŵ������Ҳ�
			char right = (*vit)[0];//�Ҳ��ĵ�һ���ַ�
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
					//�����ж�#��
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

void showFirst() {//��ʾ���ս���ŵ�first��
	for (Set::iterator it = first.begin(); it != first.end(); ++it) {

			cout << it->first << "��first��Ϊ��";
			for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
				cout << *vit << " ";
			cout << endl;
	}	
}

void showFollow() {//��ʾfollow��
	for (Set::iterator it = follow.begin(); it != follow.end(); ++it) {
		if (nonTerminate.find(it->first) != nonTerminate.end()) {
			cout << it->first << "��follow��Ϊ��";
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

void showLL1Table() {//��ʾLL1������
	cout << "LL1������������ʾ��" << endl;
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

//LL(1)����
bool LL_1(FILE* fp) {
	char c;

	LL1_stack.push('#');
	LL1_stack.push('S');
	current = getCurrent(fp);
	
	while (!feof(fp)) {
		if (current.id == -1)
			break;
		char non_end = LL1_stack.top();
		if (Terminate.find(non_end) != Terminate.end() || non_end == '#') {//ջ���������ս���Ż�#
			if (current.id == ID) {//��ʶ��
				if (current.word[0] == non_end) {
					LL1_stack.pop();
					current = getCurrent(fp);//���봮��ǰ�ƽ�
				}
				else {
					cout << "����δ����ı���" << current.word[0] << endl;
					return false;
				}
			}
			else if (non_end == convertion[current.id][0]) {
				if (non_end == '#')
					return true;
				LL1_stack.pop();
				current = getCurrent(fp);//���봮��ǰ�ƽ�
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