#include "tree.h"
#include "ui_tree.h"

void Tree::createTree(inode_d &inodo, Disco * disk, QTreeWidgetItem* &raiz){
    cout<<"no paso"<<endl;
    QList<QTreeWidgetItem*>hijos;
    vector<FileTable_d*>array_temp;
    char * buffer;
    vector<double>bloques = disk->getUsedBloques(inodo);
    disk->getData(buffer,bloques,inodo.filesize);
    array_temp = disk->getFTfromDir(buffer,inodo.filesize);
    inode t;
    cout<<array_temp.size()<<" size array"<<endl;
    for(int i =0;i<array_temp.size();i++){
        cout<<array_temp[i]->name<<endl;
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0,array_temp[i]->name);
        t = disk->seekInode(array_temp[i]->inode_index,disk->path);
        if(t.permisos[0]=='d' && (strcmp(array_temp[i]->name,"..") != 0)){
            createTree(t,disk,item);
        }
        hijos.push_back(item);

    }
    raiz->addChildren(hijos);
}

Tree::Tree(QWidget *parent, Disco *disk) :
    QDialog(parent),
    ui(new Ui::Tree)
{
    ui->setupUi(this);
    setWindowTitle("Directory Tree");
    QStringList header;
    header<<"TREE";
    ui->treeWidget->setHeaderLabels(header);

    inode_d inodo = disk->seekInode(0,disk->path);


    //viejo
    QTreeWidgetItem *item = new QTreeWidgetItem ();
    item->setText(0,"ROOT");
    createTree(inodo,disk,item);
//    QList<QTreeWidgetItem*>hijos;

//    QTreeWidgetItem *item2 = new QTreeWidgetItem ();
//    item2->setText(0,"2");
//    QTreeWidgetItem *item3 = new QTreeWidgetItem ();
//    item3->setText(0,"3");
//    QTreeWidgetItem *item4 = new QTreeWidgetItem ();
//    item4->setText(0,"4");
//    hijos.push_back(item2);
//    hijos.push_back(item3);
//    hijos.push_back(item4);

//    if(hijos.contains(item2)){
//        QTreeWidgetItem *i = new QTreeWidgetItem ();
//        i = hijos[0];
//        QList<QTreeWidgetItem*>hijo;
//        QTreeWidgetItem *it2 = new QTreeWidgetItem ();
//        it2->setText(0,"5");
//        QTreeWidgetItem *it3 = new QTreeWidgetItem ();
//        it3->setText(0,"6");
//        hijo.push_front(it2);
//        hijo.push_front(it3);
//        i->addChildren(hijo);
//    }

//    item->addChildren(hijos);

    ui->treeWidget->addTopLevelItem(item);
}

Tree::~Tree()
{
    delete ui;
}
