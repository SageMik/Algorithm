#include "Stack.hpp"

using namespace std;
#define Item double // 支持小数运算
#define INITB '#' // 初始时运算符栈的栈底元素

void ErrorExp(); // 遇到非法表达式时进行的处理
int StrToNum(int lo, Item *result); // 数字转字符串
char Compare(char a, char b); // 运算符优先级比较
bool Calculate(Item a, Item b, char op, Item *result); // 计算
bool Legalization(int lo); // 正负数合法化

#define orderLen 7 // 支持的运算符（除井号外）
const char order[orderLen + 1] = "+-*/()#"; // \0 
const char priority[7][7] = { // 运算符优先级
//    +    -    *    /    (    )    #        先行后列
    {'>', '>', '<', '<', '<', '>', '>'}, //     +
    {'>', '>', '<', '<', '<', '>', '>'}, //     -
    {'>', '>', '>', '>', '<', '>', '>'}, //     *
    {'>', '>', '>', '>', '<', '>', '>'}, //     /
    {'<', '<', '<', '<', '<', 'T', 'F'}, //     (
    {'>', '>', '>', '>', 'F', '>', '>'}, //     )
    {'<', '<', '<', '<', '<', 'F', 'T'}  //     #
};
bool MainFlag; // 合法性标志，【ERROR】注释 表示由 MainFlag 进行标记的非法情况

Stack<char> opter; // 运算符栈
Stack<Item> opval; // 操作数栈
string uExp; // 表达式

int main() {
    char c; // 逐字符遍历
    int index; // 当前字符的索引
    Item a, b; // 操作数
    char op; // 运算符

    while (true) {
        cout << "请输入表达式：";
        cin >> uExp;
        uExp += INITB;
        opter.push(INITB);
        op = INITB;
        index = 0;

        if(!Legalization(index)) { // 表达式开头的正负数可以没有括号，因此要先合法化
            ErrorExp(); // 包含 MainFlag = false
            continue;
        }

        c = uExp[index];
        MainFlag = true;
        while ((c != INITB || op != INITB) && MainFlag) { // 操作数或运算符非井号，且处于合法情况
            a = 0;
            if (isdigit(c)) { // 数字
                /*【ERROR】小数点
                    1. 小数点在表达式末尾
                    2. 小数点在操作数末尾 */
                if((index = StrToNum(index, &a)) == -1) {
                    ErrorExp();
                    break;
                }
                opval.push(a); // 操作数入栈
                c = uExp[index];
            
            } else { // 非数字

                /*【ERROR】括号 */
                if(c == '(') {
                    if((index && isdigit(uExp[index - 1]))
                            || (size_t)(index + 2) >= uExp.length()
                            || !Legalization(index + 1)) { 
                        // 1. 括号前表达式不合法
                        // 2. 括号不匹配的一种，左括号之后至少要有两个字符，如2+(1)，若没有则必然是非法情况
                        // 3. 正负数合法化
                        ErrorExp();
                        break;
                    }
                } else if(c == ')') { 
                    // 1. 括号中表达式不合法 
                     if(index && !isdigit(uExp[index - 1]) && uExp[index - 1] != ')') {
                        ErrorExp();
                        break;
                    }
        
                    // 1. 括号后表达式不合法 
                     if(isdigit(uExp[index + 1])) {
                        ErrorExp();
                        break;
                    }
                }

                switch (Compare(op, c)) {
                    case '>': // 优先级高则先算
                        opter.pop(&op);
                        if(!opval.pop(&b) || !opval.pop(&a)) { // 不足两个操作数
                            /*【ERROR】运算符与操作数不匹配，如全是运算符等
                                    但诸如 +5, (-32) 的合法情况会被视为非法，故有提前处理 */
                            ErrorExp();
                        } else if (Calculate(a, b, op, &a))
                            opval.push(a);
                        else //【ERROR】除以零，可检测 1/0, 1/(0), 1/(0+0) 等情况
                            ErrorExp();
                        break;
                    case '<': // 优先级低则入栈
                        opter.push(c);
                        c = uExp[++index];
                        break;
                    case 'T': // 脱去括号
                        opter.pop(&op);
                        c = uExp[++index];
                        break;
                    default: 
                        //【ERROR】非法字符（操作数及运算符之外的字符），但无法处理井号
                        //【ERROR】括号错位或不匹配
                        //【ERROR】小数点 - 小数点在操作数首位，其余情况在 StrToNum() 函数中处理
                        ErrorExp();
                    }
            }
            opter.peek(&op); // 取栈顶运算符
        }
        if(MainFlag) {
            opval.pop(&a);
            // if(!opval.pop(&a))
            //     ErrorExp();
            // else if(!opval.empty()) // 可用于判断括号前后表达式合法性，但会浪费压栈和出栈操作
            //     ErrorExp();
            if((size_t)index != uExp.length() - 1) // 【ERROR】用户输入井号
                ErrorExp();
            else
                cout << "运算结果：" << a << endl << endl;
        }
    }

    return 0;
}

int StrToNum(int lo, Item *result) {
    Item sum = 0;
    char c = uExp[lo];
    while (isdigit(c)) { // 整数部分
        sum *= 10;
        sum += c - '0';
        c = uExp[++lo];
    }
    if(c == '.') { // 小数部分
        //【ERROR】小数点  - 小数点在操作数首位在主函数的 default(本质是 Calculate()函数 ) 中处理
        if(!isdigit(c = uExp[++lo])) // 1. 小数点在操作数末尾
            return -1;
        else {
            for (double i = 0.1; isdigit(c); i *= 0.1) {
                sum += (c - '0') * i;
                c = uExp[++lo]; // 可以忽略越界问题，因为字符串末尾有'\0'，其ASCII码为0
            }
            if(c == '.') // 2. 操作数中存在多个小数点
                return -1;
        }
    }
    *result = sum;
    return lo;
}

char Compare(char a, char b) {
    int i = 0;
    int j = 0;
    while(order[i] != a && i < orderLen)
        i++;
    while(order[j] != b && j < orderLen)
        j++;
    if(i > 6 || j > 6) //【ERROR】非法字符 || 括号 || 小数点
        return 'F';
    return priority[i][j];
}

bool Calculate(Item a, Item b, char op, Item *result) {
    switch(op) {
        case '+':
            *result = a + b;
            break;
        case '-':
            *result = a - b;
            break;
        case '*':
            *result = a * b;
            break;
        case '/':
            if(!b) //【ERROR】除以零
                return false;
            *result = a / b;
            break;
    }
    return true;
}

bool Legalization(int lo) {
    if(uExp[lo] == '+' || uExp[lo] == '-') {
        if(!isdigit(uExp[lo + 1]) && uExp[lo + 1] != '(') // 预防(++10)、1/(+-32)此类情况
            return false;
        opval.push(0); // 正负数合法化
    }
    return true;
}

void ErrorExp() {
    cout << "表达式不合法！" << endl << endl;
    opval.clear();
    opter.clear();
    MainFlag = false;
}
