#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <math.h>
#include <QRandomGenerator>
#include <iostream>

void MainWindow::fun(int chart_num, char* fn, unsigned int start_str, unsigned int num_str)
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

    if (chart[chart_num].getSeriesByName(id) == nullptr) {
        Series s(&chart[chart_num], id);
        s.setType(Gantt);
        chart[chart_num].addSeries(s);
    }

    chart[chart_num].getSeriesByName(id)->addXY((r_mks-first_time_mks)/1000000.0, (r_dlc * 119/8) / 1000000.0);

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

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    this->setPalette(pal);

    for (int i = 0; i < 4; i++) {
        chart[i].getLegend()->setFont(QFont("TimesNewRoman", 12));
        chart[i].getLegend()->setMaximumWidth(150);

        chart[i].setAntialiased(true);

        QFont f1 = chart[i].getTextFont(FTitle);
        f1.setPointSize(14);
        chart[i].setTextFont(f1, Qt::white, FTitle);

        QFont f2 = chart[i].getTextFont(FAxisXTitle);
        f2.setPointSize(14);
        chart[i].setTextFont(f2, Qt::white, FAxisXTitle);

        QFont f3 = chart[i].getTextFont(FAxisYTitle);
        f3.setPointSize(14);
        chart[i].setTextFont(f3, Qt::white, FAxisYTitle);

        chart[i].setTitle("График " + QString::number(i));
        chart[i].setXTitle("Время, мкс");
        chart[i].setYTitle("№");

        chart[i].setPalette(pal);

        chart[i].setAutoXLimits(true);
        chart[i].setAutoYLimits(true);
    }

    chart[0].getLegend()->setFont(QFont("TimesNewRoman", 12));
    ui->horizontalLayout->addWidget(&chart[0]);
    chart[0].showLegend(ui->horizontalLayout, true);

    chart[1].getLegend()->setFont(QFont("TimesNewRoman", 12));
    ui->horizontalLayout->addWidget(&chart[1]);
    chart[1].showLegend(ui->horizontalLayout, true);

    chart[2].getLegend()->setFont(QFont("TimesNewRoman", 12));
    ui->horizontalLayout_3->addWidget(&chart[2]);
    chart[2].showLegend(ui->horizontalLayout_3, true);

    chart[3].getLegend()->setFont(QFont("TimesNewRoman", 12));
    ui->horizontalLayout_3->addWidget(&chart[3]);
    chart[3].showLegend(ui->horizontalLayout_3, true);

    //chart.setShowBorderForGantt(true);
    //chart.setShowFreeTimeForGantt(true);
    //chart.setShowCaptionForGantt(true);

    chart[2].plotByFile("E:\\dataset.txt", true, false);
    for (int i = 0; i < chart[2].getSeries()->size(); i++) {
        QPen p = chart[2].getSeries()->operator[](i).getPen();
//        p.setWidth(3);
//        chart[2].getSeries()->operator[](i).setPen(p);
//        chart[2].getSeries()->operator[](i).setType(Circles);
    }

    GenerateRandom(&chart[0], 10, 100, Gantt);

    timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, &MainWindow::onTimer1);
    timer1->start(10);

//    Series s(&chart[3], "sss");
//    s.addXY(0,0);
//    s.addXY(10,7);
//    s.addXY(-5,DBL_MAX);
//    chart[3].addSeries(s);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
//    LineSeriesTest(0);
//    GanttSeriesTest(0);
//    GenerateRandom(0, 10, 100);
//    fun(ui->lineEdit->text().toLocal8Bit().data(), 0, 999999);
}

void MainWindow::LineSeriesTest(int chart_num)
{
    static unsigned int cnt = 0;
    static unsigned int addded_ser = 0;
    if (cnt % 10 == 0) {
        Series s(&chart[chart_num], "Name" + QString::number(addded_ser++));
        s.setType(Line);
        chart[chart_num].addSeries(s);
    }
    else {
        int scx = 1;
        if (addded_ser % 2 == 0) scx = -1;
        int scy = 1;
        if (addded_ser % 3 == 0) scy = -1;
        chart[chart_num].getSeriesByID(addded_ser - 1)->addXY(
                    (double)scx*((double)addded_ser * (double)addded_ser + (double)cnt),
                    (double)scy*((double)addded_ser + (double)cnt)
                                                  );
    }

    cnt++;
}

void MainWindow::GanttSeriesTest(int chart_num)
{
    static unsigned int cnt = 0;
    static unsigned int addded_ser = 0;
    if (cnt % 10 == 0) {
        Series s(&chart[chart_num], "Name" + QString::number(addded_ser++));
        s.setType(Gantt);
        chart[chart_num].addSeries(s);
    }
    else {
        int scx = 1;
        if (addded_ser % 2 == 0) scx = -1;
        int scy = 1;
        if (addded_ser % 3 == 0) scy = -1;
        chart[chart_num].getSeriesByID(addded_ser - 1)->addXY(
                    (double)scx*((double)addded_ser * (double)addded_ser + (double)cnt),
                    (double)scy*((double)addded_ser + (double)cnt)
                                                  );
    }

    cnt++;
}

void MainWindow::GenerateRandom(CoolChart* ch, unsigned int ser_num, unsigned int points_num, SeriesType type)
{
    ch->clear();
    for (int i = 0 ; i < ser_num; i++) {
        Series s(ch, "Name" + QString::number(i));

        QRandomGenerator::global()->generate();

        s.setType(type);
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
        ch->addSeries(s);
    }
}


void MainWindow::onTimer1()
{
    static bool first = true;
    static double x = 0.0;
    static int id_sin = 0;
    static int id_cos = 0;
    if (first) {
        Series s1(&chart[1], "sin");
        id_sin = chart[1].addSeries(s1);
        Series s2(&chart[1], "cos");
        id_cos = chart[1].addSeries(s2);
        first = false;
    }
    chart[1].getSeriesByID(id_sin)->addXY(x, sin(x));
    chart[1].getSeriesByID(id_cos)->addXY(x, cos(x));
    x += 0.1;

    if (chart[1].getSeriesByID(id_sin)->getXY()->size() > 1000) {
        x = 0;
        chart[1].getSeriesByID(id_sin)->clear();
        chart[1].getSeriesByID(id_cos)->clear();
    }
}
