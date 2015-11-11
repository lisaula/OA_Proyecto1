#include "cp.h"

QString CP::logica()
{
    parse.clear();
    size_t pos = 0;
    std::string token;
    while ((pos = actual.find(delimitador)) != std::string::npos) {
        token = actual.substr(0, pos);
        parse.push_back(token);
        actual.erase(0, pos + delimitador.length());
    }
    parse.push_back(actual);
    return parseLogic(parse);
}

void CP::setActual(QString le)
{
    actual = le.toStdString();
}

CP::CP()
{
    actual ="";
    current="";
    dir="/";
    m=false;
    delimitador=" ";
    disk= new Disco();
}

QString CP::parseLogic(vector<string> parse)
{
    QString text =QString("%1 ~ %2$-").arg(current.c_str()).arg(dir.c_str());
    for(int i =0; i<parse.size();i++){
        text += QString("%1 ").arg(parse[i].c_str());
    }
    text+="\n";
    if(parse[0]== "fdisk"){
        if(parse[1]=="D" && parse.size()==3){
            text+=disk->getSP(parse[2]);
        }else if (parse[1]== "L" && parse.size()==2){
            QDir dir("DISKS");
            QFileInfoList list = dir.entryInfoList();
            if(list.size()>2){
                QString t;
                QStringList l;
                for(int i =0; i<list.size();i++){
                    t=list.at(i).fileName();
                    if(t.contains(".dat")){
                        l=t.split(".dat");
                        text+=l.at(0)+"\n";
                    }
                    cout<<t.toStdString()<<endl;
                }
            }else
                text+="No hay discos virtuales creados";

        }else if(parse[1]== "n"){
            if(parse.size()!=5){
                text+="Error de comando";
            }else{
                string nombre = parse[2], tamano=parse[3], tamanoB= parse[4];
                if(nombre.size()<=10){
                double t = std::atoi(tamano.c_str());
                double tB = std::atoi(tamanoB.c_str());
                bool p = disk->crearDisco(nombre,t, tB);
                if(p){
//                    current=nombre;
//                    m=true;
                    text+=QString("Ha creado un disco llamado %1 con tamano %2Mb con tamano Bloque %3B").arg(nombre.c_str()).arg(tamano.c_str()).arg(tamanoB.c_str());
                }else{
                    text+="Error al crear disco.";
                }
                }else
                    text+="El nombre solo permite 10 caracteres.";
            }
        }else if(parse[1] =="d"){
            if(parse.size()!=3){
                text+="Error de comando";
            }else{
                string nombre = parse[2];
                if(nombre.size()<=10){
                    string path = DISK_PATH + nombre + ".dat";
                    if(remove(path.c_str()) ==0){
                        if(nombre==current){
                            current="";
                            dir="/";
                            m=false;
                        }
                        text+=QString("Eliminaste el disco %1").arg(nombre.c_str());
                    }
                }else
                    text+="El nombre solo permite 10 caracteres.";
            }
        }else{
            text+="Error de comando";
        }
    }else if (parse[0]=="mount"){
        if(parse.size()!=2){
            text+="Error de comando";
        }else{
            string nombre =parse[1];
            if(nombre.size()<=10){
                if(disk->mount(nombre)){
                    current = nombre;
                    dir="/root ";
                    m=true;
                text+=QString("montaste disco: %1").arg(nombre.c_str());
                }else{
                   text+=QString("ERROR AL MONTAR EL DISCO");
                }
            }else
                text+="El nombre solo permite 10 caracteres.";
        }
    }else if(m && parse[0]=="ls"){
        text+="ENLISTA DESPUES DE HABER MONTADO";

    }else if(m && parse[0]=="cd"){
        if(parse.size()==2){
            dir="/root/"+parse[1]+" ";
            text+=QString("Se movio en el disco a %1").arg(parse[1].c_str());
        }else{
            text+="Especifique path";
        }
    }else if(m && parse[0]=="mkdir" && parse.size()==2){
        text+=QString("Creo archivo %1").arg(parse[1].c_str());
    }else if(m && parse[0]=="mkfile" && parse.size()==3){
        string nombre = parse[1];
        if(nombre.size()>20){
            text+="No se admiten archivos con nombres mayor a 20 caracteres";
        }else{
            double tamano = std::atof(parse[2].c_str());
            if(disk->guardararchivo(current,tamano,nombre)){
            text+= QString("Ha creado un archivo llamado %1 con tamano %2MB en el directorio %3").arg(nombre.c_str()).arg(tamano).arg(dir.c_str());
            }
        }
    }else if(m && parse[0]=="rm" && parse.size()==2){
        string nombre = parse[1];
        if(nombre.size()>20){
            text+="No se admiten archivos con nombres mayor a 20 caracteres";
        }else{
            text+=QString("Ha borrado el archivo o directorio %1").arg(nombre.c_str());
        }
    }else if(m && parse[0]=="export" && parse.size()==2){
        string nombre = parse[1];
        if(nombre.size()>20){
            text+="No se admiten archivos con nombres mayor a 20 caracteres";
        }else{
            text+=QString("Ha exportado el archivo %1").arg(nombre.c_str());
        }
    }else if(m && parse[0]=="cp" && parse.size()==3){
        string nombre = parse[1], direccion =parse[2];
        if(nombre.size()>20 || direccion.size()>20){
            text+="El noombre o direccion es mayor a 20 caracteres";
        }else{
            text+=QString("Ha copiado el archivo %1 a carpeta %2").arg(nombre.c_str()).arg(direccion.c_str());
        }
    }else if(parse[0]=="exit"){
        exit(0);
    }else{
        text+="Comando no aceptado";
    }
    return text;
    parse.clear();
}

