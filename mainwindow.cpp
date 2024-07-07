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


//    for (int i = 0; i < 50; i++) {
//        Series s(&chart, "Имя" + QString::number(i));
//        s.setType(Gantt);
//        chart.addSeries(s);
//        for (int j = 0; j < 50; j++)
//            chart.getSeriesByName("Имя" + QString::number(i))->addXY(j*12+i, 10);
//    }

    Series s1(&chart, "Имя1");
    s1.setType(Gantt);
    chart.addSeries(s1);

    /*Series s2(&chart, "Имя2");
    s2.setType(Gantt);
    chart.addSeries(s2);*/


    for (int i = 0; i < 500000; i++) {
        chart.getSeriesByName("Имя1")->addXY(i*100+300, 50);
       // chart.getSeriesByName("Имя2")->addXY(i*70, 30);
    }

    /*Series s2(&chart, "Имя2");
    s2.setType(Gantt);
    chart.addSeries(s2);
    chart.getSeriesByName("Имя2")->addXY(40, 5);
    chart.getSeriesByName("Имя2")->addXY(50, 50);*/

    /*Series s3(&chart, "Имя3");
    s3.setType(Line);
    chart.addSeries(s3);
    chart.getSeriesByName("Имя3")->addXY(0, 0);
    chart.getSeriesByName("Имя3")->addXY(50, 50);*/

    /*chart.getSeriesByID(id1)->addXY(0, 0);
    chart.getSeriesByID(id1)->addXY(1, 10);
    chart.getSeriesByID(id1)->addXY(2, 0);
    chart.getSeriesByID(id1)->addXY(3, 10);
    chart.getSeriesByID(id1)->addXY(4, 0);
    chart.getSeriesByID(id1)->addXY(5, 10);
    chart.getSeriesByID(id1)->addXY(6, 15);
    chart.getSeriesByID(id1)->addXY(7, 0);*/

    //chart.plotByFile("C:\\Users\\Max\\Desktop\\new 12.txt", true, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

