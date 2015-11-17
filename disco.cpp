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
    FS_blockused=ceil(FS_size/blocksize);
    cout<<"FS ceil "<<FS_blockused<<endl;
    //FS_blockused+=1;
    inode_d a;
    strcpy(a.permisos,"drwxrwxrwx");
    a.blockuse=1;
    a.filesize=0;
    //memset(a.directos,-1,10);
    a.directos[0]=FS_blockused;a.directos[1]=-1;
    a.directos[2]=-1;a.directos[3]=-1;
    a.directos[4]=-1;a.directos[5]=-1;
    a.directos[6]=-1;a.directos[7]=-1;
    a.directos[8]=-1;a.directos[9]=-1;
    a.indirectossimples=-1;
    a.indirectosdobles=-1;
    a.indirectostriples=-1;
    //setFiletables(FS_blockused,path);
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
    cout<<"set blocks en uso del FS "<<FS_blockused<<endl;
    for(int i =0; i<=FS_blockused;i++){
        setBlock_use(bitmap,i);
    }
    output_file.write((bitmap),sizeof(bitmap));
    //actualizando bitmap inodes
    setBlock_use(bitmapInode,0);
    output_file.write(((char*)bitmapInode),sizeof(bitmapInode));
    output_file.close();
    cout<<"Fin crear disco"<<endl;
    return true;
}

int Disco::getFreePosInArray(double *array, int size)
{
    for(int i =0; i< size; i++ ){
        if(array[i]<0){
            return i;
        }
    }
    return -1;
}

double Disco::seek(string nombre)
{
    for(int i =0; sb.cantofinode;i++){
        if(strcmp(ft_array[i].name,nombre.c_str())==0){
            return ft_array[i].inode_index;
        }
    }
    return -1;
}

void Disco::saveFileInDir(string nombre, double inodenumdir, double inodenumFile, string path)
{
    //aun no terminada
    ifstream in(path.c_str(), ios::in | ios::out | ios::binary);
    double move_to = sizeof(superBlock_d)+(bitmap_size*sizeof(char))+(bit_inode_size*sizeof(char))+(sizeof(FileTable_d)*sb.cantofinode)
            +(inodenumdir*sizeof(inode_d));
    char* buffer = new char[sizeof(inode_d)];
    in.seekg(move_to,ios::beg);
    in.read(buffer,sizeof(inode_d));
    inode_d a;
    memcpy((&a),buffer,sizeof(inode_d));

    char *ftbuffer = new char[sizeof(FileTable_d)];
    FileTable_d f;
    bool found=false;
    for(int i =0; i<10;i++){
        if(a.directos[i]!=-1){
            move_to = sizeof(superBlock_d)+(bitmap_size*sizeof(char))+(bit_inode_size*sizeof(char))+(sizeof(FileTable_d)*sb.cantofinode)
                        +(a.directos[i]*sizeof(inode_d));
            in.seekg(move_to,ios::beg);
            for(int m=0; m<floor(sb.sizeofblock/sizeof(FileTable_d));m++){
                in.read(ftbuffer,sizeof(FileTable_d));
                memcpy((&f),ftbuffer,sizeof(FileTable_d));
                if(f.inode_index==-1){
                    strcpy(f.name,nombre.c_str());
                    f.inode_index=inodenumFile;
                    ofstream out(path.c_str(),ios::in | ios::out | ios::binary);
                    out.seekp(in.tellg(),ios::beg);
                    out.seekp(-sizeof(FileTable_d),ios::cur);
                    out.write((char*)&f,sizeof(FileTable_d));
                    out.close();
                    found=true;
                    in.close();
                    break;
                }
            }
            if(found)
                break;
        }
    }
    if(!found){
        if(a.indirectossimples!=-1){

        }
    }
    //aun no terminada
    in.close();
}

bool Disco::createDir(string diskname, string nombreDir)
{

}

void Disco::memcpybuffer(char *&dest, char *src, int sizeblock, double init , double size_src)
{
    memset(dest,'F',sizeblock*sizeof(char));
    for(int i =0; i< sizeblock;i++){
        if(size_src==init+i){
            cout<<"REtorno size src "<<size_src<<" > init "<<init<<" +i"<<+i<<endl;
            return;
        }
        dest[i]=src[(int)(init+i)];
    }
}

bool Disco::writeFile(char *file, double size_file, string diskname, vector<double> blocksindex, inode_d &inodo, vector<double>needed)
{
    ofstream out(diskname.c_str(), ios::in | ios:: out | ios::binary);
    cout<<"Size entrada "<<size_file<<endl;
    double size_entrada=size_file;
    double bloques = blocksindex.size();
    char * buffer = new char[sb.sizeofblock];
    memcpybuffer(buffer,file,sb.sizeofblock,0,size_file);
    int x = sb.sizeofblock/8;
    double alreadysave=0;
    if(!out){
        return false;
    }else{
        double move_to=0;
        int cont=0;
        //(int)needed[2] aun no necesitada
        switch (cont) {
        case 0://DS
            for(int i =0; i< 10 ;i++){
                if(size_file<=0)
                    break;

                move_to = (blocksindex[0]*sb.sizeofblock);
                out.seekp(move_to, ios::beg);
                cout<<"MOVE TO DS "<<out.tellp()<<endl;
                inodo.directos[i]=blocksindex[0];
                blocksindex.erase(blocksindex.begin());
                out.write(buffer,sb.sizeofblock*sizeof(char));
                cout<<alreadysave<<" already saved"<<endl;
                alreadysave+=sb.sizeofblock;
                cout<<alreadysave<<" already saved"<<endl;
                memcpybuffer(buffer,file,sb.sizeofblock,alreadysave,size_entrada);
                cout<<"Size of file al inicio "<<size_file<<endl;
                size_file-=sb.sizeofblock;
                cout<<"Size of file "<<size_file<<endl;
            }
        case 1://IS
            if(size_file>0){
                inodo.indirectossimples = blocksindex[0];
                double IS_pos=(blocksindex[0]*sb.sizeofblock);
                cout<<"IS_POS en caso 1 "<<IS_pos<<endl;
                cout<<"Bloque "<<blocksindex[0]<<endl;
                blocksindex.erase(blocksindex.begin());
                double DS[x];
                //memcpybuffer(buffer,file,sb.sizeofblock,alreadysave,size_file);
                for(int i =0 ; i <x ;i++ ){
                    if(size_file<=0)
                        break;
                    move_to = (blocksindex[0]*sb.sizeofblock);
                    out.seekp(move_to, ios::beg);
                    cout<<"MOVE TO IS"<<out.tellp()<<endl;
                    DS[i]=blocksindex[0];
                    blocksindex.erase(blocksindex.begin());
                    out.write(buffer,sb.sizeofblock*sizeof(char));
                    cout<<alreadysave<<" already saved"<<endl;
                    alreadysave+=sb.sizeofblock;
                    cout<<alreadysave<<" already saved"<<endl;
                    memcpybuffer(buffer,file,sb.sizeofblock,alreadysave,size_entrada);
                    cout<<"Size of file al inicio "<<size_file<<endl;
                    size_file-=sb.sizeofblock;
                    cout<<"Size of file "<<size_file<<endl;
                }
                out.seekp(IS_pos,ios::beg);
                out.write((char*)&DS,sizeof(DS));
//                cout<<"X "<<x<<endl;
//                cout<<"sizeof DS "<<sizeof(DS)<<endl;
            }

        case 2: //ID
            if(size_file>0){
                inodo.indirectosdobles = blocksindex[0];
                double ID_pos=(blocksindex[0]*sb.sizeofblock);
                cout<<"ID_POS en caso 2 "<<ID_pos<<endl;
                cout<<"Bloque "<<blocksindex[0]<<endl;
                blocksindex.erase(blocksindex.begin());
                double ID[x];
                for(int i =0;i<x;i++){
                    if(size_file<=0)
                        break;

                    ID[i]=blocksindex[0];

                    double IS[x];
                    double IS_pos=blocksindex[0]*sb.sizeofblock;
                    cout<<"IS_POS en caso 2 "<<IS_pos<<endl;
                    cout<<"Bloque "<<blocksindex[0]<<endl;
                    blocksindex.erase(blocksindex.begin());
                    for(int m =0; m< x; m++){
                        if(size_file<=0)
                            break;
                        move_to = (blocksindex[0]*sb.sizeofblock);
                        out.seekp(move_to, ios::beg);
                        cout<<"MOVE TO ID"<<out.tellp()<<endl;
                        IS[m]=blocksindex[0];
                        blocksindex.erase(blocksindex.begin());
                        cout<<blocksindex.size()<<" blocks index size"<<endl;
                        out.write(buffer,sb.sizeofblock*sizeof(char));
                        cout<<alreadysave<<" already saved"<<endl;
                        alreadysave+=sb.sizeofblock;
                        cout<<alreadysave<<" already saved"<<endl;
                        memcpybuffer(buffer,file,sb.sizeofblock,alreadysave,size_entrada);
                        cout<<"Size of file al inicio "<<size_file<<endl;
                        size_file-=sb.sizeofblock;
                        cout<<"Size of file "<<size_file<<endl;
                    }
                    out.seekp(IS_pos,ios::beg);
                    out.write((char*)&IS,sizeof(IS));
                }
                out.seekp(ID_pos,ios::beg);
                out.write((char*)&ID,sizeof(ID));
        }
            cout<<"ID "<<inodo.indirectosdobles<<endl;
        case 3://IT
            if(size_file>0){
                inodo.indirectostriples = blocksindex[0];
                double IT_pos=(blocksindex[0]*sb.sizeofblock);
                cout<<"IT_POS en caso 3 "<<IT_pos<<endl;
                cout<<"Bloque "<<blocksindex[0]<<endl;
                blocksindex.erase(blocksindex.begin());
                double IT[x];
                for(int i =0;i<x;i++){
                    if(size_file<=0)
                        break;

                    IT[i]=blocksindex[0];

                    double ID[x];
                    double ID_pos=blocksindex[0]*sb.sizeofblock;
                    cout<<"ID_POS en caso 3 "<<ID_pos<<endl;
                    cout<<"Bloque "<<blocksindex[0]<<endl;
                    blocksindex.erase(blocksindex.begin());
                    for(int m =0; m< x; m++){
                        if(size_file<=0)
                            break;

                        ID[m]=blocksindex[0];

                        double IS[x];
                        double IS_pos= blocksindex[0]*sb.sizeofblock;
                        blocksindex.erase(blocksindex.begin());

                        for(int y =0;y<x;y++){
                            if(size_file<=0)
                                break;
                        move_to = (blocksindex[0]*sb.sizeofblock);
                        out.seekp(move_to, ios::beg);
                        cout<<"MOVE TO IT"<<out.tellp()<<endl;
                        IS[y]=blocksindex[0];
                        blocksindex.erase(blocksindex.begin());
                        cout<<blocksindex.size()<<" blocks index size"<<endl;
                        out.write(buffer,sb.sizeofblock*sizeof(char));
                        cout<<alreadysave<<" already saved"<<endl;
                        alreadysave+=sb.sizeofblock;
                        cout<<alreadysave<<" already saved"<<endl;
                        memcpybuffer(buffer,file,sb.sizeofblock,alreadysave,size_entrada);
                        cout<<"Size of file al inicio "<<size_file<<endl;
                        size_file-=sb.sizeofblock;
                        cout<<"Size of file "<<size_file<<endl;
                        }
                        out.seekp(IS_pos,ios::beg);
                        out.write((char*)&IS,sizeof(IS));
                    }
                    out.seekp(ID_pos,ios::beg);
                    out.write((char*)&ID,sizeof(ID));
                }
                out.seekp(IT_pos,ios::beg);
                out.write((char*)&IT,sizeof(IT));
        }
            cout<<"IT "<<inodo.indirectosdobles<<endl;
            break;
        default:
            cout<<"Archivo demasiado grande para guardar en el inodo"<<endl;
            break;
        }
        out.close();
        cout<<"Size of file "<<size_file<<endl;
        cout<<"Imprime directos simples dentro de writeDS"<<endl;
        for(int i =0; i<10;i++){
            cout<<inodo.directos[i]<<" "<<ends;
        }
        cout<<endl;
        cout<<"Imprime IS "<<inodo.indirectossimples<<endl;
    }
    return true;

}

bool Disco::read(char * &buffer, double init, double bytesToRead)
{
    ifstream in(path.c_str(), ios::in | ios::out | ios::binary);
    in.seekg(init,ios::beg);
    in.read(buffer,bytesToRead);
    in.close();
}

int Disco::getLastIDIS(double *ID, double *&IS, int x)
{
    char *buf = new char[x*sizeof(double)];
    bool t=false;
    for(int i =0; i < x;i++){
        cout<<"cont i "<<i<<endl;
        cout<<"No entra "<<ID[i]<<endl;
        if(ID[i]!=-1){
            cout<<"Entro"<<endl;
            read(buf,ID[i]*sb.sizeofblock,sb.sizeofblock);
            cout<<"PASOOOOO"<<endl;
            memcpy((IS),buf,x*sizeof(double));
            cout<<"Paso "<<endl;
            for(int m =0; m <x;m++){
                cout<<"cont m "<<m<<endl;
                if(IS[m]==-1){
                    return i;
                }
            }
        }else{
            cout<<"Retorno esto "<<i<<endl;
            return i;
        }
    }
    return -1;
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
    path ="";
    srand(NULL);
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
    this->path=path;
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
    FS_blockused=ceil(FS_size/blocksize);
    FS_blockused+=1;
    block_utilized=sb.cantofblock-sb.freeblock;
//    cout<<"PRUEBAAAAAAAA"<<endl;
//    cout<<"FS_blockused "<<FS_blockused<<endl;
//    cout<<ft_array[0].name<<endl;
    cout<<"FS BLOCKUSED "<<FS_blockused<<endl;
    cout<<"ES bloque numero "<<1512<<" en uso "<<is_block_in_use(bitmap,1512)<<endl;
    cout<<"ES bloque numero "<<FS_blockused-1<<" en uso "<<is_block_in_use(bitmap,FS_blockused-1)<<endl;
    cout<<"ES bloque numero "<<FS_blockused<<" en uso "<<is_block_in_use(bitmap,FS_blockused)<<endl;
    cout<<"ES bloque numero "<<FS_blockused+1<<" en uso "<<is_block_in_use(bitmap,FS_blockused+1)<<endl;
    //cout<<"NEXT AVAILABLE "<<nextAvailable(bitmap,true)<<endl;
    //borrar
    char * buftemp = new char[sb.sizeofblock];
    double n[2];
    read(buftemp,4393*sb.sizeofblock,sb.sizeofblock);
    memcpy(&n,buftemp,sb.sizeofblock);
    for(int i =0;i<2;i++){
        cout<<n[i]<<ends;
    }
    //hasta aqui
    in.close();
    cd("root");
    cout<<"global  "<<global.directos[0]<<endl;
    return true;
}

vector<double> Disco::fileVerification(double size_of_file)
{
    cout<<"ENtro"<<endl;
    cout<<"SIZE OF BLOcks "<<sb.sizeofblock<<endl;
    double blocks= sb.sizeofblock/8;
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
    double size_b = (size);
    vector<double>needed = fileVerification(size_b);
    cout<<"devolvio "<<needed[0]+needed[1]<<endl;
    if(needed[2]==-1 || sb.freeinode>0){
        cout<<"archivo muy grande para guardar en inodo o no hay inodos disponibles"<<endl;
        return false;
    }else{
        vector<double> blocksindex;
        cout<<"PASOOOOOOOOOOO1111111"<<endl;
        blocksindex = this->getfreeblocks(needed[0]+needed[1]);
        cout<<"PASOOOOOOOOOOO222222"<<endl;
        if(blocksindex[0]==-1){
            cout<<"No hay capacidad de espacio en el disco para guardar archivo"<<endl;
            return false;
        }else{
            cout<<"blocks index dentro de guardar archivo"<<endl;
            for(int i =0;i<blocksindex.size();i++){
                cout<<blocksindex[i]<<" "<<ends;
            }
            cout<<endl;
            string path="DISKS/";
            nombre_disco+=".dat";
            path+=nombre_disco;
            //getting inodes and filetable
            char *buffer = new char[sizeof(inode_d)];
            int inode_pos = nextAvailable(bitmap_inode,false);
            if(inode_pos!=-1){
                cout<<"pos del inodo "<<inode_pos<<endl;
                cout<<"esta usado inode pos "<<is_block_in_use(bitmap_inode,inode_pos);
                double move_to = sizeof(superBlock_d)+(bitmap_size*sizeof(char))+(bit_inode_size*sizeof(char))+(sizeof(FileTable_d)*sb.cantofinode)
                        +(inode_pos*sizeof(inode_d));
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
                double filetable_pos = getNextFreeFileTable();
                cout<<"pos del FT "<<filetable_pos<<endl;
                cout<<" esta usado "<<is_block_in_use(bitmap_inode,filetable_pos)<<endl;
                //writing file
                char *file = new char[(int)size_b];
                //borrar esto luego
                strcpy(file,"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"//123456789012345678901234567890123456789012345678901234567890");
                            "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
                            "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
                );
                       //hasta aqui
                //memset(file,'k',size_b*sizeof(char));
                inodo.blockuse=needed[0];
                inodo.filesize=size_b;
                strcpy(ft_array[(int)filetable_pos].name,nombre_archivo.c_str());
                ft_array[(int)filetable_pos].inode_index=inode_pos;
                if(needed[2]==0){
                    if(writeFile(file,size_b,path,blocksindex,inodo,needed)){//writeDS(file,size_b,path,blocksindex,inodo)
                        cout<<"Archivo guardado en directos simples"<<endl;
                        cout<<"Imprime directos simples afuera de writeDS"<<endl;
                        for(int i =0; i<10;i++){
                            cout<<inodo.directos[i]<<" "<<ends;
                        }
                        cout<<endl;
                    }else{
                        cout<<"Error al guardar el archivo"<<endl;
                        return false;
                    }
                }else if(needed[2]==1){
                    if(writeFile(file,size_b,path,blocksindex,inodo,needed)){//writeDS(file,size_b,path,blocksindex,inodo)
                        cout<<"Archivo guardado en Indirecto simples"<<endl;
                        cout<<"Imprime directos simples afuera de writeDS"<<endl;
                        for(int i =0; i<10;i++){
                            cout<<inodo.directos[i]<<" "<<ends;
                        }
                        cout<<endl;
                    }else{
                        cout<<"Error al guardar el archivo"<<endl;
                        return false;
                    }

                }else if(needed[2]==2){
                    if(writeFile(file,size_b,path,blocksindex,inodo,needed)){//writeDS(file,size_b,path,blocksindex,inodo)
                        cout<<"Archivo guardado en Indirecto Dobles"<<endl;
                        cout<<"Imprime directos simples afuera de writeDS"<<endl;
                        for(int i =0; i<10;i++){
                            cout<<inodo.directos[i]<<" "<<ends;
                        }
                        cout<<endl;
                    }else{
                        cout<<"Error al guardar el archivo"<<endl;
                        return false;
                    }
                }else if(needed[2]==3){
                    if(writeFile(file,size_b,path,blocksindex,inodo,needed)){//writeDS(file,size_b,path,blocksindex,inodo)
                        cout<<"Archivo guardado en Indirecto Triples"<<endl;
                        cout<<"Imprime directos simples afuera de writeDS"<<endl;
                        for(int i =0; i<10;i++){
                            cout<<inodo.directos[i]<<" "<<ends;
                        }
                        cout<<endl;
                    }else{
                        cout<<"Error al guardar el archivo"<<endl;
                        return false;
                    }
                }else{
                    cout<<"Archivo muy grande para guardar en inodo"<<endl;
                    return false;
                }
                ofstream out(path.c_str(), ios::in | ios::out | ios::binary);
                //actualizando super block
                sb.freeblock-=(needed[0]+needed[1]);
                sb.freeinode--;
                sb.freespace-=size_b+(needed[1]*sb.sizeofblock);
                out.write((char*)&sb,sizeof(superBlock_d));

                //actualizando bitmap
                out.write(bitmap,bitmap_size*sizeof(char));
                //actualizando bitmap de inodes
                out.write(bitmap_inode,bit_inode_size*sizeof(char));
                //actualizando filetable
                FileTable_d ft = ft_array[(int)filetable_pos];
                double ftb_pos = filetable_pos*sizeof(FileTable_d);
                out.seekp(ftb_pos,ios::cur);
                out.write((char*)&ft,sizeof(FileTable_d));
                //actualizando inodo
                out.seekp(move_to,ios::beg);
                out.write(((char*)&inodo),sizeof(inode_d));
                out.close();
                return true;
            }else{
                cout<<"No hay inodos disponibles"<<endl;
                return false;
            }
        }
    }
}

int Disco::nextAvailable(char* &bitmap, bool BT_BIF)
{
    if(BT_BIF){
        for(int i =FS_blockused; i<sb.cantofblock;i++){
            //cout<<"i "<<i<<ends;
            if(is_block_in_use(bitmap,i)==false){
                cout<<"DEVOLVIO EL NA "<<i<<endl;
                return i;
            }
        }
    }else{
        for(int i =0; i<sb.cantofinode;i++){
            if(is_block_in_use(bitmap,i)==false){
                cout<<"DEVOLVIO EL NA de I "<<i<<endl;
                setBlock_use(bitmap,i);
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
            array.push_back(num);
            setBlock_use(bitmap,num);
        }else{
            array.clear();
            array.push_back(-1);
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

bool Disco::writeBloque(char *bloque, inode_d &inodo, double size, string permisos)
{
    double move_to=0, free_space=0, size_file=size;
    int x = sb.sizeofblock/8;
    cout<<"ESCRIBIENDO "<<size<<endl;
    cout<<"X "<<x<<endl;
    bool nuevo=false;
    double ocupados  = inodo.filesize/sb.sizeofblock;
    strcpy(inodo.permisos,permisos.c_str());
    cout<<"File size "<<inodo.filesize<<endl;
    vector<double>blocksindex;
    cout<<"Inodo blockuse "<<inodo.blockuse<<endl;
    char *buf = new char[sb.sizeofblock];
    if(inodo.blockuse==0){
        blocksindex = getfreeblocks(1);
        move_to = (blocksindex[0]*sb.sizeofblock);
        inodo.directos[0]=blocksindex[0];
        blocksindex.erase(blocksindex.begin());
        write(bloque, move_to,size);
        cout<<"Inodo virgen "<<bloque<<endl;
        size_file -=size;
    }else{
        cout<<"I FiSize "<<inodo.filesize<<endl;
        //double ocupados  = inodo.filesize/sb.sizeofblock;
        cout<<"Ocuoados "<<ocupados<<endl;
        if(ocupados<10){
            double dif = ocupados - floor(ocupados);
            cout<<"DIF "<<dif<<endl;
            if(dif ==0 ){
                blocksindex = getfreeblocks(1);
                inodo.directos[(int)ocupados]=blocksindex[0];
                move_to = (blocksindex[0]*sb.sizeofblock);
                blocksindex.erase(blocksindex.begin());
                write(bloque,move_to,size);
                size_file -=size;
            }else{
                cout<<"Entro dif !=0"<<endl;
                free_space = sb.sizeofblock-(sb.sizeofblock*dif);
                cout<<"FREE Space "<<free_space<<endl;
                if(free_space >= size){
                    cout<<"Entro space >size "<<bloque<<endl;
                    move_to = inodo.directos[(int)ocupados]*sb.sizeofblock + dif*sb.sizeofblock;
                    write(bloque,move_to,size);
                    size_file-=size;
                }else{
                    memcpybuffer(buf,bloque,free_space,0,sb.sizeofblock);
                    cout<<"Entro space <size "<<buf<<endl;
                    move_to = inodo.directos[(int)floor(ocupados)]*sb.sizeofblock + dif*sb.sizeofblock;
                    cout<<"move to "<<move_to<<endl;
                    write(buf,move_to,free_space);
                    //temp = size-free_space;
                    size_file-=free_space;
                    if(((free_space+size_file)/sb.sizeofblock)+ocupados<10){
                        memcpybuffer(buf,bloque,sb.sizeofblock,free_space,sb.sizeofblock);
                        cout<<"Entro space <size "<<buf<<endl;
                        blocksindex = getfreeblocks(1);
                        int pos = floor(ocupados)+1;
                        inodo.directos[pos]= blocksindex[0];
                        move_to = blocksindex[0]*sb.sizeofblock;
                        blocksindex.erase(blocksindex.begin());
                        write(buf,move_to,size-free_space);
                        size_file-=(size-free_space);
                    }else{
                        nuevo = true;
                        cout<<"Directos Simples LLenos "<<endl;
                    }
                }
            }//fin indirectos simples
        }//comienza Indirectos Simples
        if(size_file>0){
            cout<<"ENtro IS"<<endl;
            blocksindex.clear();
            double IS[x];
            double IS_pos=0;
            if(inodo.indirectossimples ==-1){
                cout<<"Indirectos -1"<<endl;
                blocksindex = getfreeblocks(1);
                for(int i =0; i<x;i++){
                    IS[i]=-1;
                }
                inodo.indirectossimples= blocksindex[0];
                IS_pos = blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());
            }else{
                IS_pos = inodo.indirectossimples*sb.sizeofblock;
                char * buftem = new char[x*sizeof(double)];
                read(buftem,IS_pos,sb.sizeofblock);
                memcpy((&IS),buftem,x*sizeof(double));
            }
            for(int i =0; i< x;i++){
                cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
            }
            if(nuevo){
                cout<<"entro nuevo "<<endl;
                memcpybuffer(buf,bloque,size_file,size-size_file,sb.sizeofblock);
                blocksindex = getfreeblocks(1);
                int pos = getFreePosInArray(IS,x);
                cout<<"POS EN NUEVO "<<pos<<endl;
                IS[pos] = blocksindex[0];
                cout<<IS[pos]<<endl;
                move_to=blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());
                write(buf,move_to,size_file);
                size_file-=size_file;
                nuevo = false;
            }else{
                if(ocupados < 10 + x){
                    double dif = ocupados - floor(ocupados);
                    cout<<"DIF "<<dif<<endl;
                    ocupados -= 10;
                    if(dif ==0 ){
                        blocksindex = getfreeblocks(1);
                        IS[(int)ocupados]=blocksindex[0];
                        move_to = (blocksindex[0]*sb.sizeofblock);
                        blocksindex.erase(blocksindex.begin());
                        write(bloque,move_to,size);
                        size_file -=size;
                    }else{
                        cout<<"Entro dif !=0"<<endl;
                        free_space = sb.sizeofblock-(sb.sizeofblock*dif);
                        cout<<"FREE Space "<<free_space<<endl;
                        if(free_space >= size){
                            cout<<"Entro space >size "<<bloque<<endl;
                            cout<<(int)floor(ocupados)<<" QUE HAY DENTRO "<<IS[(int)floor(ocupados)]<<endl;
                            move_to = IS[(int)floor(ocupados)]*sb.sizeofblock + dif*sb.sizeofblock;
                            cout<<"Move to "<<move_to<<endl;
                            write(bloque,move_to,size);
                            size_file-=size;
                        }else{
                            memcpybuffer(buf,bloque,free_space,0,sb.sizeofblock);
                            cout<<"Entro space <size "<<buf<<endl;
                            cout<<ocupados<<" IS QUE HAY DENTRO "<<IS[(int)floor(ocupados)]<<endl;
                            move_to = IS[(int)floor(ocupados)]*sb.sizeofblock + dif*sb.sizeofblock;
                            cout<<"move to "<<move_to<<endl;
                            write(buf,move_to,free_space);
                            //temp = size-free_space;
                            size_file-=free_space;
                            double o = inodo.filesize/sb.sizeofblock;
                            if(((free_space+size_file)/sb.sizeofblock)+o<10 + x){
                                memcpybuffer(buf,bloque,sb.sizeofblock,free_space,sb.sizeofblock);
                                cout<<"Entro space <10 + x "<<buf<<endl;
                                blocksindex = getfreeblocks(1);
                                int pos = floor(ocupados)+1;
                                IS[pos]= blocksindex[0];
                                move_to = blocksindex[0]*sb.sizeofblock;
                                blocksindex.erase(blocksindex.begin());
                                write(buf,move_to,size-free_space);
                                size_file-=(size-free_space);
                            }else{
                                nuevo = true;
                                cout<<"LLENOS INDIRECTOS SIMPLES"<<endl;
                            }
                        }
                    }

                }
            }
            write((char*)&IS,IS_pos,sb.sizeofblock);
        }//aqui comienza Indirectos Dobles
        if(size_file>0){
            cout<<"ENtro ID"<<endl;
            blocksindex.clear();
            cout<<"X dentro ID "<<x<<endl;
            double ID[x];
            double ID_pos=0;
            double IS[x];
            double IS_pos=0;
            int array_pos=0;
            int array_ISpos=0;
            double difID=0;
            ocupados -= 10+x;
            char *buf = new char[x*sizeof(double)];
            bool t=false;
            if(inodo.indirectosdobles ==-1){
                cout<<"Indirectos -1"<<endl;
                blocksindex = getfreeblocks(1);
                for(int i =0; i<x;i++){
                    ID[i]=-1;
                    IS[i]=-1;
                }
                inodo.indirectosdobles= blocksindex[0];
                ID_pos = blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());
            }else{
                ID_pos = inodo.indirectosdobles*sb.sizeofblock;
                char * buftem = new char[x*sizeof(double)];
                read(buftem,ID_pos,sb.sizeofblock);
                memcpy((&ID),buftem,x*sizeof(double));
                cout<<"entro"<<endl;
                difID = (ocupados)- (floor(ocupados));
                array_pos = floor(ocupados/x);
                if(ID[array_pos]==-1){
                    for(int i =0; i<x;i++){
                        IS[i]=-1;
                    }
                }else{
                    IS_pos=ID[array_pos]*sb.sizeofblock;
                    read(buftem,IS_pos,sb.sizeofblock);
                    memcpy((&IS),buftem,x*sizeof(double));
                }
                if(ocupados>=x){

                    array_ISpos = floor(ocupados-x);
                }else{
                    array_ISpos = floor(ocupados);
                }

                cout<<"ocupados "<<ocupados<<endl;
                cout<<"DIFID "<<difID<<endl;
                cout<<"Array ISPOS "<<array_ISpos<<endl;
            }
            cout<<"Array pos "<<array_pos<<endl;
            for(int i =0; i< x;i++){
                cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                for(int i =0; i<x;i++){
                    cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                }
            }
            if(nuevo){
                cout<<"entro nuevo "<<endl;
                blocksindex = getfreeblocks(1);
                ID[array_pos]=blocksindex[0];
                IS_pos = blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());
                memcpybuffer(buf,bloque,size_file,size-size_file,sb.sizeofblock);
                //blocksindex = getfreeblocks(1);
                int pos = getFreePosInArray(IS,x);
                cout<<"POS EN NUEVO "<<pos<<endl;
                blocksindex = getfreeblocks(1);
                IS[pos] = blocksindex[0];
                cout<<IS[pos]<<endl;
                move_to=blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());
                write(buf,move_to,size_file);
                size_file-=size_file;
                nuevo = false;
            }else{
                if(ocupados < pow(x,2)){
                    double dif = ocupados - floor(ocupados);
                    cout<<"ocupados AQUI"<<ocupados<<endl;
                    cout<<"DIF "<<dif<<endl;
                    if(dif ==0 ){
                        if(ID[array_pos]==-1){
                            blocksindex=getfreeblocks(1);
                            ID[array_pos]=blocksindex[0];
                            blocksindex.erase(blocksindex.begin());
                        }else{
                            read(buf,ID[array_pos]*sb.sizeofblock,sb.sizeofblock);
                            memcpy((&IS),buf,x*sizeof(double));
                            array_ISpos = getFreePosInArray(IS,x);
                        }
                        //borrrar aqui
                        for(int i =0; i< x;i++){
                            cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                        }
                        cout<<"ARRAY POS "<<array_pos<<endl;
                        IS_pos=ID[array_pos]*sb.sizeofblock;
                        blocksindex = getfreeblocks(1);
//                        if(ocupados>x){
//                            ocupados-=x;
//                        }
                        //
                        int pos = getFreePosInArray(IS,x);
                        cout<<"POS EN NUEVO "<<pos<<endl;
                        IS[array_ISpos] = blocksindex[0];
                        cout<<IS[pos]<<endl;
                        move_to=blocksindex[0]*sb.sizeofblock;
                        cout<<"Move to "<<move_to<<endl;
                        blocksindex.erase(blocksindex.begin());
                        write(bloque,move_to,size);
                        size_file-=size;
                        //borrar aqui
                        cout<<"IS "<<endl;
                        cout<<IS[0]<<endl;
                        cout<<IS[1]<<endl;
                    }else{
                        cout<<"Entro dif !=0"<<endl;
                        free_space = sb.sizeofblock-(sb.sizeofblock*difID);
                        cout<<"FREE Space "<<free_space<<endl;
                        if(free_space >= size){
                            cout<<"Entro space >size "<<bloque<<endl;
                            cout<<(int)array_ISpos<<" QUE HAY DENTRO IS"<<IS[(int)array_ISpos]<<endl;
                            move_to = IS[(int)array_ISpos]*sb.sizeofblock + difID*sb.sizeofblock;
                            cout<<"Move to "<<move_to<<endl;
                            cout<<"Bloque "<<bloque<<endl;
                            write(bloque,move_to,size);
                            IS_pos = ID[array_pos]*sb.sizeofblock;
                            size_file-=size;
                        }else{
                            memcpybuffer(buf,bloque,free_space,0,sb.sizeofblock);
                            cout<<"Entro space <size "<<buf<<endl;
                            cout<<array_ISpos<<" IS QUE HAY DENTRO "<<IS[(int)array_ISpos]<<endl;
                            move_to = IS[(int)array_ISpos]*sb.sizeofblock + dif*sb.sizeofblock;
                            cout<<"move to "<<move_to<<endl;
                            write(buf,move_to,free_space);
                            size_file-=free_space;
                            double o = inodo.filesize/sb.sizeofblock;
                            IS_pos = ID[array_pos]*sb.sizeofblock;
                            write((char*)IS,IS_pos,sb.sizeofblock);
                            if(((free_space+size_file)/sb.sizeofblock)+o<10 + x +pow(x,2)){
                                int nue =getFreePosInArray(IS,x);
                                if(nue<0){
                                    if(array_pos<x){
                                        array_pos++;
                                        blocksindex = getfreeblocks(1);
                                        ID[array_pos]= blocksindex[0];
                                        IS_pos = blocksindex[0]*sb.sizeofblock;
                                        blocksindex.erase(blocksindex.begin());
                                        nue=0;
                                    }
                                }
                                memcpybuffer(buf,bloque,sb.sizeofblock,free_space,sb.sizeofblock);
                                cout<<"Entro space <10 + x + x2 "<<buf<<endl;
                                blocksindex = getfreeblocks(1);
                                IS[nue]= blocksindex[0];
                                move_to = blocksindex[0]*sb.sizeofblock;
                                blocksindex.erase(blocksindex.begin());
                                write(buf,move_to,size-free_space);
                                size_file-=(size-free_space);
                            }else{
                                nuevo = true;
                                cout<<"LLENOS INDIRECTOS dobles"<<endl;
                            }
                        }
                    }

                }else{
                    cout<<"Llenos Indirectos dobles"<<endl;
                }
            }
            write((char*)IS,IS_pos,sb.sizeofblock);
            write((char*)ID,ID_pos,sb.sizeofblock);
        }//aqui comienza indirectos triples
        if(size_file>0){
            cout<<"ENtro IT"<<endl;
            blocksindex.clear();
            cout<<"X dentro ID "<<x<<endl;
            double ID[x];
            double ID_pos=0;
            double IT[x];
            double IT_pos=0;
            double IS[x];
            double IS_pos=0;
            int array_pos=0;
            int array_ISpos=0;
            int array_IDpos=0;
            double difID=0;
            ocupados -= (pow(x,2));
            char *buf = new char[x*sizeof(double)];
            bool t=false;
            if(inodo.indirectostriples ==-1){
                cout<<"Indirectos -1"<<endl;
                blocksindex = getfreeblocks(1);
                for(int i =0; i<x;i++){
                    ID[i]=-1;
                    IS[i]=-1;
                    IT[i]=-1;
                }
                inodo.indirectostriples= blocksindex[0];
                IT_pos = blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());
            }else{//Si esta usado...
                IT_pos = inodo.indirectostriples*sb.sizeofblock;
                read(buf,IT_pos,sb.sizeofblock);
                memcpy((&IT),buf,x*sizeof(double));

                cout<<"entro"<<endl;
                difID = (ocupados)- (floor(ocupados));

                array_pos = floor(ocupados/pow(x,2));
                if(IT[array_pos]==-1){
                    for(int i =0; i<x;i++){
                        IS[i]=-1;
                        ID[i]=-1;
                    }
                }else{

                    ID_pos = IT[array_pos]*sb.sizeofblock;
                    if(ID_pos>0){
                    cout<<"ID_POS "<<ID_pos<<endl;
                    read(buf,ID_pos,sb.sizeofblock);
                    memcpy((&ID),buf,x*sizeof(double));
                    }else{
                        for(int i =0;i<x;i++){
                            ID[i]=-1;
                        }
                    }
                    if(ocupados>pow(x,2)){
                        array_IDpos =floor(ocupados/x)-x;
                    }else{
                        array_IDpos = floor(ocupados/x);
                    }
                    //
                    IS_pos=ID[array_IDpos]*sb.sizeofblock;
                    if(IS_pos>0){
                        cout<<"IS_pos"<<IS_pos<<endl;
                        read(buf,IS_pos,sb.sizeofblock);
                        memcpy((&IS),buf,x*sizeof(double));
                    }else{
                        for(int i =0;i<x;i++){
                            IS[i]=-1;
                        }
                    }
                }
                if(ocupados>=pow(x,2)){

                    array_ISpos = floor(ocupados-pow(x,2));
                }else if(ocupados >= x){
                    array_ISpos = floor(ocupados-x);
                }else{
                    array_ISpos = floor(ocupados);
                }


                //nuevo aqui
//                ID_pos = *sb.sizeofblock;
//                char * buftem = new char[x*sizeof(double)];
//                read(buftem,ID_pos,sb.sizeofblock);
//                memcpy((&ID),buftem,x*sizeof(double));

                cout<<"ocupados "<<ocupados<<endl;
                cout<<"DIFID "<<difID<<endl;
                cout<<"Array ISPOS "<<array_ISpos<<endl;
                cout<<"Array IDPOS "<<array_IDpos<<endl;
                cout<<"Array POS "<<array_pos<<endl;
            }
            cout<<"Array pos "<<array_pos<<endl;
            for(int i =0; i< x;i++){
                cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                for(int i =0; i<x;i++){
                    cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                }
            }
            if(nuevo){
                cout<<"entro nuevo "<<endl;
                blocksindex = getfreeblocks(1);
                IT[array_pos]=blocksindex[0];
                ID_pos = blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());

                blocksindex = getfreeblocks(1);
                ID[array_IDpos]=blocksindex[0];
                IS_pos = blocksindex[0]*sb.sizeofblock;
                blocksindex.erase(blocksindex.begin());

                blocksindex = getfreeblocks(1);
                IS[array_ISpos]= blocksindex[0];
                cout<<IS[array_ISpos]<<endl;
                move_to=blocksindex[0]*sb.sizeofblock;
                cout<<"Move to IT NUEVO "<<move_to<<endl;
                blocksindex.erase(blocksindex.begin());
                memcpybuffer(buf,bloque,size_file,size-size_file,sb.sizeofblock);
                write(buf,move_to,size_file);
                size_file-=size_file;
                nuevo = false;
                //blocksindex = getfreeblocks(1);
                //int pos = getFreePosInArray(IS,x);
            }else{
                if(ocupados < pow(x,3)){
                    double dif = ocupados - floor(ocupados);
                    cout<<"ocupados AQUI"<<ocupados<<endl;
                    cout<<"DIF "<<dif<<endl;
                    if(dif ==0 ){
                        if(IT[array_pos]==-1){
                            blocksindex=getfreeblocks(1);
                            IT[array_pos]=blocksindex[0];
                            blocksindex.erase(blocksindex.begin());
                        }
                        if(ID[array_IDpos]==-1){
                            blocksindex=getfreeblocks(1);
                            ID[array_IDpos]=blocksindex[0];
                            blocksindex.erase(blocksindex.begin());
                        }
                        /*else{
                            read(buf,IT[array_pos]*sb.sizeofblock,sb.sizeofblock);
                            memcpy((&ID),buf,x*sizeof(double));

                            array_IDpos = floor(ocupados/x);
                            cout<<"Array IDpos "<<array_IDpos<<endl;
                            read(buf,ID[array_IDpos]*sb.sizeofblock,sb.sizeofblock);
                            memcpy((&IS),buf,x*sizeof(double));
                            array_ISpos = getFreePosInArray(IS,x);
                        }*/
                        //borrrar aqui
                        for(int i =0; i< x;i++){
                            cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                            for(int i =0; i<x;i++){
                                cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                            }
                        }
                        cout<<"ARRAY POS "<<array_pos<<endl;
                        ID_pos=IT[array_pos]*sb.sizeofblock;
                        cout<<"ID_POS adentro DIf==0 "<<ID_pos<<endl;
                        IS_pos=ID[array_IDpos]*sb.sizeofblock;
                        cout<<"IS_POS adentro DIf==0 "<<IS_pos<<endl;
                        blocksindex = getfreeblocks(1);
//                        if(ocupados>x){
//                            ocupados-=x;
//                        }
                        //
                        int pos = getFreePosInArray(IS,x);
                        IS[pos] = blocksindex[0];
                        cout<<"POS EN NUEVO "<<pos<<" AIS "<<array_ISpos<<endl;
                        cout<<IS[array_ISpos]<<endl;
                        move_to=blocksindex[0]*sb.sizeofblock;
                        cout<<"Move to "<<move_to<<endl;
                        blocksindex.erase(blocksindex.begin());
                        write(bloque,move_to,size);
                        size_file-=size;
                        for(int i =0; i< x;i++){
                            cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                            for(int i =0; i<x;i++){
                                cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                            }
                        }
                    }else{
                        cout<<"Entro dif !=0"<<endl;
                        free_space = sb.sizeofblock-(sb.sizeofblock*difID);
                        cout<<"FREE Space "<<free_space<<endl;
                        if(free_space >= size){
                            cout<<"Entro space >size "<<bloque<<endl;
                            cout<<(int)array_ISpos<<" QUE HAY DENTRO IS"<<IS[(int)array_ISpos]<<endl;
                            move_to = IS[(int)array_ISpos]*sb.sizeofblock + difID*sb.sizeofblock;
                            cout<<"Move to "<<move_to<<endl;
                            cout<<"Bloque "<<bloque<<endl;
                            write(bloque,move_to,size);
                            //IS_pos = ID[array_IDpos]*sb.sizeofblock;
                            size_file-=size;
                            for(int i =0; i< x;i++){
                                cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                                for(int i =0; i<x;i++){
                                    cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                                }
                            }
                        }else{
                            memcpybuffer(buf,bloque,free_space,0,sb.sizeofblock);
                            cout<<"Entro space <size "<<buf<<endl;
                            cout<<array_ISpos<<" IS QUE HAY DENTRO "<<IS[(int)array_ISpos]<<endl;
                            move_to = IS[(int)array_ISpos]*sb.sizeofblock + dif*sb.sizeofblock;
                            cout<<"move to "<<move_to<<endl;
                            write(buf,move_to,free_space);
                            size_file-=free_space;
                            double o = inodo.filesize/sb.sizeofblock;
                            //IS_pos = ID[array_IDpos]*sb.sizeofblock;
                            write((char*)IS,IS_pos,sb.sizeofblock);
                            cout<<"POS IS AFUERA "<<IS_pos<<endl;
                            if(((free_space+size_file)/sb.sizeofblock)+o<10 + x +pow(x,2)+ pow(x,3)){
                                int nue =getFreePosInArray(IS,x);
                                cout<<"entro nue < 0"<<endl;
                                if(nue<0){
                                    cout<<"entro nue < 0"<<endl;
                                    int t = getFreePosInArray(ID,x);
                                    if(t<0){
                                        cout<<"Entro t <0 "<<endl;
                                        if(array_pos<x){
                                            array_pos++;
                                            blocksindex = getfreeblocks(1);
                                            IT[array_pos]= blocksindex[0];
                                            ID_pos = blocksindex[0]*sb.sizeofblock;
                                            blocksindex.erase(blocksindex.begin());
                                            for(int i =0; i<x;i++){
                                                ID[i]=-1;
                                            }
                                            blocksindex = getfreeblocks(1);
                                            ID[0]=blocksindex[0];
                                            IS_pos=blocksindex[0]*sb.sizeofblock;
                                            blocksindex.erase(blocksindex.begin());
                                            nue=0;
                                        }
                                    }else{
                                        for(int i =0; i<x;i++){
                                            IS[i]=-1;
                                        }
                                        array_IDpos=t;
                                        cout<<"Array IDPOS "<<array_IDpos<<endl;
                                        blocksindex=getfreeblocks(1);
                                        ID[array_IDpos]=blocksindex[0];
                                        blocksindex.erase(blocksindex.begin());
                                        IS_pos=ID[array_IDpos]*sb.sizeofblock;
                                        cout<<ID[array_IDpos]<<endl;
                                        nue=0;
                                    }
                                }
                                for(int i =0; i< x;i++){
                                    cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                                    for(int i =0; i<x;i++){
                                        cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                                    }
                                }
                                memcpybuffer(buf,bloque,sb.sizeofblock,free_space,sb.sizeofblock);
                                cout<<"Entro space <10 + x + x2 +x3 "<<buf<<endl;
                                blocksindex = getfreeblocks(1);
                                cout<<"NUE abajo "<<nue<<endl;
                                IS[nue]= blocksindex[0];
                                move_to = blocksindex[0]*sb.sizeofblock;
                                cout<<"Move to "<<move_to<<endl;
                                blocksindex.erase(blocksindex.begin());
                                write(buf,move_to,size-free_space);
                                size_file-=(size-free_space);
                                for(int i =0; i< x;i++){
                                    cout<<"ID pos "<<i<<" "<<ID[i]<<endl;
                                    for(int i =0; i<x;i++){
                                        cout<<"IS pos "<<i<<" "<<IS[i]<<endl;
                                    }
                                }
                            }else{
                                nuevo = true;
                                cout<<"LLENOS INDIRECTOS Triples"<<endl;
                            }
                        }
                    }

                }else{
                    cout<<"Llenos Indirectos Triples"<<endl;
                }
            }
            write((char*)IS,IS_pos,sb.sizeofblock);
            cout<<"POS IS AFUERA "<<IS_pos<<endl;
            write((char*)ID,ID_pos,sb.sizeofblock);
            cout<<"POS ID AFUERA "<<ID_pos<<endl;
            write((char*)IT,IT_pos,sb.sizeofblock);
            cout<<"POS IT AFUERA "<<IT_pos<<endl;
        }

    }//me quede aqui
    cout<<"Size_file "<<size_file<<endl;
    cout<<"file size ANTES "<<inodo.filesize<<endl;
    inodo.blockuse+=size/sb.sizeofblock;
    inodo.filesize+=size;
    cout<<"Blockused "<<inodo.blockuse<<" y filesize "<<inodo.filesize<<endl;
    return true;

}

bool Disco::write(char *buffer, double init, double byte_size)
{
    ofstream out(path.c_str(), ios::in | ios::out | ios::binary);
    if(!out){
        return false;
    }else{
        out.seekp(init, ios::beg);
        out.write(buffer,byte_size);
        out.close();
    }
    return true;
}

bool Disco::nameExist(string nombre)
{
    for(int i =0; i < sb.cantofinode; i++){
        if(ft_array[i].name == nombre){
            return true;
        }
    }
    return false;
}

bool Disco::crearBloqueFT()
{
    vector<double>bloque;
    bool found = false;
    for(int i =0; i<10 ;i++){
        if(global.directos[i]==-1){
            bloque = getfreeblocks(1);
            global.directos[i]=bloque[0];
            setFiletables(bloque[0],path);
            found=true;
            return true;
        }
    }
    if(!found){
        if(global.indirectossimples==-1){
            bloque = getfreeblocks(2);
            global.indirectossimples =bloque[0];
            int x = sb.sizeofblock/8;
            double IS[x];
            double IS_pos = bloque[0]*sb.sizeofblock;
            bloque.erase(bloque.begin());
            for(int i =0; i< x;i++){
                setFiletables(bloque[0],path);
            }
        }
    }

}

bool Disco::mkDir(string nombre)
{
    if(sb.freeinode>0){

    }else{
        cout<<"No hay inodos disponibles "<<endl;
    }
}

void Disco::cd(string nombre)
{
    double num = seek(nombre);
    if(num <0 ){
        cout<<"No existe registro de nombre "<<nombre<<endl;
    }else{
        global =seekInode(num,path);
    }
}

inode_d Disco::seekInode(double num, string path)
{
    ifstream in(path.c_str(), ios::in | ios::out | ios:: binary);
    double move_to = sizeof(superBlock_d)+(bitmap_size*sizeof(char))+(bit_inode_size*sizeof(char))+(sizeof(FileTable_d)*sb.cantofinode)
            +(num*sizeof(inode_d));
    in.seekg(move_to,ios::beg);
    char *buffer = new char[sizeof(inode_d)];
    in.read(buffer, sizeof(inode_d));
    inode_d a;
    memcpy((&a),buffer,sizeof(inode_d));
    return a;
}

bool Disco::mkFile(double size_file, string file_name)
{
    vector<double>needed = fileVerification(size_file);
    if(needed[2]==-1){
        cout<<"Archivo muy grande para la capacidad del inodo"<<endl;
        return false;
    }else{
        if(size_file>sb.freespace){
            cout<<"No hay suficiente espacio en el disco para guardar archivo"<<endl;
            return false;
        }else{
            if(sb.freeinode<=0){
                cout<<"No hay inodos dispobibles para ese archivo"<<endl;
                return false;
            }else{
                if(nameExist(file_name)){
                    cout<<"Ese nombre ya existe, re-escriba el nombre"<<endl;
                    return false;
                }else{
                    //getting inodes and filetable
                    char *buffer = new char[sizeof(inode_d)];
                    int inode_pos = nextAvailable(bitmap_inode,false);

                    if(inode_pos!=-1){
                        cout<<"pos del inodo "<<inode_pos<<endl;
                        cout<<"esta usado inode pos "<<is_block_in_use(bitmap_inode,inode_pos);
                        double move_to = sizeof(superBlock_d)+(bitmap_size*sizeof(char))+(bit_inode_size*sizeof(char))+(sizeof(FileTable_d)*sb.cantofinode)
                                +(inode_pos*sizeof(inode_d));
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
                        //borrar comen
                        double filetable_pos =getNextFreeFileTable();
                        cout<<"pos del FT "<<filetable_pos<<endl;
                        cout<<" esta usado "<<is_block_in_use(bitmap_inode,filetable_pos)<<endl;
                        strcpy(ft_array[(int)filetable_pos].name,file_name.c_str());
                        ft_array[(int)filetable_pos].inode_index=inode_pos;
                        char * bloque = new char[sb.sizeofblock];

                        for(int i =0; i<sb.sizeofblock;i++){
                            int x = rand()%25+65;
                            bloque[i]= x;
                       }
                       bloque[0]='A';
                       bloque[sb.sizeofblock-1]='Z';
                       //strcpy(bloque,"1234567890123456");
                       sb.freespace-=size_file+(needed[1]*sb.sizeofblock);
                       for(int i =0; i < needed[0];i++){
                           if(size_file<=0)
                               break;
                           if(size_file>=sb.sizeofblock){
                                writeBloque(bloque,inodo,sb.sizeofblock,"-rwxrwxrwx");
                           }else{
                               writeBloque(bloque,inodo,size_file,"-rwxrwxrwx");
                           }
                           size_file-=sb.sizeofblock;
                       }
                       ofstream out(path.c_str(), ios::in | ios::out | ios::binary);
                       //actualizando super block
                       sb.freeblock-=(needed[0]+needed[1]);
                       sb.freeinode--;
                       out.write((char*)&sb,sizeof(superBlock_d));

                       //actualizando bitmap
                       out.write(bitmap,bitmap_size*sizeof(char));
                       //actualizando bitmap de inodes
                       out.write(bitmap_inode,bit_inode_size*sizeof(char));
                       //actualizando filetable
                       FileTable_d ft = ft_array[(int)filetable_pos];
                       double ftb_pos = filetable_pos*sizeof(FileTable_d);
                       out.seekp(ftb_pos,ios::cur);
                       out.write((char*)&ft,sizeof(FileTable_d));
                       //actualizando inodo
                       out.seekp(move_to,ios::beg);
                       out.write(((char*)&inodo),sizeof(inode_d));
                       out.close();

                       cout<<"Imprime directos simples afuera de writeDS"<<endl;
                       for(int i =0; i<10;i++){
                           cout<<inodo.directos[i]<<" "<<ends;
                       }
                       cout<<endl;
                       cout<<"Indirectos simples"<<endl;
                       if(inodo.indirectossimples>0){
                       double move = inodo.indirectossimples*sb.sizeofblock;
                       char * tem = new char[sb.sizeofblock];
                       double t[2];
                       if(move>0){
                           read(tem,move,sb.sizeofblock);
                           memcpy((&t),tem,sb.sizeofblock);
                       }

                       for(int i =0; i<2;i++){
                           cout<<t[i]<<" "<<ends;
                       }
                       cout<<endl;
                       }
                       if(inodo.indirectosdobles>0){
                       cout<<"Indirectos dobles"<<endl;
                       double move = inodo.indirectosdobles*sb.sizeofblock;
                       char *tem = new char[sb.sizeofblock];
                       double t[2];
                       if(move>0){
                           read(tem,move,sb.sizeofblock);
                           memcpy((&t),tem,sb.sizeofblock);
                       }

                       for(int i =0; i<2;i++){
                           cout<<"ID"<<endl;
                           cout<<t[i]<<" "<<endl;
                           double t2[2];
                           if(t[i]!=-1){
                               move=t[i]*sb.sizeofblock;
                               read(tem,move,sb.sizeofblock);
                               memcpy((&t2),tem,sb.sizeofblock);
                               cout<<"IS"<<endl;
                               for(int i =0;i<2;i++){
                                   cout<<t2[i]<<" "<<ends;
                               }
                               cout<<endl;
                           }
                       }
                       cout<<endl;
                       }
                       char * buftemp = new char[sb.sizeofblock];
                       read(buftemp,4387*16,sb.sizeofblock);
                       double s[2];
                       memcpy((&s),buftemp,sb.sizeofblock);
                       cout<<"Afuera"<<endl;
                       cout<<s[0]<<endl;
                       cout<<s[1]<<endl;
                       double y[2];
                       double m[2];
                       double n[2];
                       cout<<"Indirectos triples"<<endl;
                       cout<<inodo.indirectostriples<<endl;
//                       if(inodo.indirectostriples>0){
//                           cout<<"IT"<<endl;
//                           read(buftemp,inodo.indirectostriples*sb.sizeofblock,sb.sizeofblock);
//                           memcpy(&y,buftemp,sb.sizeofblock);
//                           cout<<y[0]<<" "<<y[1]<<ends;
//                           cout<<endl;

//                           read(buftemp,y[0]*sb.sizeofblock,sb.sizeofblock);
//                           memcpy(&m,buftemp,sb.sizeofblock);
//                           for(int i =0; i<2;i++){
//                               cout<<"ID"<<endl;

//                               cout<<m[i]<<endl;
//                               cout<<"IS"<<endl;
//                               if(m[i]!=-1){
//                                   read(buftemp,m[i]*sb.sizeofblock,sb.sizeofblock);
//                                   memcpy(&n,buftemp,sb.sizeofblock);
//                                   for(int i =0;i<2;i++){
//                                       cout<<n[i]<<ends;
//                                   }
//                                   cout<<endl;
//                               }
//                           }
//                           cout<<endl;
//                       }
                       int x=sb.sizeofblock/8;
                       char *buf = new char[sb.sizeofblock];
                       double z[x];
                       double v[x];
                       double c[x];
                       read(buf,(inodo.indirectostriples*sb.sizeofblock),(double)sb.sizeofblock);
                       memcpy(&z,buf,sb.sizeofblock);
                           //    memcpy(&ino,buf,size_block);
                               cout<<"Leido del Disco IT en!: "<<inodo.indirectostriples<<endl;
                               if(inodo.indirectostriples!=-1)
                               {
                                   for (int i = 0; i < x; ++i) {
                                       cout<<"ID en- "<<z[i]<<endl;
                                       char *buf2 = new char[sb.sizeofblock];
                                       read(buf2,z[i]*sb.sizeofblock,(double)sb.sizeofblock);
                                       memcpy(&v,buf2,sb.sizeofblock);
                               //        cout<<"ID del IT"<<endl;
                                       if(z[i]!=-1)
                                       {
                                           for (int j = 0; j < x; ++j) {
                                               cout<<"IS en- "<<v[j]<<endl;
                                               char *buf3 = new char[sb.sizeofblock];
                                               read(buf3,v[j]*sb.sizeofblock,(double)sb.sizeofblock);
                                               memcpy(&c,buf3,sb.sizeofblock);
                                   //            cout<<"IS del ID del IT"<<endl;
                                               if(v[j]!=-1)
                                               {
                                                   for (int k = 0; k < x; ++k) {
                                                       cout<<"data en- "<<c[k]<<endl;
                                                   }
                                               }
                                           }
                                       }
                                   }
                               }
                       return true;
                    }

                }
            }
        }
    }
}

void Disco::setFiletables(double numBlock, string path)
{
    ofstream out(path.c_str(), ios::in| ios::out | ios::binary);
    double pos = numBlock*sb.sizeofblock;
    out.seekp(pos,ios::beg);
    FileTable_d ft;
    memset(ft.name,0,20);
    ft.inode_index=-1;
    double cant = sb.sizeofblock/sizeof(FileTable_d);
    for(int i =0; i< floor(cant); i++){
        out.write((char*)&ft,sizeof(FileTable_d));
    }
    out.close();
}

