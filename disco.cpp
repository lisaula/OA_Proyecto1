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
    a.directos[0]=FS_blockused+=1;
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
    block_utilized=0;
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

    bitmap_size =sb.cantofblock/8;
    char *buffer2 = new char[bitmap_size];
    in.read(buffer2,bitmap_size*sizeof(char));
    bitmap = new char[bitmap_size];
    memcpy((bitmap),buffer2,bitmap_size*sizeof(char));

    bit_inode_size =(sb.cantofinode/8);
    char *buffer3 = new char[bit_inode_size];
    in.read(buffer3,bit_inode_size*sizeof(char));
    bitmap_inode = new char[bit_inode_size];
    memcpy((bitmap_inode),buffer3,bit_inode_size*sizeof(char));

    char *b = new char[sizeof(FileTable_d)];
    ft_array = new FileTable_d[sb.cantofinode];
    FileTable temp;
    for(int i =0; i < sb.cantofinode;i++){
        in.read(b,sizeof(FileTable_d));
        memcpy((&temp),b,sizeof(FileTable_d));
        ft_array[i]=temp;
    }
    FS_size= sizeof(superBlock)+bitmap_size*sizeof(char)+bit_inode_size*sizeof(char)+(sizeof(FileTable_d)*sb.cantofinode)+
            (sizeof(inode_d)*sb.cantofinode);
    blocksize=sb.sizeofblock;
    FS_blockused=FS_size/blocksize;
    FS_blockused+=2;
    block_utilized=sb.cantofblock-sb.freeblock;
//    cout<<"PRUEBAAAAAAAA"<<endl;
//    cout<<"FS_blockused "<<FS_blockused<<endl;
//    cout<<ft_array[0].name<<endl;
    in.close();
    return true;
}

vector<double> Disco::fileVerification(double size_of_file)
{
    cout<<"ENtro"<<endl;
    cout<<"SIZE OF BLOcks "<<sb.sizeofblock<<endl;
    double blocks= sb.sizeofblock/4;
    cout<<"BLOCKS "<<blocks<<endl;
    double aditional_blocks=0;
    double data_blocks = ceil(size_of_file/sb.sizeofblock);
    vector<double>caso;
    cout<<"Data blocks "<<data_blocks<<endl;
    if(data_blocks <= 10){
        cout<<"ADITIONAL BLOCKS "<<aditional_blocks<<endl;
        caso.push_back(data_blocks);
        caso.push_back(aditional_blocks);
        caso.push_back(0);
        return caso;
    }else if(data_blocks <=(10+blocks)){
        cout<<"Entro IS"<<endl;
        aditional_blocks+=1;
        cout<<"ADITIONAL BLOCKS "<<aditional_blocks<<endl;
        caso.push_back(data_blocks);
        caso.push_back(aditional_blocks);
        caso.push_back(1);
        return caso;
    }else if(data_blocks <= (10+(1+blocks)*blocks)){
        cout<<"Entro ID"<<endl;
        double temp = data_blocks-10-blocks;
        aditional_blocks=ceil(temp/blocks)+2;
        cout<<"ADITIONAL BLOCKS "<<aditional_blocks<<endl;
        caso.push_back(data_blocks);
        caso.push_back(aditional_blocks);
        caso.push_back(2);
        return caso;
    }else if(data_blocks <= (10 + blocks + pow(blocks,2) + pow(blocks,3) )){
        cout<<"Entro IT"<<endl;
        cout<<blocks<<" x2 "<<pow(blocks,2)<<" x3 "<<pow(blocks,3)<<endl;
        double temp = data_blocks-10-blocks - pow(blocks,2);
        cout<<temp<<" temp"<<endl;
        aditional_blocks = ceil(temp/pow(blocks,2)) + ceil(temp/pow(blocks,1)) + 3 + blocks ;
        cout<<"ADITIONAL BLOCKS "<<aditional_blocks<<endl;
        caso.push_back(data_blocks);
        caso.push_back(aditional_blocks);
        caso.push_back(3);
        return caso;
    }else{
        caso.push_back(0);
        caso.push_back(0);
        caso.push_back(-1);
        return caso;
    }
    cout<<aditional_blocks+data_blocks<<" total"<<endl;
}

bool Disco::guardararchivo(string nombre_disco, double size, string nombre_archivo)
{
    double size_b = (size*1024)*1024;
    vector<double>needed = fileVerification(size_b);
    cout<<"devolvio "<<needed[0]+needed[1]<<endl;
    vector<double> blocksindex;
    blocksindex = this->getfreeblocks(needed[0]+needed[1]);
    if(blocksindex[0]==-1){
        return false;
    }else{
        string path="DISKS/";
        nombre_disco+=".dat";
        path+=nombre_disco;
        //getting inodes and filetable
        char *buffer = new char[sizeof(inode_d)];
        int inode_pos = nextAvailable(this->bitmap_inode,false);        
        double move_to = sizeof(superBlock_d)+(bitmap_size*sizeof(char))+(bit_inode_size*sizeof(char))+sizeof(FileTable_d)*sb.cantofinode
                +inode_pos*sizeof(inode_d);
        ifstream in(path.c_str(), ios::in | ios::out | ios::binary);
        if(!in){
            cout<<"error al intentar abrir el disco"<<endl;
            return false;
        }
        in.seekg(move_to,ios::beg);
        in.read(buffer,sizeof(inode_d));
        inode_d inodo;
        memcpy((&inodo),buffer,sizeof(inode_d));
        in.close();
        int filetable_pos = getNextFreeFileTable();
        //writing file
        char *file = new char[(int)size_b];
        memset(file,'k',size_b*sizeof(char));
        inodo.blockuse=needed[0]+needed[1];
        inodo.filesize=size_b;
        strcpy(ft_array[filetable_pos].name,nombre_archivo.c_str());
        ft_array[filetable_pos].inode_index=inode_pos;
        ofstream out(path.c_str(), ios::in | ios::out | ios::binary);
        if(needed[2]==0){
            do{

            }while(size_b>0);
        }

    }
//    if(caso==1){
//        return "Archivo guardado exitosamente en caso 1 - "+QString::number(size);
//    }else if(caso ==2){
//        return "Archivo guardado exitosamente en caso 2 - "+QString::number(size);
//    }else if(caso ==3){
//        return "Archivo guardado exitosamente en caso 3 - "+QString::number(size);
//    }else if(caso ==4){
//        return "Archivo guardado exitosamente en caso 4 - "+QString::number(size);
//    }else{
//        return "Archivo demasiado grande para guardar en este FILE SYSTEM";
//    }
//    ifstream in(path.c_str(),ios::in | ios::out | ios::binary);
//    if(!in){
//        return false;
//    }else{
//        int next_bitmap=nextAvailable(bitmap,true);
//        int bit_i= nextAvailable(bitmap_inode,false);

//    }
//    in.close();
    return true;
}

int Disco::nextAvailable(char* bitmap, bool BT_BIF)
{
    if(BT_BIF){
        for(int i =FS_blockused; i<bitmap_size*sizeof(char);i++){
            if(!is_block_in_use(bitmap,i)){
                return i;
            }
        }
    }else{
        for(int i =0; i<bit_inode_size*sizeof(char);i++){
            if(!is_block_in_use(bitmap,i)){
                return i;
            }
        }
    }
    return -1;
}

vector<double> Disco::getfreeblocks(double blocksneeded)
{
    vector<double> array;
    double num;
    for(int i =0; i<blocksneeded;i++){
        num = nextAvailable(bitmap,true);
        if(num >=0){
            array[i]=num;
        }else{
            array[0]=-1;
            break;
        }
    }
    return array;

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

int Disco::getNextFreeFileTable()
{
    FileTable_d temp;
    for(int i =0; i<sb.cantofinode;i++){
        temp=ft_array[i];
        if(temp.inode_index==-1){
            return i;
        }
    }
    return -1;
}

bool Disco::write(char *buffer, double init, double byte_size, string path)
{
    ofstream out(path.c_str(), ios::in | ios::out | ios::binary);
    if(!out){
        return false;
    }else{
        double i = FS_size+(init*sb.sizeofblock);
        out.seekp(i, ios::beg);
        out.write(buffer,byte_size);
        out.close();
    }
    return true;
}

