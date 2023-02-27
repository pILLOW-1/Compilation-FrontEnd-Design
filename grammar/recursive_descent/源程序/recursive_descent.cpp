#include "lex.h"
#include "recursive_descent.h"

//G[S]: S→V = E
//E→TE′
//E′→ATE′ | ε
//T→FT′
//T′→MFT′ | ε
//F→(E) | i
//A→ + | -
//M→ * |/
//V→i

typedef struct tuple {
	int id;
	string word;
}WORD;//词法分析的二元组结构体

WORD current = { -1,"" };
bool E(FILE* fp);
bool EE(FILE* fp);
bool T(FILE* fp);
bool TT(FILE* fp);
bool F(FILE* fp);
bool A(FILE* fp);
bool M(FILE* fp);
bool V(FILE* fp);

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

bool S(FILE* fp) {
	if (!V(fp))
		return false;
	current = getCurrent(fp);
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id != ASSIGN) {
		cout << "缺少=, 不符语法规则" << endl;
		return false;
	}
	current = getCurrent(fp);
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (!E(fp))
		return false;
	return true;
}

bool E(FILE* fp) {
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.word != "i" && current.id != LEFT_BRACKET)
		return false;
	if (!T(fp))
		return false;
	if (!EE(fp))
		return false;
	return true;
}

bool EE(FILE* fp){//E'
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id != ADD && current.id != SUB) {
		if (current.id == RIGHT_BRACKET || current.id == EOF)
			return true;
		else return false;
	}
	if (!A(fp))
		return false;
	if (!T(fp))
		return false;
	if (EE(fp))
		return true;
	else return false;
}

bool T(FILE* fp) {
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.word != "i" && current.id != LEFT_BRACKET)
		return false;
	if (!F(fp))
		return false;
	if (!TT(fp))
		return false;
	return true;
}

bool TT(FILE* fp) {
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id != MUL && current.id != DIV) {
		if (current.id == RIGHT_BRACKET || current.id == EOF ||
			current.id==ADD || current.id==SUB)
			return true;
		return false;
	}
	if (!M(fp))
		return false;
	if (!F(fp))
		return false;
	if (TT(fp))
		return true;
	else return false;
}

bool F(FILE* fp) {
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id != LEFT_BRACKET) {
		if (current.word == "i") {
			current = getCurrent(fp);
			return true;
		}
		else {
			cout << "含有非法字符！" << endl;
			return false;
		}
	}
	current = getCurrent(fp);
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (!E(fp))
		return false;
	if (current.id == RIGHT_BRACKET) {
		current = getCurrent(fp);
		return true;
	}
	else {
		cout << "左右括号不匹配!" << endl;
		return false;
	}
}

bool A(FILE* fp) {
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id != ADD) {
		if (current.id != SUB)
			return false;
		current = getCurrent(fp);
		return true;
	}
	current = getCurrent(fp);
	return true;
}

bool M(FILE* fp) {
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id != MUL && current.id != DIV)
		return false;
	current = getCurrent(fp);
	return true;
}

bool V(FILE* fp) {
	current = getCurrent(fp);
	if (current.id == -1) {
		cout << "文件已经结束" << endl;
		return false;
	}
	if (current.id == 0 && current.word == "i")
		return true;
	else {
		cout << "含有非法字符！" << endl;
		return false;
	}
}

bool recursive_descent(FILE* fp) {
	if (S(fp))
		return true;
	else return false;
}