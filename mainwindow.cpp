#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->verticalLayout->addWidget(&chart);

    Series s(&chart);

    int id = chart.addSeries(s);

    for (double x = 0; x < 100; x+=0.1)
        chart.getSeriesByID(id)->addXY(x, sin(x));
}

MainWindow::~MainWindow()
{
    delete ui;
}

