#ifndef TREE_H
#define TREE_H
#include<QTreeWidgetItem>
#include <QDialog>
#include<QMessageBox>
#include"disco.h"

namespace Ui {
class Tree;
}

class Tree : public QDialog
{
    Q_OBJECT

public:
    void createTree(inode_d &inodo, Disco * disk, QTreeWidgetItem *&raiz);
    explicit Tree(QWidget *parent, Disco * disk);
    ~Tree();

private:
    Ui::Tree *ui;
};

#endif // TREE_H
