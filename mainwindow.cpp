#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core.h"
#include "qpainter.h"
#include <QGridLayout>
#include <QListWidget>
#include <iostream>

using namespace fixedValues;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    for (int i = 0; i < 100; ++i) {
//        ui->listPlays->addItem("ITEM " + QString::number(i));
//    }

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
//    core.generate_solutions();

//   Pool_of_Solutions p_solutions = core.generate_solutions();
//   Array_Solution ** a_solutions = p_solutions.get_solutions();//core.get_solutions();

//   p_solutions.print();

//    Pool_of_Solutions p_solutions = core.get_pool_of_solutions();
//    cout << "\nFINAL POOL: " << endl;
//    p_solutions.print();

//    string s_item;

//    for (int i = 0; i < p_solutions.get_size(); i++) {
//        s_item = std::to_string(i);
//        s_item.append(". ");
//        s_item.append(a_solutions[i]->to_string());
//        ui->listPlays->addItem(QString::fromStdString(s_item));
//    }

//    cout << "fim";


//    renderArea = new Dialog;

//    QGridLayout *mainLayout = new QGridLayout;

//    mainLayout->addWidget(renderArea, 0, 0);

//    QListWidget *listWidget = new QListWidget;

//    for (int i = 0; i < 10; ++i) {
//        listWidget->addItem("ITEM" + QString::number(i));
//    }

//    mainLayout->addWidget(listWidget, 0, 1);

//    QWidget *placeholderWidget = new QWidget;
//    placeholderWidget->setLayout(mainLayout);
//    setCentralWidget(placeholderWidget);



//    setLayout(mainLayout);

//    QPalette pal = palette();

//    // set white background
//    pal.setColor(QPalette::Background, Qt::white);
//    ui->centralWidget->setAutoFillBackground(true);
//    ui->centralWidget->setPalette(pal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset_canvas()
{
    scene->clear();
    scene->setSceneRect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
}


void MainWindow::print_map(Matrix * m_map)
{    
//    ui->graphicsView->fitInView();

    QBrush blackBrush(Qt::black);
    QPen blackpen(Qt::black);


    QBrush redBrush(Qt::red);
    QPen redpen(Qt::red);

    QBrush blueBrush(Qt::blue);
    QPen bluepen(Qt::blue);

    update_interval(m_map);
//    blackpen.setWidth(6);

//    ui->graphicsView->x()

//    qDebug() << ui->graphicsView->viewport()->width() << " x " << ui->graphicsView->viewport()->height() << endl;

    int rows = m_map->get_rows();
    int cols = m_map->get_cols();


    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            print_point(m_map->get_stop_point(i, j));
//            if(j < QTY_COLUMNS - 1){
//                scene->addLine(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*j     + POINT_SIZE/2, MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*i + POINT_SIZE/2,
//                               MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*(j+1) + POINT_SIZE/2, MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*i + POINT_SIZE/2,
//                               blackpen);
//            }

//            if(i < QTY_LINES - 1){
//                scene->addLine(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*j + POINT_SIZE/2, MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*i     + POINT_SIZE/2,
//                               MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*j + POINT_SIZE/2, MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*(i+1) + POINT_SIZE/2,
//                               blackpen);
//            }

//            scene->addEllipse(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*j,
//                              MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*i,
//                              POINT_SIZE, POINT_SIZE, blackpen, blackBrush);
        }
    }
}

void MainWindow::showEvent( QShowEvent* event ) {
    QWidget::showEvent( event );
    //your code here
//    core.generate_solutions();
//    vector <Array_Solution> temp = core.get_solutions();
//    Array_Solution* a_solutions = core.get_solutions();

//    while (!temp.empty())
//      {
//        core.get_solutions().back().print();

//        ui->listPlays->addItem(QString::fromStdString(temp.back().to_string()));
//        temp.pop_back();
//      }

//    for (int i = 0; i < 20; ++i) {
//        ui->listPlays->addItem(QString::fromStdString(a_solutions[i].to_string()));

////        core.get_solutions().back().print();
////        core.get_solutions().pop_back();
////        ui->listPlays->addItem("ITEM " + QString::number(i));
//    }

    reset_canvas();
//    print_map(core.get_map());
}

void MainWindow::update_interval(Matrix * m_map)
{
    interval_horizontal_size = (ui->graphicsView->viewport()->width()  - 2 * MARGIN_SIZE - m_map->get_cols() * POINT_SIZE)/(m_map->get_cols() - 1);
    interval_vertical_size   = (ui->graphicsView->viewport()->height() - 2 * MARGIN_SIZE - m_map->get_rows() * POINT_SIZE)/(m_map->get_rows() - 1);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   // Your code here.
   reset_canvas();
   if(newProject){       
       print_map(core.get_map());
   }
}

//void MainWindow::paintEvent(QPaintEvent *e)
//{
//    QPainter painter(this);
//    painter.drawLine(10, 10, 100, 100);
//}

void MainWindow::print_point(Stop_Point point)
{
//    QBrush gray_brush(Qt::gray);
//    QPen gray_pen(Qt::gray);

//    QBrush red_brush(Qt::red);
//    QPen red_pen(Qt::red);

//    QBrush blue_brush(Qt::blue);
//    QPen blue_pen(Qt::blue);

//    QBrush green_brush(Qt::green);
//    QPen green_pen(Qt::green);

    QColor color;

    switch (point.get_type()) {
    case STOPS::NOTHING:
        color = QColor(Qt::gray);
//        actual_brush = gray_brush;
//        actual_pen = gray_pen;
        break;
    case STOPS::COLLECT:
        color = QColor(Qt::blue);
//        color = QColor(Qt::yellow);
//        actual_brush = red_brush;
//        actual_pen = red_pen;
        break;
    case STOPS::DELIVER:
        color = QColor(Qt::blue);
//        actual_brush = blue_brush;
//        actual_pen = blue_pen;
        break;
    case STOPS::CHARGE:
        color = QColor(Qt::green);
//        actual_brush = green_brush;
//        actual_pen = green_pen;
        break;
    case STOPS::PROHIBITED:
        color = QColor(Qt::red);
//        actual_brush = green_brush;
//        actual_pen = green_pen;
        break;
    }

    QBrush actual_brush(color);
    QPen actual_pen(color);

    scene->addEllipse(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*point.get_x(),
                      MARGIN_SIZE + (interval_vertical_size   + POINT_SIZE)*point.get_y(),
                      POINT_SIZE, POINT_SIZE, actual_pen, actual_brush);
}

void MainWindow::print_solution(Solution * a_solution)
{
    QPen black_pen(Qt::black);
    black_pen.setWidth(LINE_SIZE);


//    if(a_solution.get_size() > 0){
//        scene->addLine(MARGIN_SIZE + 0 + POINT_SIZE/2, MARGIN_SIZE + 0 + POINT_SIZE/2,
//                       MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE)*a_solution.get_solution_pair(0).get_x() + POINT_SIZE/2, MARGIN_SIZE + (interval_vertical_size + POINT_SIZE)*a_solution.get_solution_pair(0).get_y() + POINT_SIZE/2,
//                       black_pen);
//    }

    for (int pos_route = 0; pos_route < a_solution->get_size(); pos_route++) {
        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;

        QColor color(r, g, b, 1);

        for (int i = 0; i < a_solution->uavs[pos_route]->get_size() - 1; ++i) {
//            color.setAlpha((int) (i * 200 / (a_solution->get_routes()[pos_route]->get_size())) + 5);
            color.setAlpha(255);
            black_pen.setColor(color);
            scene->addLine(MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE) * a_solution->uavs[pos_route]->get_movement(  i)->get_x() + POINT_SIZE/2,
                           MARGIN_SIZE + (  interval_vertical_size + POINT_SIZE) * a_solution->uavs[pos_route]->get_movement(  i)->get_y() + POINT_SIZE/2,
                           MARGIN_SIZE + (interval_horizontal_size + POINT_SIZE) * a_solution->uavs[pos_route]->get_movement(i+1)->get_x() + POINT_SIZE/2,
                           MARGIN_SIZE + (  interval_vertical_size + POINT_SIZE) * a_solution->uavs[pos_route]->get_movement(i+1)->get_y() + POINT_SIZE/2,
                           black_pen);
        }
    }
}

void MainWindow::on_listPlays_currentRowChanged(int currentRow)
{
    set_actual_play(currentRow);
}

void MainWindow::on_listPlays_itemClicked(QListWidgetItem *item)
{
    set_actual_play(ui->listPlays->row(item));
}

void MainWindow::set_actual_play(int currentRow) // pointer
{
//    std::vector<Array_Solution *> plays = core.get_solutions();
    Solution * play = a_solutions[currentRow];

    string s_info_play;
    s_info_play.append("Play #");
    s_info_play.append(std::to_string(currentRow));

//    s_info_play.append("\n\nCollect: ");
//    s_info_play.append(std::to_string(play->get_qty_collect()));

//    s_info_play.append("\nInspection Points: ");
//    s_info_play.append(std::to_string(play.get_qty_deliver()));

//    s_info_play.append("\nClients : ");
//    s_info_play.append(std::to_string((int) play->get_z()[0]));

//    s_info_play.append("\nCharge : ");
//    s_info_play.append(std::to_string(play.get_qty_charge()));

    s_info_play.append("\nSize : ");
//    s_info_play.append(std::to_string(play->get_size()));

    for (int pos_route = 0; pos_route < play->get_size(); pos_route++) {
        s_info_play.append(std::to_string(play->uavs[pos_route]->get_size()));
        s_info_play.append(" ");
    }

//    s_info_play.append("\nTotal points : ");
//    s_info_play.append(std::to_string((-1) * (int) play.get_points()));

    s_info_play.append("\nFinal Charge : ");
    s_info_play.append(std::to_string(play->objectives[1]));

    s_info_play.append("\nTime : ");
    s_info_play.append(std::to_string((-1) * play->objectives[2]));

    s_info_play.append("\nConsumption : ");
    s_info_play.append(std::to_string((-1) * play->objectives[3]));

    s_info_play.append("\nIteration : ");
    s_info_play.append(std::to_string(play->iteration));

    s_info_play.append("\nValid : ");
    s_info_play.append(play->valid ? "true" : "false");

    s_info_play.append("\n\nVelocity : \n\n");
    for (int pos_route = 0; pos_route < play->get_size(); pos_route++) {
        for (int i = 0; i < play->uavs[pos_route]->get_size(); i++) {
            s_info_play.append(std::to_string(play->uavs[pos_route]->get_movement(i)->vel));
            s_info_play.append(" | ");
        }

        s_info_play.append("\n\n");
    }

    s_info_play.append("\n\nCharge Rate : \n\n");
    Matrix * m_map = core.get_map();
    for (int pos_route = 0; pos_route < play->get_size(); pos_route++) {
        for (int i = 0; i < play->uavs[pos_route]->get_size(); i++) {
            int type = m_map->get_stop_point(play->uavs[pos_route]->get_movement(i)->get_x(), play->uavs[pos_route]->get_movement(i)->get_y()).get_type();
            if(type == STOPS::CHARGE){
                s_info_play.append(std::to_string(play->uavs[pos_route]->get_movement(i)->charge_rate));
                s_info_play.append(" | ");
            }
        }
        s_info_play.append("\n\n");
    }


//    ui->teSelectedPlay->setText(QString::number(ui->listPlays->row(item)));//item->text());
    ui->teSelectedPlay->setText(QString::fromStdString(s_info_play));
//    ui->textEdit_2->setText(QString::fromStdString(play->to_string()));

    reset_canvas();
    print_solution(play);
    print_map(core.get_map());
}

void MainWindow::on_actionNew_triggered()
{
//    newProject = true;
//    ui->listPlays->clear();
////    p_solutions = core.generate_solutions();
////    vector<Array_Solution *> a_solutions = p_solutions.get_solutions();
//    a_solutions = core.generate_solutions(0);
//    reset_canvas();
//    print_map(core.get_map());

//    string s_item;

//    for (int i = 0; i < a_solutions.size(); i++) {
//        s_item = std::to_string(i);
//        s_item.append(". ");
//        s_item.append(a_solutions[i]->to_string());
//        ui->listPlays->addItem(QString::fromStdString(s_item));
//    }
}

void MainWindow::on_actionLoad_triggered()
{
//    QString fileName =  QFileDialog::getOpenFileName(
//              this,
//              "Open Document",
//              QDir::currentPath(),
//              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

//    newProject = true;
//    ui->listPlays->clear();
//    a_solutions = core.generate_solutions(1, fileName.toUtf8().constData());
//    reset_canvas();
//    print_map(core.get_map());

//    string s_item;

//    for (int i = 0; i < a_solutions.size(); i++) {
//        s_item = std::to_string(i);
//        s_item.append(". ");
//        s_item.append(a_solutions[i]->to_string());
//        ui->listPlays->addItem(QString::fromStdString(s_item));
//    }
}

void MainWindow::on_actionTest_triggered()
{
//    newProject = true;
//    ui->listPlays->clear();
//    a_solutions = core.generate_solutions(2);
//    reset_canvas();
//    print_map(core.get_map());

//    string s_item;

//    for (int i = 0; i < a_solutions.size(); i++) {
//        s_item = std::to_string(i);
//        s_item.append(". ");
//        s_item.append(a_solutions[i]->to_string());
//        ui->listPlays->addItem(QString::fromStdString(s_item));
//    }
}

void MainWindow::on_actionVNS_triggered()
{
    bool ok;
    QInputDialog qDialog;
    QStringList items;
    items << QString("SWAP 3");
    items << QString("SWAP 4");
    items << QString("SWAP 4 and SWAP 5");
    items << QString("SWAP 4 and SWAP 6");
    items << QString("SWAP 6");

    //Instance file
    QString instance_name =  QFileDialog::getOpenFileName(
              this,
              "Choose Instance",
              QString::fromUtf8(default_instance_directory.c_str()),
//              QDir::currentPath(),
              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

    cout << instance_name.toUtf8().constData() << endl;

    //Directory file
    QString directoryName =  QFileDialog::getExistingDirectory(
              this,
              "Choose Directory",
//              QDir::currentPath()
              QString::fromUtf8(default_directory.c_str()));

    cout << directoryName.toUtf8().constData() << endl;

    //Methods
    QString text = qDialog.getItem(0, "VNS", "Methods:", items);
//    qDialog.exec();

    int method = items.indexOf(text.toUtf8().constData());
    cout << method << endl;

    std::string::size_type sz;   // alias of size_t
    //PROHIBITED POINTS
    text = qDialog.getText(0, "VNS", "% of prohibited points:", QLineEdit::Normal, "", &ok);
    int time_limit, n_executions, p_prohibited_points;
    try{
        p_prohibited_points = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        p_prohibited_points = 5;
    }
    cout << p_prohibited_points << endl;

    //TIME
    text = qDialog.getText(0, "VNS", "Time limit:", QLineEdit::Normal, "", &ok);
    try{
        time_limit = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        time_limit = 300;
    }
    cout << time_limit << endl;


    //# EXECUTIONS
    text = qDialog.getText(0, "VNS", "# of executions:", QLineEdit::Normal, "", &ok);    
    try{
        n_executions = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        n_executions = 3;
    }
    cout << n_executions << endl;   

//    a_solutions = core.test(1, method, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

    reset_canvas();
    print_map(core.get_map());

    string s_item;

    for (int i = 0; i < a_solutions.size(); i++) {
        s_item = std::to_string(i);
        s_item.append(". ");
        s_item.append(a_solutions[i]->to_string());
        ui->listPlays->addItem(QString::fromStdString(s_item));
    }

//    QMessageBox qMsgBox;
//    qMsgBox.setText("Tests executed with success.");
//    qMsgBox.exec();

//    newProject = true;
//    ui->listPlays->clear();
//    a_solutions = core.generate_solutions(2);
//    reset_canvas();
}

void MainWindow::on_actionAggregate_triggered()
{
    //Directory file
    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/A/origin_1";
    QString directoryName =  QFileDialog::getExistingDirectory(
              this,
              "Choose Directory",
              QString::fromUtf8(default_directory.c_str()));
//              QDir::currentPath());

    cout << directoryName.toUtf8().constData() << endl;

    core.aggregate_STRUCT(3, N_EXEC, default_directory);

    QMessageBox qMsgBox;
    qMsgBox.setText("Tests executed with success.");
    qMsgBox.exec();

    //END: Multidrone
}

void MainWindow::on_actionI_VNS_triggered()
{
    bool ok;
    QInputDialog qDialog;
    QStringList items;
    items << QString("SWAP Complete");
    items << QString("SWAP Nearest");
    items << QString("SWAP Nearest and SWAP Furthest");
    items << QString("SWAP Nearest and SWAP Random");
    items << QString("SWAP Random");

    //Instance file
//    QString instance_name =  QFileDialog::getOpenFileName(
//              this,
//              "Choose Instance",
//              QString::fromUtf8(default_instance_directory.c_str()),
////              QDir::currentPath(),
//              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

//    cout << instance_name.toUtf8().constData() << endl;

    //Directory file
//    QString directoryName =  QFileDialog::getExistingDirectory(
//              this,
//              "Choose Directory",
//              QString::fromUtf8(default_directory.c_str()));
////              QDir::currentPath());

//    cout << directoryName.toUtf8().constData() << endl;


    //Methods
//    QString text = qDialog.getItem(0, "I-VNS", "Methods:", items);


//    qDialog.exec();

//    int method = items.indexOf(text.toUtf8().constData());
//    cout << method << endl;

    std::string::size_type sz;   // alias of size_t
    //PROHIBITED POINTS
//    text = qDialog.getText(0, "I-VNS", "% of prohibited points:", QLineEdit::Normal, "", &ok);
    int time_limit, n_executions, p_prohibited_points;
//    try{
//        p_prohibited_points = std::stoi (text.toUtf8().constData(),&sz);
//    }catch(exception e){
//        p_prohibited_points = PROHIBITED_PCT;
//    }
    p_prohibited_points = PROHIBITED_PCT;
    cout << p_prohibited_points << endl;

    //TIME
//    text = qDialog.getText(0, "I-VNS", "Time limit:", QLineEdit::Normal, "", &ok);
//    try{
//        time_limit = std::stoi (text.toUtf8().constData(),&sz);
//    }catch(exception e){
//        time_limit = T_LIMIT;
//    }
    time_limit = T_LIMIT;
    cout << time_limit << endl;


    //# EXECUTIONS
//    text = qDialog.getText(0, "VNS", "# of executions:", QLineEdit::Normal, "", &ok);
//    try{
//        n_executions = std::stoi (text.toUtf8().constData(),&sz);
//    }catch(exception e){
//        n_executions = N_EXEC;
//    }
    n_executions = N_EXEC;
    cout << n_executions << endl;    

//    a_solutions = core.test(2, method, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

    default_directory    = "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/A/origin_1";
    string instance_file = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances/eil51.tsp";
    string brkga_file    = "/brkga/solutions/solution_0";
    string load_file     = default_directory + brkga_file;

    time_limit = 60;

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, true, load_file);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/A/origin_2";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/B/origin_1";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/eil_51/B/origin_2";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

     //101

    time_limit = 60;

    default_directory = "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/A/origin_1";
    instance_file     = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances/eil101.tsp";
    load_file         = default_directory + brkga_file;
    bool load = true;

    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);

    default_directory = "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/A/origin_2";
    load_file         = default_directory + brkga_file;

    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);

    default_directory = "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/B/origin_1";
    load_file         = default_directory + brkga_file;

    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);

    default_directory = "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/B/origin_2";
    load_file         = default_directory + brkga_file;

    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);
    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points, load, load_file);

    //195

    time_limit = 900;

    default_directory   = "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/A/origin_1";
    instance_file       = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances/rat195.tsp";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

//    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/A/origin_2";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

//    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/B/origin_1";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

//    default_directory          = "/home/elias/Documents/Mestrado/Projeto/instances/rat_195/B/origin_2";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

//    default_directory          = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/tests/eil_101/B/mono/10_min/origin_1_mono_5";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

//    default_directory          = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/tests/eil_101/B/mono/10_min/origin_2_mono_5";

//    core.test(2, 0, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 1, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 2, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 3, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);
//    core.test(2, 4, instance_file, default_directory, time_limit, n_executions, p_prohibited_points);

//    Core core0;
//    core0.test(2, 0, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

//    Core core1;
//    core1.test(2, 1, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

//    Core core2;
//    core2.test(2, 2, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

//    Core core3;
//    core3.test(2, 3, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

//    Core core4;
//    core4.test(2, 4, instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, n_executions, p_prohibited_points);

    //START: Multidrone

//    reset_canvas();
//    print_map(core.get_map());

//    string s_item;

//    for (int i = 0; i < a_solutions.size(); i++) {
//        s_item = std::to_string(i);
//        s_item.append(". ");
//        s_item.append(a_solutions[i]->to_string());
//        ui->listPlays->addItem(QString::fromStdString(s_item));
//    }

    QMessageBox qMsgBox;
    qMsgBox.setText("Tests executed with success.");
    qMsgBox.exec();

    //END: Multidrone


//    newProject = true;
//    ui->listPlays->clear();
//    a_solutions = core.generate_solutions(2);
//    reset_canvas();
}

void MainWindow::on_actionEvaluate_triggered()
{
//    bool ok;
//    QInputDialog qDialog;
//    QStringList items;
//    items << QString("SWAP 3");
//    items << QString("SWAP 4");
//    items << QString("SWAP 4 and SWAP 5");
//    items << QString("SWAP 4 and SWAP 6");

//    QString text = qDialog.getItem(0, "I-VNS", "Methods:", items);
////    qDialog.exec();

//    int method = items.indexOf(text.toUtf8().constData());
//    cout << method << endl;

//    std::string::size_type sz;   // alias of size_t
//    text = qDialog.getText(0, "I-VNS", "Time limit:", QLineEdit::Normal, "", &ok);
//    int time_limit = std::stoi (text.toUtf8().constData(),&sz);
//    cout << time_limit << endl;


//    text = qDialog.getText(0, "I-VNS", "# of executions:", QLineEdit::Normal, "", &ok);
//    int n_executions = std::stoi (text.toUtf8().constData(),&sz);
//    cout << n_executions << endl;

//    QString fileName =  QFileDialog::getOpenFileName(
//              this,
//              "Open Document",
//              QDir::currentPath(),
//              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

    //Directory name
    QString directory_name =  QFileDialog::getExistingDirectory(
              this,
              "Open Directory",
//              QDir::currentPath()
              QString::fromUtf8(default_directory.c_str()));
//                ,              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

    cout << directory_name.toUtf8().constData() << endl;

    //Number of executions
    QInputDialog qDialog;
    bool ok;
    int n_executions;
    std::string::size_type sz;   // alias of size_t
    QString text = qDialog.getText(0, "VNS", "# of executions:", QLineEdit::Normal, "", &ok);
    try{
        n_executions = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        n_executions = N_EXEC;
    }
    cout << n_executions << endl;

    core.evaluate_methods(directory_name.toUtf8().constData(), n_executions);

    QMessageBox qMsgBox;
    qMsgBox.setText("Evaluation of methods executed with success.");
    qMsgBox.exec();
}

void MainWindow::on_actionLoad_2_triggered()
{
    QString directory_name =  QFileDialog::getExistingDirectory(
              this,
              "Open Directory",
//              QDir::currentPath()
              QString::fromUtf8(default_directory.c_str()));
//                ,              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

    cout << directory_name.toUtf8().constData() << endl;

    string folder_name = directory_name.toUtf8().constData();    

    //Number of executions
    QInputDialog qDialog;
    bool ok;
    std::string::size_type sz;   // alias of size_t
    int n_executions;

    //# EXECUTIONS
    QString text = qDialog.getText(0, "VNS", "# of executions:", QLineEdit::Normal, "", &ok);
    try{
        n_executions = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        n_executions = 3;
    }
    cout << n_executions << endl;

    newProject = true;
    ui->listPlays->clear();
    a_solutions = core.show_solutions(n_executions, 3, folder_name);

    reset_canvas();
    print_map(core.get_map());

    string s_item;

    for (int i = 0; i < a_solutions.size(); i++) {
        s_item = std::to_string(i);
        s_item.append(". ");
        s_item.append(a_solutions[i]->to_string());
        ui->listPlays->addItem(QString::fromStdString(s_item));
    }

    QMessageBox qMsgBox;
    qMsgBox.setText("Tests executed with success.");
    qMsgBox.exec();

}

void MainWindow::on_actionNeural_Network_triggered()
{
    bool ok;
    QInputDialog qDialog;

    //Instance file
    QString instance_name =  QFileDialog::getOpenFileName(
              this,
              "Choose Instance",
              QString::fromUtf8(default_instance_directory.c_str()),
              "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

    cout << instance_name.toUtf8().constData() << endl;

    //Directory file
    QString directoryName =  QFileDialog::getExistingDirectory(
              this,
              "Choose Directory",
              QString::fromUtf8(default_directory.c_str()));

    cout << directoryName.toUtf8().constData() << endl;


    std::string::size_type sz;   // alias of size_t

    //PROHIBITED POINTS
    QString text = qDialog.getText(0, "Neural Network", "% of prohibited points:", QLineEdit::Normal, "", &ok);
    int time_limit, n_executions, p_prohibited_points;
    try{
        p_prohibited_points = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        p_prohibited_points = 5;
    }
    cout << p_prohibited_points << endl;

    //TIME
    text = qDialog.getText(0, "Neural Network", "Time limit:", QLineEdit::Normal, "", &ok);
    try{
        time_limit = std::stoi (text.toUtf8().constData(),&sz);
    }catch(exception e){
        time_limit = 300;
    }
    cout << time_limit << endl;


//    a_solutions = core.neural_network(instance_name.toUtf8().constData(), directoryName.toUtf8().constData(), time_limit, p_prohibited_points);

    //START: Multidrone

    reset_canvas();
    print_map(core.get_map());

    string s_item;

    for (int i = 0; i < a_solutions.size(); i++) {
        s_item = std::to_string(i);
        s_item.append(". ");
        s_item.append(a_solutions[i]->to_string());
        ui->listPlays->addItem(QString::fromStdString(s_item));
    }

    QMessageBox qMsgBox;
    qMsgBox.setText("Tests executed with success.");
    qMsgBox.exec();

    //END: Multidrone
}
