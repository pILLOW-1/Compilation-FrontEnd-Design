#include "lex.h"

//����������
string reserved_words[] = { "void", "int", "char", "float",
                        "double", "bool", "if", "else",
                        "for", "while", "do", "return" };

bool endFile = false;//�ļ�������־
string word = "";//�������һ�����ʵĸ����ַ�
int row = 1, col = 1;


bool isAlpha(char word) {//�ж��Ƿ�Ϊ��ĸ
    if (word >= 'a' && word <= 'z' || word >= 'A' && word <= 'Z')
        return true;
    return false;
}

bool isNumber(char word) {//�ж��Ƿ�Ϊ����
    if (word >= '0' && word <= '9')
        return true;
    return false;
}



//���ұ����ֱ��ҵ����ر���������룬���򷵻�0
int lookup(string token) {
    for (int i = 0; i < RESERVED_WORDS_LEN;i++)
        if (reserved_words[i] == token)
            return RESERVED+i+1;
    return 0;
}

//������ʵ��ڲ���ʾ
void out(int id, string token, FILE* res) {
    //д���м��ļ�
    fprintf(res,"%c%d%c", '(', id, ',');
    for(char c:token)
        fputc(c, res);
    fputc(')', res);
    fputc('\n', res);
    cout << "(" << id << "," << token << ")" << endl;

}

void scan(FILE* fp, FILE* res) {//���ļ���ȡһ�����ʣ��ж����
    char ch;//��ǰɨ���ַ�
    int c;//���浥�����
    ch = fgetc(fp);

    if (feof(fp)) {//�ļ�����
        endFile = true;
        return;
    }

    if (ch == ' ') {
        col++;
        return;
    }
    if (ch == '\n') {
        row++;
        col = 1;
        return;
    }
    int idLength = 0;//��ʶ������

    //�Ϸ��ı�ʶ��������ĸ���»��߿�ͷ�������֡���ĸ���»�����ɣ����Ȳ�����32
    if (isAlpha(ch) || ch == '_') {
        word += ch;
        idLength++;
        ch = fgetc(fp);
        int begincol = col;
        while (idLength <=32 && (isNumber(ch) || isAlpha(ch) || ch == '_')) {//���֡���ĸ���»���
            word += ch;
            idLength++;
            col++;
            if (feof(fp))
                break;
            ch = fgetc(fp);
        }
        if (idLength > 32) {
            cout << "��" << row << "��" << "��" << begincol << "��" << ": "<< word << "Ϊ�Ƿ���ʶ�������ȳ���32��\n";
            return;
        }
        fseek(fp, -1, 1);//�����ַ�
        col--;
        c = lookup(word);//�ڱ����������в��ҵ���
        if (c == 0)//cΪ��ʶ��
            out(ID, word,res);//������ʵ��ڲ���ʾ
        else out(c, "",res);//cΪ������
    }//�ж��޷�������
    else if (isNumber(ch)) {
        word += ch;
        ch = fgetc(fp);
        col++;
        while (isNumber(ch)) {
            word += ch;
            col++;
            if (feof(fp))
                break;
            ch = fgetc(fp);
        }
        col--;
        fseek(fp, -1, 1);
        out(UNSIGNED_INT, word,res);
    }
    else switch (ch) {
        case '<':
            col++;
            ch = fgetc(fp);
            if (ch == '=')
                out(LE, "",res);//"<="
            else if (ch == '<')
                out(LS, "",res);//"<<"
            else {
                col--;
                fseek(fp, -1, 1);
                out(LT, "",res);//"<"
            }
            break;
        case '=':
            col++;
            ch = fgetc(fp);
            if (ch == '=')
                out(EQ, "",res);//"=="
            else {
                col--;
                fseek(fp, -1, 1);
                out(ASSIGN, "",res);//"="
            }
            break;
        case '>':
            ch = fgetc(fp);
            col++;
            if (ch == '=')
                out(GE, "",res);//">="
            else if (ch == '>')
                out(RS, "",res);//">>"
            else {
                col--;
                fseek(fp, -1, 1);
                out(GT, "",res);//">"
            }
            break;
        case '!':
            ch = fgetc(fp);
            col++;
            if (ch == '=')
                out(NEQ, "",res);//"!="
            else {
                col--;
                fseek(fp, -1, 1);
                out(NOT, "",res);//"!"
            }
            break;
        case '+':
            ch = fgetc(fp);
            col++;
            if (ch == '=')
                out(ADDEQ, "",res);//"+="
            else if (ch == '+')
                out(SELF_ADD, "",res);//"++"
            else {
                fseek(fp, -1, 1);
                col--;
                out(ADD, "",res);//"+"
            }
            break;
        case '-':
            ch = fgetc(fp);
            col++;
            if (ch == '=')
                out(SUBEQ, "",res);//"-="
            else if (ch == '-')
                out(SELF_SUB, "",res);//"--"
            else {
                col--;
                fseek(fp, -1, 1);
                out(SUB, "",res);//"-"
            }
            break;
        case '*':
            ch = fgetc(fp);
            col++;
            if (ch == '=')
                out(MULEQ, "",res);
            else {
                col--;
                fseek(fp, -1, 1);
                out(MUL, "",res);
            }
            break;
        case '/':
            ch = fgetc(fp);
            col++;
            if (ch == '=')
                out(DIVEQ, "",res);
            else if (ch == '/') {//��ʱ������һ��ע��//
                if (feof(fp)) {
                    out(ANNOTATION1, "",res);// "//"
                    endFile = true;
                    return;
                }
                ch = fgetc(fp);
                while (ch != '\n') {//һֱ����ɨ��ֱ�������س�
                    if (feof(fp)) {
                        out(ANNOTATION1, "",res);// "//"
                        endFile = true;
                        return;
                    }
                    col++;
                    ch = fgetc(fp);
                }
                row++;
                col = 1;
                out(ANNOTATION1, "",res);// "//"
            }
            else if (ch == '*') {//��ʱ�����ڶ���ע��/*
                if (feof(fp)) {
                    cout << "��" << row << "��" << "��" << col << "��" << ": " << "�ļ�ע�ͽ�βδ�պ�" << endl;
                    endFile = true;
                    return;
                }
                ch = fgetc(fp);
                while (1) {//һֱ����ɨ��ֱ������*
                    if (feof(fp)) {
                        cout << "��" << row << "��" << "��" << col << "��" << ": " << "�ļ�ע�ͽ�βδ�պ�" << endl;
                        endFile = true;
                        return;
                    }
                    if (ch == '\n') {
                        row++;
                        col = 1;
                    }
                    if (ch == '*') {
                        if (feof(fp)) {
                            cout << "��" << row << "��" << "��" << col << "��" << ": " << "�ļ�ע�ͽ�βδ�պ�" << endl;
                            endFile = true;
                            return;
                        }
                        ch = fgetc(fp);
                        col++;
                        if (ch == '/') {
                            out(ANNOTATION2, "",res);// "/**/"
                            return;
                        }
                        else {
                            col--;
                            fseek(fp, -1, 1);
                            cout << "��" << row << "��" << "��" << col << "��" << ": " << "�ļ�ע�ͽ�βδ�պ�" << endl;
                            break;
                        }
                    }
                    ch = fgetc(fp);
                    col++;
                }

            }
            else {
                col--;
                fseek(fp, -1, 1);
                if (!isNumber(ch) && !isAlpha(ch)) {
                    cout << "��" << row << "��" << "��" << col << "�У�" << "/�÷�����!" << endl;
                }
                else
                    out(DIV, "",res);//����
            }
            break;
        case '(':
            out(LEFT_BRACKET, "",res);
            break;
        case ')':
            out(RIGHT_BRACKET, "",res);
            break;
        case '{':
            out(LEFT_BRACE, "",res);
            break;
        case '}':
            out(RIGHT_BRACE, "",res);
            break;
        case ';':
            out(SEMICOLON, "",res);
            break;
        case ',':
            out(COMMA, "",res);
            break;
        case '&':
            if (feof(fp)) {
                cout << "��" << row << "��" << "��" << col << "��" << ": " << "&ȱ�ٲ�����" << endl;
                return;
            }
            ch = fgetc(fp);
            col++;
            if (ch == '&') 
                out(AND, "",res);
            else if (!isNumber(ch) && !isAlpha(ch)) {
                col--;
                fseek(fp, -1, 1);
                cout << "��" << row << "��" << "��" << col << "��" << ": " << "&�÷�����" << endl;
            }
            else {
                col--;
                fseek(fp, -1, 1);
                out(BIT_AND, "",res);
            }
            break;
        case '|':
            if (feof(fp)) {
                cout << "��" << row << "��" << "��" << col << "��" << ": " << "|ȱ�ٲ�����" << endl;
                return;
            }
            ch = fgetc(fp);
            col++;
            if (ch == '|')
                out(OR, "",res);
            else if (!isNumber(ch) && !isAlpha(ch)) {
                col--;
                fseek(fp, -1, 1);
                cout << "��" << row << "��" << "��" << col << "��" << ": " << "|�÷�����" << endl;
            }
            else {
                col--;
                fseek(fp, -1, 1);
                out(BIT_OR, "",res);
            }
            break;
        case '\'':
            out(SINGLE_QUO, "",res);
            break;
        case '"':
            out(DOUBLE_QUO, "",res);
            break;
        case '%':
            out(PERCENT, "",res);
            break;
        case '#':
            out(EOF, "", res);
            break;
        default:
            cout << "��" << row << "��" << "��" << col << "��" << ": " << "δ֪���ţ�" << endl;
            break;

    }
}

