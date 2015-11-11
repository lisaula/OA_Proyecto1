#ifndef DISCO_H
#define DISCO_H
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include<QDir>
#include<math.h>

using namespace std;

typedef struct superBlock{
    char name[10];
    double size;
    double freespace;
    int cantofblock;
    int freeblock;
    int cantofinode;
    int freeinode;
    int sizeofblock;
    char endsb [3];
}superBlock_d;

typedef struct inode{
    char permisos[10];
    int directos[10];
    double filesize;
    double blockuse;
    int indirectossimples;
    int indirectosdobles;
    int indirectostriples;
}inode_d;

typedef struct FileTable{
    char name[20];
    int inode_index;

}FileTable_d;

class Disco
{
public:
    bool is_block_in_use(char* bitmap, int blocknum);
    void setBlock_use(char* bitmap, int blocknum);
    void setBlock_unuse(char* bitmap, int blocknum);
    double FS_size;
    int FS_blockused,bit_inode_size,bitmap_size;
    bool mount(string nombre);
    vector<double> fileVerification(double size_of_file);
    bool guardararchivo(string nombre_disco, double size, string nombre_archivo);
    int nextAvailable(char *bitmap, bool BT_BIF);
    vector<double> getfreeblocks(double blocksneeded);
    QString getSP(string nombre);
    char name[10];
    double disksizemb, disksizebyte;
    int getNextFreeFileTable();
    int blocksize; double block_utilized;
    char block;
    bool write(char* buffer, double init, double byte_size, string path);
    superBlock_d sb;
    char *bitmap;
    char *bitmap_inode;
    FileTable_d *ft_array;
    bool crearDisco(string nombre, double disksizeMb, double blocksizeB);
    Disco();
    bool writeIS(char* buffer, double size_buffer, string diskname, vector<double>blocksindex);
};

#endif // DISCO_H
