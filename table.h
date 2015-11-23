#ifndef TABLE_H
#define TABLE_H
#include<stdlib.h>
#include <QDialog>
#include<vector>
#include<iostream>

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
    Ui::Table *ui;
};

#endif // TABLE_H
