#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->horizontalLayout->addWidget(&chart);
    chart.showLegend(ui->horizontalLayout, true);
    chart.getLegend()->setFont(QFont("TimesNewRoman", 14));
    chart.setAntialiased(true);

    QFont f = chart.getTextFont();
    f.setPointSize(14);
    chart.setTextFont(f);

    chart.setMarginBottom(50);

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    this->setPalette(pal);

    chart.setPalette(pal);

    /*Series s1(&chart);
    Series s2(&chart);



    s1.setPen(QPen(Qt::red, 3));
    s2.setPen(QPen(Qt::blue, 3));

    int id1 = chart.addSeries(s1);
    int id2 = chart.addSeries(s2);

    for (double x = 0; x < 100; x+=0.1) {
        chart.getSeriesByID(id1)->addXY(x, sin(x));
        chart.getSeriesByID(id2)->addXY(x, cos(x));
    }*/

    chart.plotByFile("C:\\Users\\Max\\Desktop\\new 12.txt", true, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

