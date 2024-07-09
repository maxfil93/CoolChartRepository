#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "coolchart.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void onTimer1();

private:
    Ui::MainWindow *ui;

    CoolChart chart[4];

    void LineSeriesTest(int chart_num);
    void GanttSeriesTest(int chart_num);
    void GenerateRandom(CoolChart* ch, unsigned int ser_num, unsigned int points_num, SeriesType type);

    void fun(int chart_num, char* fn, unsigned int start_str, unsigned int num_str);

    QTimer* timer1;
};
#endif // MAINWINDOW_H
