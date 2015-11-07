#ifndef CP_H
#define CP_H
#include<iostream>
#include<QString>
#include<vector>
#include"disco.h"
#include<QGraphicsSimpleTextItem>
using namespace std;

class CP
{
public:
    vector<string>parse;
    string actual;
    string delimitador;
    QString logica();
    void setActual(QString le);

    CP();
private:
    Disco *disk;
    QString parseLogic(vector<string> parse);
};

#endif // CP_H
