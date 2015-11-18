#ifndef CP_H
#define CP_H
#include<iostream>
#include<QString>
#include<vector>
#include"disco.h"
#include<QGraphicsSimpleTextItem>
#include<stdio.h>

using namespace std;

class CP
{
public:
    vector<string>parse;
    vector<string>list_disk;
    string actual;
    string delimitador;
    QString logica();
    void setActual(QString le);

    CP();
private:
    Disco *disk;
    string DISK_PATH="DISKS/";
    string current, dir, old_dir="Root", old;
    int cont=0;
    bool m;
    QString parseLogic(vector<string> parse);
};

#endif // CP_H
