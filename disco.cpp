#include "disco.h"

bool Disco::crearDisco(string nombre, double disksizeMb, double blocksizeB)
{
    string path="DISKS/";
    strcpy(name, nombre.c_str());
    nombre+=".dat";
    path+=nombre;
    this->disksizemb=disksizeMb;
    this->blocksize=blocksizeB;
    disksizebyte = (disksizemb*1024)*1024;
    char block[blocksize];
    int cantofblock = disksizebyte / blocksize;
    cout<<"Writing Blocks # "<<cantofblock << endl;
    memset(block,'0',blocksize);
    cout<<"paso"<<endl;
    ofstream output_file(path.c_str(),ios::binary);
    for(int i=0;i<cantofblock;i++){
    output_file.write((char*)block,sizeof(block));
    }
    output_file.close();

    output_file.open(path.c_str(), ios::in | ios::out | ios::binary);
    //escribiendo SUPER BLOCK
    superBlock_d SP;
    SP.cantofblock = cantofblock;
    SP.cantofinode = (disksizebyte*0.05)/sizeof(inode_d);
    SP.freeblock = cantofblock;
    SP.freeinode = SP.cantofinode-1;
    SP.freespace = disksizebyte - sizeof(superBlock_d);
    strcpy(SP.name, name);
    SP.size = disksizebyte;
    SP.sizeofblock = blocksize;
    strcpy(SP.endsb, "FIN");
    cout<<"writing Super Block with size: "<<sizeof(SP)<<endl;
    cout<<"Cant of INODE "<<SP.cantofinode<<endl;
    cout<<"disk size "<<SP.size<<endl;
    output_file.write((char*)&SP,sizeof(superBlock));

    //ESCRIBIENDO mapa de bits
    int bitmapsize = cantofblock/8;
    char bitmap [bitmapsize];
    memset(bitmap,0,bitmapsize);
    cout<<"writing Bitmap of "<<bitmapsize<<" long"<<endl;
    output_file.write(((char*)bitmap),sizeof(bitmap));

    //ESCRIBIENDO mapa de  inodes
    int bitmapInodesize = SP.cantofinode/8;
    char bitmapInode [bitmapInodesize];
    memset(bitmapInode,0,bitmapInodesize);
    cout<<"writing Inode Bitmap of "<<bitmapInodesize<<" long"<<endl;
    output_file.write(((char*)bitmapInode),sizeof(bitmapInode));

    //ESCRIBIENDO FILE TABLE
    FileTable_d ft;
    strcpy(ft.name, "root");
    ft.inode_index=0;
    output_file.write(((char*)&ft),sizeof(FileTable_d));
    memset(ft.name,0,20);
    ft.inode_index=-1;
    for(int x =0; x<SP.cantofinode-1;x++){
        output_file.write((char*)&ft,sizeof(FileTable_d));
    }

    //ESCRIBIENDO INODES
    FS_size= sizeof(superBlock)+sizeof(bitmap)+sizeof(bitmapInode)+(sizeof(FileTable_d)*SP.cantofinode)+
            (sizeof(inode_d)*SP.cantofinode);
    FS_blockused=FS_size/blocksize;
    FS_blockused+=1;
    inode_d a;
    strcpy(a.permisos,"drwxrwxrwx");
    a.blockuse=1;
    a.filesize=0;
    memset(a.directos,-1,10);
    a.directos[0]=FS_blockused+1;
    a.indirectossimples=-1;
    a.indirectosdobles=-1;
    a.indirectostriples=-1;
    output_file.write((char*)&a,sizeof(inode_d));

    strcpy(a.permisos,"-rwxrwxrwx");
    a.blockuse=0;
    a.directos[0]=-1;

    for(int x =0; x<SP.cantofinode-1;x++){
        output_file.write((char*)&a,sizeof(inode_d));
    }

    //Actualizando superbloc;
    output_file.seekp(0,ios::beg);
    SP.freeblock = cantofblock-FS_blockused;
    SP.freespace = disksizebyte - FS_size;
    output_file.write((char*)&SP,sizeof(superBlock));

    //actualizando bitmap
    for(int i =0; i<FS_blockused;i++){
        setBlock_use(bitmap,i);
    }
    output_file.write(((char*)bitmap),sizeof(bitmap));
    //actualizando bitmap inodes
    setBlock_use(bitmapInode,0);
    output_file.write(((char*)bitmapInode),sizeof(bitmapInode));
    output_file.close();
    return true;
}

Disco::Disco()
{
    cout<<"Size of name"<<sizeof(name)<<endl;
    for(int i =0; i<sizeof(name); i++){
        name[i]=' ';
    }
    //QDir qdir;
    //qdir.mkdir("DISKS");
    FS_size=0;
    FS_blockused=0;
    disksizemb=0;
    blocksize=0;
    disksizebyte=0;
}

bool Disco::is_block_in_use(char* bitmap, int blocknum)
{
    int indexonbitmap = blocknum / 8;
    int posinchar = blocknum%8;

     if(bitmap[indexonbitmap] & 1<<posinchar){
         return true;
     }else{
         return false;
     }

}

void Disco::setBlock_use(char* bitmap, int blocknum){
    int indexonbitmap = blocknum / 8;
    int posinchar = blocknum%8;
    bitmap[indexonbitmap] = bitmap[indexonbitmap] | 1 << posinchar;
}

void Disco::setBlock_unuse(char* bitmap, int blocknum){
    int indexonbitmap = blocknum / 8;
    int posinchar = blocknum%8;
    bitmap[indexonbitmap]= bitmap[indexonbitmap] & ~(1<<posinchar);
}

bool Disco::mount(string nombre)
{
    string path="DISKS/";
    nombre+=".dat";
    path+=nombre;
    ifstream in(path.c_str(),ios::in | ios::out | ios::binary);
    if(!in){
        return false;
    }
    char *buffer = new char[sizeof(superBlock_d)];
    in.read(buffer,sizeof(superBlock_d));
    memcpy((&sb),buffer,sizeof(superBlock_d));

    int bitmap_size =sb.cantofblock/8;
    char *buffer2 = new char[bitmap_size];
    in.read(buffer2,bitmap_size*sizeof(char));
    memcpy((&bitmap),buffer2,bitmap_size*sizeof(char));

    int bit_inode_size =(sb.cantofinode/8);
    char *buffer3 = new char[bit_inode_size];
    in.read(buffer3,bit_inode_size*sizeof(char));
    memcpy((&bitmap_inode),buffer3,bit_inode_size*sizeof(char));

    char *b = new char[sizeof(FileTable_d)];
    ft_array = new FileTable_d[sb.cantofinode];
    FileTable temp;
    for(int i =0; i < sb.cantofinode;i++){
        in.read(b,sizeof(FileTable_d));
        memcpy((&temp),b,sizeof(FileTable_d));
        ft_array[i]=temp;
    }
    cout<<"PRUEBAAAAAAAA"<<endl;
    cout<<ft_array[0].name<<endl;
    in.close();
    //THIS PROGRAM HAS UNexpectedly finished
    return true;
}

bool Disco::guardararchivo(string nombre, double size)
{string path="DISKS/";
    nombre+=".dat";
    path+=nombre;
    ifstream in(path.c_str(),ios::in | ios::out | ios::binary);
    if(!in){
        return false;
    }else{
        char *buffer = new char[sizeof(superBlock_d)];
        in.read(buffer,sizeof(superBlock_d));
        superBlock_d sp;
        memcpy((&sp),buffer,sizeof(superBlock_d));

        char *buffer2 = new char[(sp.cantofblock/8)];
        char bitmap[sp.cantofblock/8];
        in.read(buffer2,sizeof(bitmap));
        memcpy((&bitmap),buffer2,sizeof(bitmap));

        char *buffer3 = new char[(sp.cantofinode/8)];
        char bitmap_inode[sp.cantofinode/8];
        in.read(buffer3,sizeof(bitmap_inode));
        memcpy((&bitmap_inode),buffer3,sizeof(bitmap_inode));
        in.close();


    }
    double size_b = (size*1024)*1024;
}

int Disco::nextAvailable()
{
    int pos=1;
}

QString Disco::getSP(string nombre)
{
    string path="DISKS/";
    nombre+=".dat";
    path+=nombre;
    ifstream in(path.c_str(),ios::in | ios::out | ios::binary);
    if(!in){
        return "ERROR AL INTENTAR ABRIR ARCHIVO";
    }else{
        char *buffer = new char[sizeof(superBlock_d)];
        in.read(buffer,sizeof(superBlock_d));
        in.close();

        superBlock_d sp;
        memcpy((&sp),buffer,sizeof(superBlock_d));

        QString text=QString("Name: %1\nSize: %2B\nBlock Quantity: %3\nFree Blocks: %4\nFree Space: %5B\nSize of Block %6B\nInodes Quantity %7\nFree Inodes %8\n").
                arg(sp.name).arg(sp.size).arg(sp.cantofblock).arg(sp.freeblock).arg(sp.freespace).arg(sp.sizeofblock).arg(sp.cantofinode).arg(sp.freeinode);
        return text;
    }
}

