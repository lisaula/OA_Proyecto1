#include "widget.h"
#include <QApplication>
//#include<disco.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    //Disco * disco = new Disco();


    return a.exec();
}
