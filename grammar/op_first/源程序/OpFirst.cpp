#include "OpFirst.h"
#include "lex.h"

Set firstvt, lastvt;//firstvt����lastvt��
NODES grammar;//�ķ�����
vector<char> opFirst_stack;//����ջ
OpFirstMatrix matrix;//������Ⱦ���
set<char> nonTerminate, Terminate;//���ս���ż�, �ս���ż�
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
	fseek(fp, 2, 1);//windows�»س�ռ2���ֽ�
	return current;
}

//�ֽ����ʽ����->Ϊ��
NODE splitGeneration(string generation) {
	string right;
	int len = generation.size();
	int pos = 0;
	for (; pos < len; pos++)
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

//1). U -> b... �� U -> Vb..., b ���� firstvt(U)
//2). U -> V..., ��firstvt(V) �� firstvt(U)��
void genFirstvt() {//����firstvt��
	bool change = true;//��Ϊfirstvt���Ƿ�����ı�־
	while (change) {
		change = false;
		for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
			char left = it->first;//����ʽ��
			for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
				string right = *vit;//����ʽ�Ҳ�
				int len = right.size();//�Ҳ�����
				if (Terminate.find(right[0]) != Terminate.end()) {//�Ҳ����ַ������ս����
					if (firstvt[left].find(right[0]) == firstvt[left].end()) {//���ڲż���
						firstvt[left].insert(right[0]);
						change = true;
					}
				}
				else if (len >= 2 && Terminate.find(right[1]) != Terminate.end()) {//�Ҳ����ַ����ڷ��ս���ţ��ڶ��������ս����
					if (firstvt[left].find(right[1]) == firstvt[left].end()) {//���ڲż���
						firstvt[left].insert(right[1]);
						change = true;
					}
				}
				else{//�Ҳ����ַ����ڷ��ս���ţ��ڶ���Ҳ���ڷ��ս����
					for (set<char>::iterator iter = firstvt[right[0]].begin(); iter != firstvt[right[0]].end(); ++iter) {
						if (firstvt[left].find(*iter) == firstvt[left].end()) {//����left��firstvt��
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
void genLastvt() {//����lastvt��
	bool change = true;
	while (change) {
		change = false;
		for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
			char left = it->first;//����ʽ��
			for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
				string right = *vit;//����ʽ�Ҳ�
				int len = right.size();//�Ҳ�����
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

//�涨�ķ���ʼ����ΪE
//Query������end1��end2����end1 < end2, �����Ⱦ����Ԫ��Ϊ-1
//	                   ��end1 = end2, 0
//	                   ��end1 > end2, 1
void genOpFirstMatrix() {//����������Ⱦ���
	//���ȴ���#��# < firstvt(E), lastvt(E) > #
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
			string right = *vit;//����ʽ�Ҳ�
			int len = right.size();//�Ҳ�����
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

void showOpFirstMatrix() {//��ʾ������Ⱦ���
	cout << "������Ⱦ������£�" << endl;
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
		//����#
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

void showFirstvt() {//��ʾfirstvt��
	cout << "firstvt������:" << endl;
	for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {
		cout << *it << "��firstvt��: ";
		for (set<char>::iterator vit = firstvt[*it].begin(); vit != firstvt[*it].end(); ++vit)
			cout << *vit << "  ";
		cout << endl;
	}
}

void showLastvt() {//��ʾlastvt��
	cout << "lastvt������:" << endl;
	for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {
		cout << *it << "��lastvt��: ";
		for (set<char>::iterator vit = lastvt[*it].begin(); vit != lastvt[*it].end(); ++vit)
			cout << *vit << "  ";
		cout << endl;
	}
}

//ƥ�������ض����ĳ������ʽ�Ҳ�
bool match(string leftsuDuanYu, string right) {
	if (leftsuDuanYu.size() != right.size())
		return false;
	int len = leftsuDuanYu.size();
	for (int i = 0; i < len; ++i) {
		if (Terminate.find(leftsuDuanYu[i]) != Terminate.end()) {//��ǰ�����ض���ķ���Ϊ�ս����
			if (Terminate.find(right[i]) == Terminate.end() || leftsuDuanYu[i] != right[i])//������ȵ��ս����
				return false;
		}
		else {
			if (nonTerminate.find(right[i]) == nonTerminate.end())//���Ƕ�Ӧ�ķ��ս����
				return false;
		}
	}
	return true;
}

//��Լ�����ض���
char reduce(string leftsuDuanYu) {
	for (NODES::iterator it = grammar.begin(); it != grammar.end(); ++it) {
		char left = it->first;
		for (set<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
			string right = *vit;
			if (match(leftsuDuanYu, right))
				return left;
		}
	}
	return '0';//�Ҳ�����Լ�ķ���
}

//�����м��ļ����ж�����Ƿ�Ϸ�
bool opFirst(FILE* fp) {
	opFirst_stack.push_back('#');
	char topEnd = '#';//��ǰջ���ǰ���ս����(��Լ�������ұߵ��ս����)
	int topEndPos = 0;
	char inputCh;//�������
	char reduceCh;//��Լ����

	current = getCurrent(fp);

	while (!feof(fp)) {
		if (current.id == -1)
			break;
		if (current.id == ID) {
			inputCh = current.word[0];
			if (inputCh != 'i') {
				cout << "����δ�����ʶ��!" << endl;
				return false;
			}

		}
		else inputCh = convertion[current.id][0];
		Query q = { topEnd,inputCh };
		if (matrix.find(q) == matrix.end())
			return false;
		int priorRes = matrix[q];//���ȼ��ȽϽ��
		if (priorRes <= 0) {//topEnd �����ȼ�������������ŵ����ȼ�
			opFirst_stack.push_back(inputCh);//�����������ջ
			topEnd = inputCh;//����topEnd
			topEndPos = opFirst_stack.size() - 1;
			current = getCurrent(fp);//���봮����һλ
		}
		else {//topEnd �����ȼ�����������ŵ����ȼ�
			//����ȷ����Լ��
			while (matrix[{topEnd, inputCh}] == 1) {
				if (topEnd == opFirst_stack.back()) {//��Լ������topEnd
					string toBeReduced = "";
					toBeReduced += topEnd;
					reduceCh = reduce(toBeReduced);//��Լ����
					if (reduceCh == '0')
						return false;
					//����topEnd��topEndPos
					topEndPos--;
					while (topEndPos >= 0 && Terminate.find(opFirst_stack[topEndPos]) == Terminate.end())
						topEndPos--;
					if (topEndPos == -1)
						topEndPos = 0;
					topEnd = opFirst_stack[topEndPos];
					opFirst_stack.pop_back();//ȥ������Լ�ķ���
					opFirst_stack.push_back(reduceCh);//�����Լ����
				}
				else {
					int nextPos = topEndPos;
					for (int i = topEndPos - 1; i >= 0; i--)
						if (i == 0 || (Terminate.find(opFirst_stack[i]) != Terminate.end() && matrix[{opFirst_stack[i], opFirst_stack[nextPos]}] == -1)) {
							string toBeReduced = "";//����Լ��
							int len = opFirst_stack.size();
							for (int k = i + 1; k < len; ++k)
								toBeReduced += opFirst_stack[k];
							reduceCh = reduce(toBeReduced);
							if (reduceCh == '0')
								return false;
							//����topEnd
							topEndPos = i;
							topEnd = opFirst_stack[topEndPos];
							for (int t = len - 1; t != i; t--)//ɾ������Լ��
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
			topEnd = inputCh;//����topEnd
			topEndPos = opFirst_stack.size() - 1;
			if (current.id == EOF)
				break;
			current = getCurrent(fp);//���봮����һλ	
		}
	}
	return true;
}