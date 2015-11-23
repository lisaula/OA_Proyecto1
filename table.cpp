#include "table.h"
#include "ui_table.h"

Table::Table(QWidget *parent, double cant, double fs_used):
    QDialog(parent),
    ui(new Ui::Table)
{
    ui->setupUi(this);
    setWindowTitle("Blocks");
    srand(time(NULL));
    //QString t = "";
    double columna=0;
    ui->tableWidget->setColumnCount(10);
    for(int i =0; i< cant;i++){
        if(i%10==0){
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            columna=0;
        }
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(i));
        if(i<fs_used){
            QBrush brush(QColor(104,104,104));
            item->setBackground(brush);
        }else{
            QBrush brush(QColor(255,255,255));
            item->setBackground(brush);
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,columna,item);
        columna++;
    }
}

void Table::set_color(vector<double> blocks)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    QBrush brush = getNewColor();
    for(int i =0;i<blocks.size();i++){
        item = ui->tableWidget->findItems(QString::number(blocks[i]),Qt::MatchExactly).first();
        item->setBackground(brush);
    }
}

QBrush Table::getNewColor()
{
    int num;
    int num2;
    int num3;
    QString c;
    while(true){
        num = rand()%256;
        num2 = rand()%256;
        num3 = rand()%256;
        c = QString::number(num)+"-"+QString::number(num2)+"-"+QString::number(num3);
        //cout<<c.toStdString()<<endl;
        if(!usados.contains(c)){
            QBrush brush(QColor(num,num2,num3));
            usados.push_back(c);
            return brush;
        }
    }
}


Table::~Table()
{
    delete ui;
}

void Table::initTable()
{


}
