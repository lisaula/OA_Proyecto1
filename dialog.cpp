#include "dialog.h"
#include "ui_dialog.h"

void Dialog::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void Dialog::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void Dialog::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void Dialog::dropEvent(QDropEvent *event)
{
    QList<QUrl>urls;
    QString nombre;
    QList<QUrl>::Iterator i;
    urls =event->mimeData()->urls();
    for(i = urls.begin();i!=urls.end();++i){
        nombre =i->fileName();
        ui->listWidget->addItem(nombre);
        paths.push_back(i->path());
        name.push_back(nombre);
    }
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("ADD FILE");
    setAcceptDrops(true);
}

Dialog::~Dialog()
{
    delete ui;
}

vector<QString> Dialog::getUrls()
{
    return this->paths;
}

vector<QString> Dialog::getNames()
{
    return this->name;
}

void Dialog::on_buttonBox_accepted()
{
    accept();
}

void Dialog::on_buttonBox_rejected()
{
    reject();
}
