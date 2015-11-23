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
    ui->plainTextEdit->setReadOnly(true);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::insertar()
{
    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem();
    QString t = commandPromt->logica();
//    text->setText(t);
//    string s;
//    s=t.toStdString();
    //cout<<"Funcion insertar"<<endl;
    ui->plainTextEdit->appendPlainText(t);
    //cout<<"Salio funcion imprime "<<t.toStdString()<<endl;

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
    if(commandPromt->getMount()){
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
        //ui->plainTextEdit->setReadOnly(true);
    }else{
        QMessageBox::warning(this,"ERROR","No hay disco montado");
    }
}

void Widget::on_show_block_clicked()
{
    if(commandPromt->getMount()){
        double blocks = commandPromt->getBlocks();
        double FS_used_blocks = commandPromt->getFS_used_block();
        Table * t = new Table(this,blocks,FS_used_blocks);
        Disco *disk = commandPromt->getDisk();
        for(int i =0;i<disk->sb.cantofinode;i++){
            //cout<<disk->sb.cantofinode<<" cant inodo"<<endl;
            if(disk->is_block_in_use(disk->bitmap_inode,i)){
                //cout<<"entro en "<<i<<endl;
               inode_d inodo = disk->seekInode(i,disk->path);
               vector<double>b = disk->getAllUsedBlocks(inodo);
               t->set_color(b);
            }
        }
        t->exec();
    }else{
        QMessageBox::warning(this,"ERROR","No hay disco montado");
    }
}

void Widget::on_show_trees_clicked()
{
    if(commandPromt->getMount()){
        Tree * t = new Tree(this,commandPromt->getDisk());
        t->exec();
    }else{
        QMessageBox::warning(this,"ERROR","No hay disco montado");
    }
}
