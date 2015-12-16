#ifndef TABLE_H
#define TABLE_H
#include<stdlib.h>
#include <QDialog>
#include<vector>
#include<iostream>
#include<QTableWidgetItem>
#include<QList>

using namespace std;

namespace Ui {
class Table;
}

class Table : public QDialog
{
    Q_OBJECT

public:
    explicit Table(QWidget *parent, double cant, double fs_used);
    void set_color(vector<double>blocks);
    QBrush getNewColor();
    ~Table();
    void initTable();
private:
    QStringList usados;
    QList<QTableWidgetItem*> listItems;
    Ui::Table *ui;
};

#endif // TABLE_H
