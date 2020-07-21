#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QtGui>
#include <QtCore>

namespace Ui {
class Dialog;
}

class Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();        

private:
    Ui::Dialog *ui;
    const int POINT_SIZE  = 6;
    const int MARGIN_SIZE = 10;
    int QTY_COLUMNS = 10;
    int QTY_LINES   = 10;

protected:
    void paintEvent(QPaintEvent *e);

private slots:

    void on_spinBox_vert_valueChanged(int arg1);

    void on_spinBox_hor_valueChanged(int arg1);

private:
    QPen pen;
};

#endif // DIALOG_H
