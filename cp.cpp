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
    delimitador=" ";
    disk= new Disco();
}

QString CP::parseLogic(vector<string> parse)
{
    QString text ="";
    if(parse[0]== "fdisk"){
        if(parse[1]=="D" && parse.size()==2){
            text="nombre, tamanoblocks, freeblocks, inodes, freeinodes, espacio, espacio libre";
        }else if (parse[1]== "L" && parse.size()==2){
            text="Muestra todos los discos virtuales creados";
        }else if(parse[1]== "n"){
            if(parse.size()!=5){
                text="error de comando";
            }else{
                string nombre = parse[2], tamano=parse[3], tamanoB= parse[4];
                if(nombre.size()<=10){
                int t = std::atoi(tamano.c_str());
                int tB = std::atoi(tamanoB.c_str());
                bool p = disk->crearDisco(nombre,t, tB);
                if(p){
                    text=QString("HA creado un disco llamado %1 con tamano %2Mb con tamano Bloque %3B").arg(nombre.c_str()).arg(tamano.c_str()).arg(tamanoB.c_str());
                }else{
                    text="Error al crear disco.";
                }
                }else
                    text="El nombre solo permite 10 caracteres.";
            }
        }else if(parse[1] =="d"){
            if(parse.size()!=3){
                text="error de comando";
            }else{
                string nombre = parse[2];
                if(nombre.size()<=10)
                    text=QString("Eliminaste el disco %1").arg(nombre.c_str());
                else
                    text="El nombre solo permite 10 caracteres.";
            }
        }else{
            text="error de comando";
        }
    }else if (parse[0]=="mount"){
        if(parse.size()!=2){
            text="error de comando";
        }else{
            string nombre =parse[1];
            if(nombre.size()<=10)
                text=QString("montaste disco %1").arg(nombre.c_str());
            else
                text="El nombre solo permite 10 caracteres.";
        }
    }else{
        text="error de comando";
    }
    return text;
    parse.clear();
}

