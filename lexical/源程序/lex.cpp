#include "lex.h"

//保留字数组
string reserved_words[] = { "void", "int", "char", "float",
                        "double", "bool", "if", "else",
                        "for", "while", "do", "return" };

bool endFile = false;//文件结束标志
string word = "";//用来存放一个单词的各个字符
int row = 1, col = 1;


bool isAlpha(char word) {//判断是否为字母
    if (word >= 'a' && word <= 'z' || word >= 'A' && word <= 'Z')
        return true;
    return false;
}

bool isNumber(char word) {//判断是否为数字
    if (word >= '0' && word <= '9')
        return true;
    return false;
}



//查找保留字表，找到返回保留字类别码，否则返回0
int lookup(string token) {
    for (int i = 0; i < RESERVED_WORDS_LEN;i++)
        if (reserved_words[i] == token)
            return RESERVED+i+1;
    return 0;
}

//输出单词的内部表示
void out(int id, string token, FILE* res) {
    //写入中间文件
    fprintf(res,"%c%d%c", '(', id, ',');
    for(char c:token)
        fputc(c, res);
    fputc(')', res);
    fputc('\n', res);
    cout << "(" << id << "," << token << ")" << endl;

}

void scan(FILE* fp, FILE* res) {//从文件读取一个单词，判断类别
    char ch;//当前扫描字符
    int c;//保存单词类别
    ch = fgetc(fp);

    if (feof(fp)) {//文件结束
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
    int idLength = 0;//标识符长度

    //合法的标识符：以字母或下划线开头，由数字、字母、下划线组成，长度不超过32
    if (isAlpha(ch) || ch == '_') {
        word += ch;
        idLength++;
        ch = fgetc(fp);
        int begincol = col;
        while (idLength <=32 && (isNumber(ch) || isAlpha(ch) || ch == '_')) {//数字、字母或下划线
            word += ch;
            idLength++;
            col++;
            if (feof(fp))
                break;
            ch = fgetc(fp);
        }
        if (idLength > 32) {
            cout << "第" << row << "行" << "第" << begincol << "列" << ": "<< word << "为非法标识符：长度超过32！\n";
            return;
        }
        fseek(fp, -1, 1);//回退字符
        col--;
        c = lookup(word);//在保留字数组中查找单词
        if (c == 0)//c为标识符
            out(ID, word,res);//输出单词的内部表示
        else out(c, "",res);//c为保留字
    }//判断无符号整数
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
            else if (ch == '/') {//此时遇到第一种注释//
                if (feof(fp)) {
                    out(ANNOTATION1, "",res);// "//"
                    endFile = true;
                    return;
                }
                ch = fgetc(fp);
                while (ch != '\n') {//一直往后扫描直到遇到回车
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
            else if (ch == '*') {//此时遇到第二种注释/*
                if (feof(fp)) {
                    cout << "第" << row << "行" << "第" << col << "列" << ": " << "文件注释结尾未闭合" << endl;
                    endFile = true;
                    return;
                }
                ch = fgetc(fp);
                while (1) {//一直往后扫描直到遇到*
                    if (feof(fp)) {
                        cout << "第" << row << "行" << "第" << col << "列" << ": " << "文件注释结尾未闭合" << endl;
                        endFile = true;
                        return;
                    }
                    if (ch == '\n') {
                        row++;
                        col = 1;
                    }
                    if (ch == '*') {
                        if (feof(fp)) {
                            cout << "第" << row << "行" << "第" << col << "列" << ": " << "文件注释结尾未闭合" << endl;
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
                            cout << "第" << row << "行" << "第" << col << "列" << ": " << "文件注释结尾未闭合" << endl;
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
                    cout << "第" << row << "行" << "第" << col << "列：" << "/用法错误!" << endl;
                }
                else
                    out(DIV, "",res);//除法
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
                cout << "第" << row << "行" << "第" << col << "列" << ": " << "&缺少操作数" << endl;
                return;
            }
            ch = fgetc(fp);
            col++;
            if (ch == '&') 
                out(AND, "",res);
            else if (!isNumber(ch) && !isAlpha(ch)) {
                col--;
                fseek(fp, -1, 1);
                cout << "第" << row << "行" << "第" << col << "列" << ": " << "&用法错误" << endl;
            }
            else {
                col--;
                fseek(fp, -1, 1);
                out(BIT_AND, "",res);
            }
            break;
        case '|':
            if (feof(fp)) {
                cout << "第" << row << "行" << "第" << col << "列" << ": " << "|缺少操作数" << endl;
                return;
            }
            ch = fgetc(fp);
            col++;
            if (ch == '|')
                out(OR, "",res);
            else if (!isNumber(ch) && !isAlpha(ch)) {
                col--;
                fseek(fp, -1, 1);
                cout << "第" << row << "行" << "第" << col << "列" << ": " << "|用法错误" << endl;
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
            cout << "第" << row << "行" << "第" << col << "列" << ": " << "未知符号！" << endl;
            break;

    }
}

