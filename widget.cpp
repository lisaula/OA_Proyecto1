#include "widget.h"
#include "ui_widget.h"



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("FILE SYSTEM");
    scene = new QGraphicsScene();
    text = new QGraphicsSimpleTextItem();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::insertar()
{
    //QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem();
    QString t = commandPromt->logica();
    ui->plainTextEdit->appendPlainText(t);
    ui->plainTextEdit->setReadOnly(true);
}

void Widget::on_lineEdit_returnPressed()
{
    if(ui->lineEdit->text()==""){
        ui->lineEdit->setText(last);
        //commandPromt->setActual(last);
    }else{
    commandPromt->setActual(ui->lineEdit->text());
    last = ui->lineEdit->text();
    insertar();
    ui->lineEdit->clear();
    }
}

void Widget::on_add_file_clicked()
{
    int res;
    Dialog * d = new Dialog(this);
    res=d->exec();
    if(res == QDialog::Rejected){
        return;
    }
    vector<QString>urls;
    vector<QString>name;
    urls=d->getUrls();
    name=d->getNames();

    QString t = commandPromt->AddFile(urls[0].toStdString(),name[0].toStdString());
    ui->plainTextEdit->appendPlainText(t);
    ui->plainTextEdit->setReadOnly(true);
}
