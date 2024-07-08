#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <math.h>

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
    chart.setXTitle("Время, мкс");
    chart.setYTitle("№");

    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    this->setPalette(pal);

    chart.setPalette(pal);

    chart.setShowBorderForGantt(true);
     chart.setShowFreeTimeForGantt(true);

//    for (int i = 0; i < 50; i++) {
//        Series s(&chart, "Имя" + QString::number(i));
//        s.setType(Gantt);
//        chart.addSeries(s);
//        for (int j = 0; j < 50; j++)
//            chart.getSeriesByName("Имя" + QString::number(i))->addXY(j*12+i, 10);
//    }



//    Series s1(&chart, "Имя1");
//    s1.setType(Gantt);
//    chart.addSeries(s1);

    /*Series s2(&chart, "Имя2");
    s2.setType(Gantt);
    chart.addSeries(s2);*/


//    for (int i = 0; i < 500; i++) {
//        chart.getSeriesByName("Имя1")->addXY(i*100+300, 50);
//       // chart.getSeriesByName("Имя2")->addXY(i*70, 30);
//    }

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


void MainWindow::on_pushButton_clicked()
{
    chart.clear();
    fun(ui->lineEdit->text().toLocal8Bit().data(), 0, /*105798*/1000);
}

