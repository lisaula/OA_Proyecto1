#include "node.h"

Node::Node(char name[20], int i)
{
    cout<<"Cons"<<endl;
    cout<<name<<endl;
    for(int i =0; i<20;i++){
        this->name[i]=name[i];
    }
    index_inarray = i;
    hash = hashCode(name);
}

int Node::hashCode(string text)
{
    int hash = 0, strlen = text.length(), i;
    char character;
    if (strlen == 0)
        return hash;
    for (i = 0; i < strlen; i++) {
        character = text[i];
        hash = (31 * hash) + int(character);
    }
    return hash;
}


