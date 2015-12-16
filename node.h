#ifndef NODE_H
#define NODE_H
#include<iostream>
using namespace std;

class Node
{
public:
    char name[20];
    int index_inarray;
    int hash;
    Node(char name[20],int i);
    int hashCode(string text);
};

#endif // NODE_H
