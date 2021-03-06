#ifndef WIDGET_H
#define WIDGET_H
#include"cp.h"
#include <QWidget>
#include<QGraphicsScene>
#include<QGraphicsSimpleTextItem>
#include<QListWidgetItem>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    void insertar();
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_lineEdit_returnPressed();

private:
    QGraphicsScene *scene;
    QGraphicsSimpleTextItem *text;
    CP * commandPromt = new CP();
    Ui::Widget *ui;
};

#endif // WIDGET_H
