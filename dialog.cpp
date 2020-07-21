#include "dialog.h"
#include "ui_dialog.h"
#include "QDebug"
#include <math.h>

Dialog::Dialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    ui->spinBox_hor->setValue (QTY_COLUMNS);
    ui->spinBox_vert->setValue(QTY_LINES);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::paintEvent(QPaintEvent *e)
{        
    QPainter painter(this);
    QPen pointpen(Qt::black);
    pointpen.setWidth(POINT_SIZE);

    int interval_horizontal_size = floor((this->width()  - MARGIN_SIZE - QTY_COLUMNS * POINT_SIZE)/(QTY_COLUMNS - 1));
    int interval_vertical_size   = floor((this->height() - MARGIN_SIZE - QTY_LINES   * POINT_SIZE)/(QTY_LINES   - 1));

    painter.setPen(pointpen);

    for (int i = 0; i < QTY_LINES; ++i) {
        for (int j = 0; j < QTY_COLUMNS; ++j) {
            painter.drawPoint(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*j,
                              MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*i);
        }
    }
}

void Dialog::on_spinBox_vert_valueChanged(int qty_line)
{
    QTY_LINES = qty_line;
    update();
}

void Dialog::on_spinBox_hor_valueChanged(int qty_col)
{
    QTY_COLUMNS = qty_col;
    update();
}
