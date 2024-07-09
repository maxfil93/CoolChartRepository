#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <math.h>
#include <QRandomGenerator>
#include <iostream>

void MainWindow::fun(char* fn, unsigned int start_str, unsigned int num_str)
{
FILE* F = fopen(fn, "rt");
if (F == NULL)
    {
    return;
    }
unsigned int cnt = 0;
unsigned int first_time_mks = -1;

while(!feof(F))
    {
    char rx[5] = {0};
    unsigned int num = 0;
    char sff[5] = {0};
    char id[10] = {0};
    unsigned int dlc = 0;
    char hex[10] = {0};

    fscanf(F, "%s %d %s %s %d %s", rx, &num, sff, id, &dlc, hex);

    if (feof(F)) break;


    for (int i = 0; i < dlc; i++)
        {
        char data_s[5] = {0};
        fscanf(F, "%s", data_s);
        }

    char time1_s[20] = {0};
    fscanf(F, "%s", time1_s);
    unsigned int time1_int = atoi(time1_s);

    if (first_time_mks == -1) {
        first_time_mks = time1_int;
    }

    char date[20] = {0};
    char time2[20] = {0};
    char date3[20] = {0};
    char date4[20] = {0};
    fscanf(F, "%s %s %s %s", date, &time2, date3, date4);

    if (num < start_str) continue;
    if (cnt >= num_str) break;

    QString id_qs(id);

    bool ok = false;

    unsigned short r_id  = id_qs.toInt(&ok, 16);
    unsigned int r_mks = time1_int;
    unsigned short r_dlc = dlc;

    if (chart.getSeriesByName(id) == nullptr) {
        Series s(&chart, id);
        s.setType(Gantt);
        chart.addSeries(s);
    }

    chart.getSeriesByName(id)->addXY((r_mks-first_time_mks)/1000000.0, (r_dlc * 119/8) / 1000000.0);

    cnt++;
    }
fclose(F);
}
//---------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->horizontalLayout->addWidget(&chart);
    chart.showLegend(ui->horizontalLayout, true);
    chart.getLegend()->setFont(QFont("TimesNewRoman", 12));
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
    chart.setXTitle("Время, мкс");
    chart.setYTitle("№");

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    this->setPalette(pal);

    chart.setPalette(pal);

    chart.setAutoXLimits(true);
    chart.setAutoYLimits(true);
   // chart.setShowBorderForGantt(true);
    //chart.setShowFreeTimeForGantt(true);
    //chart.setShowCaptionForGantt(true);

    //chart.plotByFile("C:\\Users\\Max\\Desktop\\new 12.txt", true, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
//    LineSeriesTest();
//    GanttSeriesTest();
    GenerateRandom(10, 100);
//    fun(ui->lineEdit->text().toLocal8Bit().data(), 0, 999999);
}

void MainWindow::LineSeriesTest()
{
    static unsigned int cnt = 0;
    static unsigned int addded_ser = 0;
    if (cnt % 10 == 0) {
        Series s(&chart, "Name" + QString::number(addded_ser++));
        s.setType(Line);
        chart.addSeries(s);
    }
    else {
        int scx = 1;
        if (addded_ser % 2 == 0) scx = -1;
        int scy = 1;
        if (addded_ser % 3 == 0) scy = -1;
        chart.getSeriesByID(addded_ser - 1)->addXY(
                    (double)scx*((double)addded_ser * (double)addded_ser + (double)cnt),
                    (double)scy*((double)addded_ser + (double)cnt)
                                                  );
    }

    cnt++;
}

void MainWindow::GanttSeriesTest()
{
    static unsigned int cnt = 0;
    static unsigned int addded_ser = 0;
    if (cnt % 10 == 0) {
        Series s(&chart, "Name" + QString::number(addded_ser++));
        s.setType(Gantt);
        chart.addSeries(s);
    }
    else {
        int scx = 1;
        if (addded_ser % 2 == 0) scx = -1;
        int scy = 1;
        if (addded_ser % 3 == 0) scy = -1;
        chart.getSeriesByID(addded_ser - 1)->addXY(
                    (double)scx*((double)addded_ser * (double)addded_ser + (double)cnt),
                    (double)scy*((double)addded_ser + (double)cnt)
                                                  );
    }

    cnt++;
}

void MainWindow::GenerateRandom(unsigned int ser_num, unsigned int points_num)
{
    chart.clear();
    for (int i = 0 ; i < ser_num; i++) {
        Series s(&chart, "Name" + QString::number(i));

        QRandomGenerator::global()->generate();

        s.setType(Gantt);
        QPen p = s.getPen();
        p.setWidth(3);
        s.setPen(p);

        for (int j = 0 ; j < points_num; j++) {
            double xmin = (j*10-2);
            double xmax = 4;
            double x = xmin + QRandomGenerator::global()->bounded(xmax);

            double wmax = 10;
            double w = QRandomGenerator::global()->bounded(wmax);
            s.addXY(x, w);
        }
        chart.addSeries(s);
    }
}
