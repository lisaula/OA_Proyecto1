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
    double freeblock;
    double freeinode;
    int cantofinode;
    int cantofblock;
    int sizeofblock;
    char endsb [3];
}superBlock_d;

typedef struct inode{
    char permisos[10];
    double directos[10];
    double filesize;
    double blockuse;
    double indirectossimples;
    double indirectosdobles;
    double indirectostriples;
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
    int nextAvailable(char *&bitmap, bool BT_BIF);
    vector<double> getfreeblocks(double blocksneeded);    //bool writeIS(char * file, double size_file, string diskname, vector<double>blocksindex, inode_d &inodo, double B_IS);
    QString getSP(string nombre);
    char name[10];
    double disksizemb, disksizebyte;
    int getNextFreeFileTable();
    int blocksize; double block_utilized;
    bool rm(string nombre);
    vector<double>getAllUsedBlocks(inode_d inodo);
    bool writeBloque(char * bloque, inode_d &inodo, double size, string permisos);
    bool write(char* buffer, double init, double byte_size);
    superBlock_d sb;
    char *bitmap;
    double global_index;
    char *bitmap_inode;
    inode_d global;
    vector<double> getUsedBloques(inode_d inodo);
    void getData(char* &buffer, vector<double>bloques, double size);
    void memtransbuffer(char *&dest, char *src, double init, double size_src);
    double global_pos=0;
    bool nameExist(string nombre);
    void reWriteFT(string nombre);
    bool exportFile(string nombre);
    string path;
    bool crearBloqueFT();
    bool mkDir(string nombre);
    bool cd(string nombre);
    bool cp(string nombre, string path, string new_name);
    bool addFile(string direccion, string nombre);
    QString ls();
    inode_d seekInode(double num, string path);
    bool mkFile(double size_file, string file_name);
    void setFiletables(double numBlock, string path);
    FileTable_d *ft_array;
    bool crearDisco(string nombre, double disksizeMb, double blocksizeB);
    int getFreePosInArray(double * array, int size);
    double seek(string nombre);
    vector<FileTable_d *> getFTfromDir(char * buffer, double size);
    void saveFileInDir(string nombre, double inodenumdir);
    double seekByIndexInode(double index);
    void memcpybuffer(char *&dest, char* src, int sizeblock, double init, double size_src);
    bool writeFile(char *file, double size_file, string diskname, vector<double>blocksindex, inode_d &inodo, vector<double> needed);
    bool read(char *&buffer, double init, double bytesToRead);
    int getLastIDIS(double *ID, double *&IS, int x);
    Disco();
};

#endif // DISCO_H
