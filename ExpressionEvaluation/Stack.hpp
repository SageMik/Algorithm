#pragma once
#include <iostream>
#include <cstring> // memcpy()
#include <string> // std::string
#include <cctype> // isdigit()
#define INITSIZE 20

template <typename Item>
class Stack {
   private:
    Item *data;
    int top = -1;
    int size = 0;
    void resize(); // 重新分配栈空间
   public:
    Stack();
    ~Stack();
    bool empty();
    void push(Item x);
    bool pop(Item *x);
    bool peek(Item *x);
    void clear(); // 只令 top = -1，并不清除栈所占用的空间
    void output(std::string separator = "   ");
};

template <typename Item>
Stack<Item>::Stack() {
    data = new Item[size = INITSIZE]; // 赋值语句返回所赋的值
}
template <typename Item>
Stack<Item>::~Stack() {
    delete[] data;
}

template <class Item>
void Stack<Item>::resize() {
    int temp = top + 1;
    int minLimit = size / 4; // 小于四分之一大小
    int maxLimit = size; // 栈满
    if(temp >= minLimit && temp < maxLimit)
        return;
    
    Item *newData;
    int newSize = size;
    if(temp < minLimit)
        newSize /= 2;
    else
        newSize *= 2;
    newData = new Item[newSize];
    memcpy(newData, data, sizeof(Item) * temp);
    delete[] data;
    data = newData;
    size = newSize;
}

template <class Item>
bool Stack<Item>::empty() {
    return top == -1;
}

template <class Item>
void Stack<Item>::push(Item x) {
    data[++top] = x;
    resize();
}
template <class Item>
bool Stack<Item>::pop(Item *x) {
    if(empty())
        return false;
    *x = data[top--];
    return true;
}
template <class Item>
bool Stack<Item>::peek(Item *x) {
    if(empty())
        return false;
    *x = data[top];
    return true;
}

template <class Item>
void Stack<Item>::clear() {
    top = -1;
}

template <class Item>
void Stack<Item>::output(std::string separator) {
    for(int i = 0; i <= top; i++)
        std::cout << data[i] << separator;
    std::cout << std::endl;
}

void StackTest() {
    const int MAXN = 100;
    Stack<int> s;
    for(int i = 0; i < MAXN; i++) 
        s.push(i);
    int x;
    for(int i = 0; i < MAXN / 2; i++) 
        s.pop(&x);
    s.output();
}