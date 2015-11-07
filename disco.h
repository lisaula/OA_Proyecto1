#ifndef DISCO_H
#define DISCO_H
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>

using namespace std;

typedef struct superBlock{
    char name[10];
    int size;
    int cantofblock;
    int freeblock;
    int freespace;
    int cantofinode;
    int freeinode;
    int sizeofblock;
    char endsb [3];
}superBlock_d;

typedef struct inode{
    char permisos[10];
    int filesize;
    int blockuse;
    int directos[10];
    int indirectossimples;
    int indirectosdobles;
    int indirectostriples;

}inode_d;

class Disco
{
public:
    char name[10];
    int disksizemb, blocksize, disksizebyte;
    char block;
    bool crearDisco(string nombre, int disksizeMb, int blocksizeB);
    Disco();
};

#endif // DISCO_H
