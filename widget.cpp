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
    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem();
    QString t = commandPromt->logica();
    ui->plainTextEdit->appendPlainText(t);
    ui->plainTextEdit->setReadOnly(true);
}

void Widget::on_lineEdit_returnPressed()
{
    commandPromt->setActual(ui->lineEdit->text());
    insertar();
    ui->lineEdit->clear();
}
