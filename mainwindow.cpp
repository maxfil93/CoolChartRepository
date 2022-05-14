#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->verticalLayout->addWidget(&chart);

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    this->setPalette(pal);

    chart.setPalette(pal);

    Series s1(&chart);
    Series s2(&chart);

    s1.setPen(QPen(Qt::red));
    s2.setPen(QPen(Qt::blue));

    int id1 = chart.addSeries(s1);
    int id2 = chart.addSeries(s2);

    for (double x = 0; x < 100; x+=0.1) {
        chart.getSeriesByID(id1)->addXY(x, sin(x));
        chart.getSeriesByID(id2)->addXY(x, cos(x));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

