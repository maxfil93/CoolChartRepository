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

    QFont f = chart.getTextFont(FTitle);
    f.setPointSize(14);
    chart.setTextFont(f, Qt::white, FTitle);

    QFont f1 = chart.getTextFont(FAxisXTitle);
    f1.setPointSize(14);
    chart.setTextFont(f1, Qt::white, FAxisXTitle);

    QFont f2 = chart.getTextFont(FAxisYTitle);
    f2.setPointSize(14);
    chart.setTextFont(f2, Qt::white, FAxisYTitle);

    chart.setTitle("График1");
    chart.setXTitle("Время, с");
    chart.setYTitle("Амплитуда, В");

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    this->setPalette(pal);

    chart.setPalette(pal);

    Series s1(&chart);
    Series s2(&chart);

    s1.setPen(QPen(Qt::red, 3));
    s2.setPen(QPen(Qt::blue, 3));

    int id1 = chart.addSeries(s1);
    int id2 = chart.addSeries(s2);

    for (double x = 0; x < 1000000; x+=1) {
        chart.getSeriesByID(id1)->addXY(x, sin(x));
        chart.getSeriesByID(id2)->addXY(x, cos(x));
    }

    /*chart.getSeriesByID(id1)->addXY(0, 0);
    chart.getSeriesByID(id1)->addXY(1, 10);
    chart.getSeriesByID(id1)->addXY(2, 0);
    chart.getSeriesByID(id1)->addXY(3, 10);
    chart.getSeriesByID(id1)->addXY(4, 0);
    chart.getSeriesByID(id1)->addXY(5, 10);
    chart.getSeriesByID(id1)->addXY(6, 15);
    chart.getSeriesByID(id1)->addXY(7, 0);*/

    chart.plotByFile("C:\\Users\\Max\\Desktop\\new 12.txt", true, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

