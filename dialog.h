#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QDragEnterEvent>
#include<QUrl>
#include<QMimeData>
#include<vector>
using namespace std;
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
protected:
    void dragEnterEvent(QDragEnterEvent * event);
    void dragLeaveEvent(QDragLeaveEvent * event);
    void dragMoveEvent(QDragMoveEvent * event);
    void dropEvent(QDropEvent * event);
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    vector<QString>paths;
    vector<QString>name;
    vector<QString> getUrls();
    vector<QString> getNames();

private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
