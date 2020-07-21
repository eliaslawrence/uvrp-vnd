#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <dialog.h>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <vector>
#include "core.h"
#include "stop_point.h"
#include "matrix.h"
#include "pool_of_solutions.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

private:
    Ui::MainWindow * ui        ;
    Dialog         * renderArea;
    QGraphicsScene * scene     ;
    Core             core      ;
//    Pool_of_Solutions p_solutions;
    vector<Solution *> a_solutions; // pointer
    bool               newProject = false;
//    QGraphicsEllipseItem * ellipse;

    //HEURISTIC
    const int PROHIBITED_PCT = 5;
    const int N_EXEC         = 3;
    const int T_LIMIT        = 300;

    string default_directory          = "/home/elias/Documents/Mestrado/Projeto/tests/101/2_drones_v_01/brkga_10/";//"/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/tests/eil_101/A/mono/10_min/origin_2";//"/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/results/test/new_tests/eil_51/b_70/visit_10/s_15/struct"; // rat_195/b_90/visit_10/s_15
    string default_instance_directory = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances";

    //CANVAS
    const double POINT_SIZE  = 6;//10;
    const int    LINE_SIZE   = 3;//5;
    const double MARGIN_SIZE = 5;
          int QTY_COLUMNS    = 20;
          int QTY_LINES      = 20;
          double interval_horizontal_size;
          double interval_vertical_size  ;    

    void update_interval(Matrix * m_map);
    void print_map      (Matrix * m_map);
    void print_point    (Stop_Point point);
    void print_solution (Solution * a_solution);  // pointer
    void reset_canvas   ();
    void set_actual_play(int currentRow);

protected:
//    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *ev);
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_listPlays_currentRowChanged   (int currentRow)       ;
    void on_listPlays_itemClicked         (QListWidgetItem *item);
    void on_actionNew_triggered           ()                     ;
    void on_actionLoad_triggered          ()                     ;
    void on_actionTest_triggered          ()                     ;
    void on_actionVNS_triggered           ()                     ;
    void on_actionI_VNS_triggered         ()                     ;
    void on_actionEvaluate_triggered      ()                     ;
    void on_actionLoad_2_triggered        ()                     ;
    void on_actionAggregate_triggered     ()                     ;

    void on_actionNeural_Network_triggered()                     ;
//    void on_actionAuthomatic_Test_triggered();
};

#endif // MAINWINDOW_H
