#include "coolchart.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <math.h>
#include <float.h>
#include <QSizePolicy>
#include <QMenu>
#include <QFile>
#include <QElapsedTimer>
#include <iostream>
#include <QPainterPath>
#include <QTextStream>

int Series::cnt = 0;

#define PredefColors_NUM 16
QColor PredefColors[PredefColors_NUM] = {
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::cyan,
    Qt::magenta,
    Qt::yellow,
    Qt::darkGray,
    Qt::gray,
    Qt::lightGray,
    Qt::darkRed,
    Qt::darkGreen,
    Qt::darkBlue,
    Qt::darkCyan,
    Qt::darkMagenta,
    Qt::darkYellow,
    Qt::white
};


Series::Series(CoolChart* parent)
{
    this->parent = parent;

    type = Line;
    xy.clear();

    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);

    pen.setColor(Qt::red);
    pen.setStyle(Qt::SolidLine);

    visible = true;

    id = cnt++;

    max_x = min_x = max_y = min_y = 0;

    avg_sum_y = 0;
    avg_n_y = 0;
    avg_y = 0;

    avg_vis_sum_y = 0;
    avg_vis_n_y = 0;
    avg_vis_y = 0;

    name = "Series" + QString::number(id);

    first_drawable_point_ind = -1;
    first_drawable_point_x = -1;
}

void Series::addXY(QPointF p)
{
    xy.append(p);

    avg_sum_y += p.y();
    avg_n_y++;
    avg_y = avg_sum_y / avg_n_y;

    if (xy.size() == 1) {
        max_x = p.x();
        min_x = p.x();
        max_y = p.y();
        min_y = p.y();
    }

    if (p.x() > max_x)
        max_x = p.x();
    if (p.x() < min_x)
        min_x = p.x();
    if (p.y() > max_y)
        max_y = p.y();
    if (p.y() < min_y)
        min_y = p.y();

    if (parent->getAutoXLimits()) {
        if (min_x < parent->getXMin()) parent->setXMin(min_x);
        if (max_x > parent->getXMax()) parent->setXMax(max_x);
    }
    if (parent->getAutoYLimits()) {
        if (min_y < parent->getYMin()) parent->setYMin(min_y);
        if (max_y > parent->getYMax()) parent->setYMax(max_y);
    }

    parent->update();
}

void Series::addXY(double x, double y)
{
    QPointF p(x, y);
    xy.append(p);

    avg_sum_y += p.y();
    avg_n_y++;
    avg_y = avg_sum_y / avg_n_y;

    if (xy.size() == 1) {
        max_x = p.x();
        min_x = p.x();
        max_y = p.y();
        min_y = p.y();
    }

    if (p.x() > max_x)
        max_x = p.x();
    if (p.x() < min_x)
        min_x = p.x();
    if (p.y() > max_y)
        max_y = p.y();
    if (p.y() < min_y)
        min_y = p.y();

    if (parent->getAutoXLimits()) {
        if (min_x < parent->getXMin()) parent->setXMin(min_x);
        if (max_x > parent->getXMax()) parent->setXMax(max_x);
    }
    if (parent->getAutoYLimits()) {
        if (min_y < parent->getYMin()) parent->setYMin(min_y);
        if (max_y > parent->getYMax()) parent->setYMax(max_y);
    }

    parent->update();
}

void Series::clear()
{
    avg_sum_y = 0;
    avg_n_y = 0;
    avg_y = 0;

    avg_vis_sum_y = 0;
    avg_vis_n_y = 0;
    avg_vis_y = 0;

    xy.clear();
    parent->update();
}

QList<QPointF>* Series::getXY()
{
    return &xy;
}

void Series::setType(SeriesType type)
{
    this->type = type;
    parent->update();
}

void Series::setBrush(QBrush brush)
{
    this->brush = brush;
    parent->update();
}

void Series::setPen(QPen pen)
{
    this->pen = pen;
    parent->update();
}

SeriesType Series::getType()
{
    return type;
}

QBrush Series::getBrush()
{
    return brush;
}

QPen Series::getPen()
{
    return pen;
}

void Series::setVisible(bool v)
{
    visible = v;
}

bool Series::getVisible()
{
    return visible;
}


CoolChart::CoolChart(QWidget *ob) : QOpenGLWidget(ob)
{
    antialiased = false;

    //Внешний квадрат
    outerRectPen.setWidth(1);
    outerRectPen.setColor(Qt::white);
    outerRectPen.setCapStyle(Qt::SquareCap);
    outerRectPen.setJoinStyle(Qt::MiterJoin);
    outerRectBrush.setColor(Qt::black);
    outerRectBrush.setStyle(Qt::SolidPattern);
    marginTop = 40, marginRight = 10, marginBottom = 40, marginLeft = 100;
    //marginTop = 0, marginRight = 0, marginBottom = 0, marginLeft = 0;

    //Сетка
    gridPen.setWidth(1);
    gridPen.setColor(Qt::white);
    gridPen.setCapStyle(Qt::SquareCap);
    gridPen.setJoinStyle(Qt::MiterJoin);
    gridPen.setStyle(Qt::DashLine);
    gridLineCountX = 5, gridLineCountY = 5;

    //Физические границы построения
    xMin = 0, xMax = 10, yMin = 0, yMax = 10;

    for (int i = 0; i < Font_NUM; i++) {
        this->textFont[i].setFamily("Consolas");
        this->textFont[i].setPointSize(10);
        this->textColor[i] = Qt::white;
    }


    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    setMouseTracking(true);
    series.clear();

    lmb_pressed = false;
    rmb_pressed = false;
    mmb_pressed = false;

    autoXLimit = true;
    autoYLimit = true;

    textX_fmt = 'f';
    textX_prec = 3;

    textY_fmt = 'f';
    textY_prec = 3;

    crossPen.setColor(Qt::red);
    crossPen.setStyle(Qt::DashLine);
    QVector<qreal> dashes;
    dashes << 10 << 7;
    crossPen.setDashPattern(dashes);
    crossPen.setWidth(1);

    draw_inf_enabled = false;

    setFocusPolicy(Qt::StrongFocus);

    zoom_rect_draw_enable = false;

    zoom_by_wheel_x = false;
    zoom_by_wheel_y = false;

    lw = new QListWidget(this);
    lw->setVisible(false);
    lw->setMaximumWidth(300);
    lw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(lw, &QListWidget::customContextMenuRequested, this, &CoolChart::showContextMenu);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    clrDlg = new QColorDialog(this);
    clrDlg->setWindowTitle("Series properties");
    connect(clrDlg, &QColorDialog::colorSelected, this, &CoolChart::colorSelected);

    l1 = new QHBoxLayout(clrDlg);
    l2 = new QHBoxLayout(clrDlg);
    l3 = new QHBoxLayout(clrDlg);

    lab1 = new QLabel("Name", clrDlg);
    lab2 = new QLabel("Width", clrDlg);
    lab3 = new QLabel("Type", clrDlg);

    lab1->setFixedWidth(80);
    lab2->setFixedWidth(80);
    lab3->setFixedWidth(80);

    edName = new QLineEdit(clrDlg);
    ed = new QSpinBox(clrDlg);
    cb = new QComboBox(clrDlg);
    cb->addItem("Line");
    cb->addItem("Circles");

    l1->addWidget(lab1);
    l1->addWidget(edName);

    l2->addWidget(lab2);
    l2->addWidget(ed);

    l3->addWidget(lab3);
    l3->addWidget(cb);

    clrDlg->layout()->addItem(l1);
    clrDlg->layout()->addItem(l2);
    clrDlg->layout()->addItem(l3);

    start_px_line_x = 0;
    start_px_line_y = 0;

    min_x_y_number = 0x7FFFFFFF;
}



//*********************************************************************
//------------------------public-Setters-------------------------------
//*********************************************************************

void CoolChart::setAntialiased(bool antialiased)
{
  this->antialiased = antialiased;
  update();
}

void CoolChart::setOuterRectPen(const QPen &pen)
{
    this->outerRectPen = pen;
    update();
}

void CoolChart::setOuterRectBrush(const QBrush &brush)
{
    this->outerRectBrush = brush;
    update();
}

void CoolChart::setGridPen(const QPen &pen)
{
    this->gridPen = pen;
    update();
}

void CoolChart::setGridLineCountX(int X)
{
    this->gridLineCountX = X;
    update();
}

void CoolChart::setGridLineCountY(int Y)
{
    this->gridLineCountY = Y;
    update();
}

void CoolChart::setTextFont(QFont font, QColor color, FontOfWhat what)
{
    this->textFont[what] = font;
    this->textColor[what] = color;
    update();
}

void CoolChart::setTextXFormat(char fmt)
{
    textX_fmt = fmt;
}

void CoolChart::setTextYFormat(char fmt)
{
    textY_fmt = fmt;
}

void CoolChart::setTextXPrecision(int precision)
{
    textX_prec = precision;
}

void CoolChart::setTextYPrecision(int precision)
{
    textY_prec = precision;
}

void CoolChart::setMarginTop(int top)
{
    marginTop = top;
    update();
}

void CoolChart::setMarginLeft(int left)
{
    marginLeft = left;
    update();
}

void CoolChart::setMarginRight(int right)
{
    marginRight = right;
    update();
}

void CoolChart::setMarginBottom(int bottom)
{
    marginBottom = bottom;
    update();
}

void CoolChart::setMargins(int top, int left, int right, int bottom)
{
    marginTop = top;
    marginLeft = left;
    marginRight = right;
    marginBottom = bottom;
    update();
}

void CoolChart::setXMin(double xMin)
{
    this->xMin = xMin;
    update();
}

void CoolChart::CoolChart::setXMax(double xMax)
{
    this->xMax = xMax;
    update();
}

void CoolChart::setYMin(double yMin)
{
    this->yMin = yMin;
    update();
}

void CoolChart::setYMax(double yMax)
{
    this->yMax = yMax;
    update();
}

void CoolChart::setLimits(double xMin, double xMax, double yMin, double yMax)
{
    this->xMin = xMin;
    this->xMax = xMax;
    this->yMin = yMin;
    this->yMax = yMax;
    update();
}

void CoolChart::setAutoXLimits(bool autoX)
{
    autoXLimit = autoX;
}

void CoolChart::setAutoYLimits(bool autoY)
{
    autoYLimit = autoY;
}

void CoolChart::setCrossPen(QPen p)
{
    crossPen = p;
}

void CoolChart::setTitle(QString tit)
{
    title = tit;
}

void CoolChart::setXTitle(QString tit)
{
    xTitle = tit;
}

void CoolChart::setYTitle(QString tit)
{
    yTitle = tit;
}


//*********************************************************************
//------------------------public-Getters-------------------------------
//*********************************************************************

bool CoolChart::getAntialiased()
{
    return antialiased;
}

QPen CoolChart::getOuterRectPen()
{
    return outerRectPen;
}

QBrush CoolChart::getOuterRectBrush()
{
    return outerRectBrush;
}

QPen CoolChart::getGridPen()
{
    return gridPen;
}

int CoolChart::getGridLineCountX()
{
    return gridLineCountX;
}

int CoolChart::getGridLineCountY()
{
    return gridLineCountY;
}

QFont CoolChart::getTextFont(FontOfWhat what)
{
    return textFont[what];
}

QColor CoolChart::getTextColor(FontOfWhat what)
{
    return textColor[what];
}

char CoolChart::getTextXFormat()
{
    return textX_fmt;
}

char CoolChart::getTextYFormat()
{
    return textY_fmt;
}

int CoolChart::getTextXPrecision()
{
    return textX_prec;
}

int CoolChart::getTextYPrecision()
{
    return textY_prec;
}

int CoolChart::getMarginTop()
{
    return marginTop;
}

int CoolChart::getMarginLeft()
{
    return marginLeft;
}

int CoolChart::getMarginRight()
{
    return marginRight;
}

int CoolChart::getMarginBottom()
{
    return marginBottom;
}

double CoolChart::getXMin()
{
    return xMin;
}

double CoolChart::getXMax()
{
    return xMax;
}

double CoolChart::getYMin()
{
    return yMin;
}

double CoolChart::getYMax()
{
    return yMax;
}

bool CoolChart::getAutoXLimits()
{
    return autoXLimit;
}

bool CoolChart::getAutoYLimits()
{
    return autoYLimit;
}

QPen CoolChart::getCrossPen()
{
    return crossPen;
}

QList<Series> *CoolChart::getSeries()
{
    return &series;
}

//*********************************************************************
//------------------------public-Functions-----------------------------
//*********************************************************************

int CoolChart::addSeries(Series s)
{
    this->series.append(s);
    lw->addItem(s.getName());
    lw->item(lw->count()-1)->setForeground(s.getPen().color());
    return s.getID();
}

Series* CoolChart::getSeriesByID(int id)
{
    for (int i = 0; i < series.size(); i++) {
        if (series[i].getID() == id) return &(series[i]);
    }
    return nullptr;
}

void CoolChart::deleteSeriesById(int id)
{
    for (int i = 0; i < series.size(); i++) {
        if (series[i].getID() == id) {
            series.removeAt(i);
            lw->takeItem(i);
            break;
        }
    }
    update();
}

void CoolChart::clear()
{
    series.clear();
    if (autoXLimit) {
        xMin = 0, xMax = 10;
    }
    if (autoYLimit) {
        yMin = 0, yMax = 10;
    }
    update();
}





//*********************************************************************
//------------------------private-Functions----------------------------
//*********************************************************************

bool CoolChart::doesPhisycalLineBelongToChart(QLineF l)
{
    double x1 = l.p1().x();
    double y1 = l.p1().y();
    double x2 = l.p2().x();
    double y2 = l.p2().y();

    if (
        (do_lines_cross(x1, y1, x2, y2,  xMin, yMax, xMax, yMax) ||
        do_lines_cross(x1, y1, x2, y2,   xMax, yMax, xMax, yMin) ||
        do_lines_cross(x1, y1, x2, y2,   xMax, yMin, xMin, yMin) ||
        do_lines_cross(x1, y1, x2, y2,   xMin, yMin, xMin, yMax) ||
        ((x1 >= xMin) && (x1 <= xMax) && (y1 >= yMin) && (y1 <= yMax)) ||
        ((x2 >= xMin) && (x2 <= xMax) && (y2 >= yMin) && (y2 <= yMax))
        )
       )
       return true;
    else
       return false;
}

bool CoolChart::doesPhisycalPointBelongToChart(QPointF p)
{
    if (p.x() > xMin && p.x() < xMax && p.y() > yMin && p.y() < yMax) {
        return true;
    }
    return false;
}

int CoolChart::calcPixDist(QLine l)
{
    int d = (int)(sqrt( pow(l.dx(),2) + pow(l.dy(),2) ));
    return d;
}

QPoint  CoolChart::phisycalPointToPix(QPointF point)
{
    QPoint res;

    double x = (point.x() - xMin) / (( (xMax - xMin) / (w_f) ));
    double y = (point.y() - yMin) / (( (yMax - yMin) / (h_f) ));

    res.setX(qRound(x));
    res.setY(qRound(y));

    return res;
}

QPointF CoolChart::pixPointToPhisycal(QPoint point)
{
    QPointF res;

    double x = (point.x() * ( (xMax - xMin) / (w_f) ))  +  (xMin);
    double y = (point.y() * ( (yMax - yMin) / (h_f) ))  +  (yMin);

    res.setX(x);
    res.setY(y);

    return res;
}

QPointF CoolChart::findNearestPointByX(Series& s, double x)
{
    double mindx = DBL_MAX;
    QPointF res;
    for (int i = 0; i < s.getXY()->size(); i++) {
        QPointF p(s.getXY()->operator[](i));
        double dx = fabs(p.x() - x);
        if (dx < mindx) {
            mindx = dx;
            res = p;
        }
    }
    return res;
}

void CoolChart::zoomByRect(QRect r)
{
    int x1, x2, y1, y2;
    r.getCoords(&x1, &y1, &x2, &y2) ;

    QPoint p1(x1,y1); QPoint p2(x2,y2);

    QPointF f1 = pixPointToPhisycal(p1);
    QPointF f2 = pixPointToPhisycal(p2);

    xMin = f1.x();
    xMax = f2.x();

    //yMin = -f2.y();
    //yMax = -f1.y();

    yMin = f2.y();
    yMax = f1.y();
}

QPixmap CoolChart::grabScreenshot()
{
    QRect wr = this->rect();
    QPoint r1 = mapToGlobal(QPoint(wr.x(), wr.y()));
    //QPoint r2 = mapToGlobal(QPoint(wr.x() + wr.width(), wr.y() + wr.height()));

    QPoint t1 (r1 + QPoint(x_f, y_f) );
    QPoint t2 (r1 + QPoint(x_f+w_f-1, y_f+h_f-1) );
    QRect screenRect(t1,t2);

    QPixmap desktopPixmap = QPixmap(screenRect.size());
    QPainter p(&desktopPixmap);

    for (QScreen* screen : QApplication::screens())
        p.drawPixmap(screen->geometry().topLeft(), screen->grabWindow(0, t1.x(), t1.y(), screenRect.width(), screenRect.height()));

    return desktopPixmap;
}






//*********************************************************************
//--------------------drawing-private-Functions------------------------
//*********************************************************************

void CoolChart::drawChartRectangle(QPainter& p)
{
    //Рисуем квадрат и заливаем его
    p.setPen(outerRectPen);
    p.setBrush(outerRectBrush);
    p.drawRect(x_l, y_l, w_l, h_l);
    p.fillRect(x_f, y_f, w_f, h_f, outerRectBrush);
}

void CoolChart::drawChartGridAndNumbers(QPainter& p)
{
    drawXNumber(p, 0);
    int px_w_sector = w_f / (gridLineCountX+1);
    int w = 0;
    for (int i = 0; i < gridLineCountX+1; i++, w += px_w_sector) {
        int x = start_px_line_x + px_w_sector * i;
        p.setPen(gridPen);
        p.drawLine(QLine(x, 0, x, h_f));
        drawXNumber(p, x);
    }
    drawXNumber(p, w_f);

    drawYNumber(p, 0);
    int px_h_sector = h_f / (gridLineCountY+1);
    int h = 0;
    for (int i = 0; i < gridLineCountY+1; i++, h+= px_h_sector) {
        int y = start_px_line_y + px_h_sector * i;
        p.setPen(gridPen);
        p.drawLine(QLine(0, y, w_f, y));
        drawYNumber(p, y);
    }
    drawYNumber(p, h_f);
}

void CoolChart::drawAllSeries(QPainter& p)
{
    for (int i = 0; i < series.size(); i++) {
        drawSeries(i, p);
    }
}

void CoolChart::drawSeries(int i, QPainter& p)
{
    if (!series[i].getVisible()) return;
    if (series[i].getType() == Line) {
        drawLineSeries(i, p);
    }
    else if (series[i].getType() == Circles) {
        drawCircleSeries(i, p);
    }
}

void CoolChart::drawLineSeries(int i, QPainter& p)
{
    QLine l;
    if (series[i].getXY()->size() > 1) {
        series[i].getXYPix()->clear();
        series[i].avg_vis_sum_y = 0;
        series[i].avg_vis_n_y = 0;
        series[i].avg_vis_y = 0;

        bool pr = false;

        int si = 0;
        if (series[i].first_drawable_point_ind != -1) {
            if (xMin >= series[i].first_drawable_point_x) {
                si = series[i].first_drawable_point_ind;
            }
            else if (series[i].first_drawable_point_ind >= 1){
                si = series[i].first_drawable_point_ind;
                do {
                    //series[i].first_drawable_point_ind--;
                    //series[i].first_drawable_point_x = series[i].first_drawable_point_ind;
                    si--;// = series[i].first_drawable_point_ind;
                }
                //while (si > 1 && series[i].getXY()->operator[](si).x()/*first_drawable_point_x*/ > xMin);
                while(si >= 1 && series[i].getXY()->operator[](si).x() > xMin);
            }
            else {
                si = 0;
                series[i].first_drawable_point_ind = -1;
                series[i].first_drawable_point_x = -1;
            }
        }

        p.setPen(series[i].getPen());
        p.setBrush(QBrush(Qt::transparent));

        QPainterPath pl;

        for (int j = si; j < series[i].getXY()->size() - 1; j++) {

            QLineF ph_l(series[i].getXY()->operator[](j), series[i].getXY()->operator[](j+1));
            if (series[i].getXY()->operator[](j).x() > xMax) break;
            if (doesPhisycalLineBelongToChart(ph_l)) {

            //if (/*doesPhisycalPointBelongToChart(ph_l.p1()) || doesPhisycalPointBelongToChart(ph_l.p2())*/1) {

                QPoint p1 = phisycalPointToPix(series[i].getXY()->operator[](j));
                QPoint p2 = phisycalPointToPix(series[i].getXY()->operator[](j+1));
                l.setP1( p1 );
                l.setP2( p2 );

                if (!pr) {
                    series[i].first_drawable_point_ind = j;
                    series[i].first_drawable_point_x = series[i].getXY()->operator[](j).x();
                    pr = true;
                    pl.moveTo(p1);
                }



                if (calcPixDist(l) >= 1) {
                    //p.drawLine(l);
                    pl.moveTo(p1);
                    pl.lineTo(p2);
                    if (series[i].getXYPix()->size() == 0) {
                        if (doesPhisycalPointBelongToChart(pixPointToPhisycal(p1)))
                        {
                            series[i].getXYPix()->append(p1);
                            series[i].avg_vis_sum_y += series[i].getXY()->operator[](j).y();
                            series[i].avg_vis_n_y++;
                            series[i].avg_vis_y = series[i].avg_vis_sum_y / series[i].avg_vis_n_y;
                        }
                        if (doesPhisycalPointBelongToChart(pixPointToPhisycal(p2)))
                        {
                            series[i].getXYPix()->append(p2);
                            series[i].avg_vis_sum_y += series[i].getXY()->operator[](j+1).y();
                            series[i].avg_vis_n_y++;
                            series[i].avg_vis_y = series[i].avg_vis_sum_y / series[i].avg_vis_n_y;
                        }
                    }
                    else
                        if (doesPhisycalPointBelongToChart(pixPointToPhisycal(p2)))
                        {
                            series[i].getXYPix()->append(p2);
                            series[i].avg_vis_sum_y += series[i].getXY()->operator[](j+1).y();
                            series[i].avg_vis_n_y++;
                            series[i].avg_vis_y = series[i].avg_vis_sum_y / series[i].avg_vis_n_y;
                        }
                }
            }
        }
        p.drawPath(pl);
    }
    else if (series[i].getXY()->size() == 1) {
        QPoint p1 = phisycalPointToPix(series[i].getXY()->operator[](0));
        p.drawEllipse(p1, series[i].getPen().width()/2, series[i].getPen().width()/2);
    }
}

void CoolChart::drawCircleSeries(int i, QPainter& p)
{
    QPointF ph_p;
    QPoint p1;
    p.setPen(series[i].getPen());
    for (int j = 0; j < series[i].getXY()->size(); j++) {
        ph_p = series[i].getXY()->operator[](j);
        if (doesPhisycalPointBelongToChart(ph_p)) {
            p1 = phisycalPointToPix(ph_p);
            p.drawEllipse(p1, series[i].getPen().width()/2, series[i].getPen().width()/2);
        }
    }
}

void CoolChart::drawXNumber(QPainter& painter, int x)
{
    //Подписи оси X
    painter.setFont(textFont[FAxisXNumbers]);
    painter.setPen(textColor[FAxisXNumbers]);
    QPointF xy = pixPointToPhisycal(QPoint(x, 0));
    QString s = QString::number( xy.x(), textX_fmt, textX_prec );
    QFontMetrics fm(textFont[FAxisXNumbers]);
    int fontheight = fm.height();
    int fontwidth = fm.horizontalAdvance(s);
    QPoint p_txt(x - fontwidth / 2, 0 - outerRectPen.width() + fontheight + 4);
    painter.scale(1, -1);
    painter.drawText(p_txt, s);
    painter.scale(1, -1);
}

void CoolChart::drawYNumber(QPainter& painter, int y)
{
    //Подписи оси Y
    painter.setFont(textFont[FAxisYNumbers]);
    painter.setPen(textColor[FAxisYNumbers]);
    QPointF xy = pixPointToPhisycal(QPoint(0, y));
    QString s = QString::number( xy.y(), textY_fmt, textY_prec );
    QFontMetrics fm(textFont[FAxisYNumbers]);
    int fontheight = fm.height();
    int fontwidth = fm.horizontalAdvance(s);
    QPoint p_txt( -(fontwidth + outerRectPen.width() + 4), -(y - fontheight/2 + 3));
    if (p_txt.x() < min_x_y_number) min_x_y_number = p_txt.x();
    painter.scale(1, -1);
    painter.drawText(p_txt, s);
    painter.scale(1, -1);
}

void CoolChart::drawTitle(QPainter& painter)
{
    painter.setFont(textFont[FTitle]);
    painter.setPen(textColor[FTitle]);
    QFontMetrics fm(textFont[FTitle]);
    painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, title);
}

void CoolChart::drawAxisTitle(QPainter& painter)
{
    painter.setFont(textFont[FAxisXTitle]);
    painter.setPen(textColor[FAxisXTitle]);
    QFontMetrics fm(textFont[FAxisXTitle]);
    QFontMetrics fm2(textFont[FAxisXNumbers]);
    QRect rr(w_f / 2 - fm.horizontalAdvance(xTitle) / 2, fm2.height() + outerRectPen.width() + 4, fm.horizontalAdvance(xTitle), fm.height());
    painter.scale(1, -1);
    painter.drawText(rr, Qt::AlignHCenter | Qt::AlignVCenter, xTitle);
    painter.scale(1, -1);


    painter.setFont(textFont[FAxisYTitle]);
    painter.setPen(textColor[FAxisYTitle]);
    QFontMetrics fm1(textFont[FAxisYTitle]);
    QRect rr1(min_x_y_number - fm1.horizontalAdvance(yTitle) / 2 - 5, -((h_f / 2) + fm1.height()/2), fm1.horizontalAdvance(yTitle), fm1.height());
    painter.scale(1, -1);
    painter.save();
    painter.translate(rr1.center());
    painter.rotate(-90);
    painter.translate(-rr1.center());
    painter.drawText(rr1, yTitle);
    painter.rotate(90);
    painter.restore();
    painter.scale(1, -1);
}

//*********************************************************************
//-------------------------------Events--------------------------------
//*********************************************************************

void CoolChart::paintEvent(QPaintEvent * /* event */)
{
    QElapsedTimer timer;
    timer.start();

    QPainter Painter(this);

    if (antialiased) {
        Painter.setRenderHint(QPainter::Antialiasing, true);
    }

    if (hasFocus()) {
        QPen pen = outerRectPen;
        QColor cl = palette().color(QPalette::Active, QPalette::Highlight);
        pen.setColor(cl);
        Painter.setPen(pen);
        Painter.setBrush(Qt::transparent);
        QRect r = this->rect();
        r.setRight(r.right()-pen.width());
        r.setBottom(r.bottom()-pen.width());
        r.setLeft(r.top()+pen.width());
        //Painter.drawRect(r);
        Painter.drawRoundedRect(r, 2, 2);
    }

    //Координаты линий внешнего квадрата
    x_l = marginLeft+outerRectPen.width()/2;
    y_l = marginTop+outerRectPen.width()/2;
    w_l = this->width() - marginLeft - marginRight - outerRectPen.width();
    h_l = this->height() - marginTop - marginBottom - outerRectPen.width();

    //Координаты квадрата заполнения
    x_f = marginLeft+outerRectPen.width();
    y_f = marginTop+outerRectPen.width();
    w_f = this->width() - marginLeft - marginRight - outerRectPen.width()-outerRectPen.width()-1;
    h_f = this->height() - marginTop - marginBottom - outerRectPen.width()-outerRectPen.width()-1;

    drawChartRectangle(Painter);
    drawTitle(Painter);


    Painter.translate(x_f , y_f + h_f);
    Painter.scale(1, -1);

    //Начинаем рисовать
    drawChartGridAndNumbers(Painter);

    drawAxisTitle(Painter);

    Painter.setClipRect(0, 0, w_f, h_f);

    //if (!lmb_pressed && !mmb_pressed)
        drawAllSeries(Painter);

    if (zoom_rect_draw_enable) {
        //Painter.scale(1,-1);
        //Painter.drawPixmap(0, -h_f, w_f, h_f, img, 0, 0, img.width(), img.height());
        //Painter.scale(1,-1);

        QPoint p1 = QPoint(zoom_rect.x(), (zoom_rect.y()));
        QPoint p2 = QPoint(zoom_rect.x() + zoom_rect.width(), (zoom_rect.y() + zoom_rect.height()));
        QRect r(p1,p2);
        QPen p(Qt::gray);
        p.setStyle(Qt::SolidLine);
        p.setWidth(2);
        Painter.setPen(p);
        Painter.setBrush(Qt::transparent);
        Painter.drawRect(r);
    }

    if (mmb_pressed) {
        //Painter.scale(1,-1);
        //Painter.drawPixmap(0, -h_f, w_f, h_f, img, 0, 0, img.width(), img.height());
        //Painter.scale(1,-1);

        QLine lx(QPoint(crossLineX, 0), QPoint(crossLineX, h_f));
        QLine ly(QPoint(0, crossLineY), QPoint(w_f, crossLineY));

        Painter.setPen(crossPen);

        QBrush br(Qt::transparent);
        Painter.setBrush(br);

        Painter.drawLine(lx);
        Painter.drawLine(ly);
        Painter.scale(1,-1);
        QPoint pnt(crossLineX, crossLineY);
        QPointF pntf = pixPointToPhisycal(pnt);

        QString s = "(" + QString::number(pntf.x()) + "; " + QString::number(pntf.y()) + ")";

        Painter.drawText(crossLineX+15, -crossLineY+20, s);
        Painter.scale(1,-1);

        QPen p(crossPen.color());
        p.setStyle(Qt::SolidLine);

        for (int i = 0; i < series.size(); i++) {
            QPointF nr = findNearestPointByX(series[i], pntf.x());
            QPoint pp = phisycalPointToPix(nr);
            p.setColor(series[i].getPen().color());
            p.setWidth(2); //series[i].getPen().width()
            Painter.setPen(p);
            Painter.drawEllipse(pp, series[i].getPen().width()*2, series[i].getPen().width()*2);

            QString s = "(" + QString::number(nr.x()) + "; " + QString::number(nr.y()) + ")";

            QFontMetrics fm(textFont[FAxisXNumbers]);
            int fontheight = fm.height();
            int fontwidth = fm.horizontalAdvance(s);

            int r, g, b;
            p.color().getRgb(&r, &g, &b);

            int intclr = (r<<16) | (g <<8) | b;

            Painter.scale(1,-1);
            Painter.fillRect(pp.x()+15, -pp.y()+20-(fontheight-(fontheight/3)), fontwidth, fontheight, 0xFFFFFF - intclr);
            Painter.drawText(pp.x()+15, -pp.y()+20, s);
            Painter.scale(1,-1);
        }
    }

    if (draw_inf_enabled)
        DrawInf(Painter);

    std::cout << "The slow operation took " << timer.elapsed() << " milliseconds" << std::endl;
}

void CoolChart::mouseMoveEvent(QMouseEvent *event)
{
    if (rmb_pressed) {
        QPointF tek_p_f = pixPointToPhisycal(event->pos());
        QPointF dp = tek_p_f - rmb_pr_p_f;

        this->xMin -= dp.x();
        this->xMax -= dp.x();

        this->yMin += dp.y();
        this->yMax += dp.y();

        rmb_pr_p_f = pixPointToPhisycal(event->pos());



        int dx = -(rmb_pr_p_p.x() - event->pos().x());
        int ww = w_f / (gridLineCountX+1);
        start_px_line_x += dx;
        if (start_px_line_x > ww) start_px_line_x = start_px_line_x - ww;
        else if (start_px_line_x < 0) start_px_line_x = ww + start_px_line_x;

        int dy = (rmb_pr_p_p.y() - event->pos().y());
        int hh = h_f / (gridLineCountY+1);
        start_px_line_y += dy;
        if (start_px_line_y > hh) start_px_line_y = start_px_line_y - hh;
        else if (start_px_line_y < 0) start_px_line_y = hh + start_px_line_y;

        rmb_pr_p_p = event->pos();

        update();


    }
    if (lmb_pressed) {
        zoom_rect_draw_enable = true;
        zoom_rect = QRect(QPoint(zoom_rect.x(), zoom_rect.y()),
                          QPoint(event->pos().x() - x_f, h_f - (event->pos().y() - y_f)) );
        update();
    }
    if (mmb_pressed) {
        crossLineX = event->pos().x() - x_f;
        crossLineY = h_f - event->pos().y() + y_f;
        update();
    }
}

void CoolChart::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        lmb_pressed = true;
        zoom_rect.setX(event->pos().x() - x_f);
        zoom_rect.setY(h_f - (event->pos().y() - y_f));
        img = grabScreenshot();
    }
    else if (event->button() == Qt::RightButton) {
        rmb_pressed = true;
        rmb_pr_p_f = pixPointToPhisycal(event->pos());
        rmb_pr_p_p = event->pos();
    }
    else if (event->button() == Qt::MiddleButton) {
        mmb_pressed = true;
        img = grabScreenshot();
    }
}

void CoolChart::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && zoom_rect_draw_enable){
        int x1, x2, y1, y2;
        zoom_rect.getCoords(&x1, &y1, &x2, &y2) ;

        if (x1 > x2) {
            if (!zoom_stack.isEmpty()) {
                QRectF r = zoom_stack.pop();
                double x1, x2, y1, y2;
                r.getCoords(&x1, &x2, &y1, &y2) ;
                setAutoXLimits(true);
                setAutoYLimits(true);
                setLimits(x1,x2,y1,y2);
                update();
            }
            else {
                setAutoXLimits(true);
                setAutoYLimits(true);

                double xmin = DBL_MAX, ymin = DBL_MAX;
                double xmax = DBL_MIN, ymax = DBL_MIN;
                for (int i = 0; i < series.size(); i++) {
                    for (int j = 0; j < series[i].getXY()->size(); j++) {
                        double x = series[i].getXY()->operator[](j).x();
                        double y = series[i].getXY()->operator[](j).y();
                        if (x < xmin) xmin = x;
                        if (x > xmax) xmax = x;
                        if (y < ymin) ymin = y;
                        if (y > ymax) ymax = y;
                    }
                }

                setXMin(xmin);
                setXMax(xmax);

                setYMin(ymin);
                setYMax(ymax);

                update();
            }
        }
        else if (x1 < x2) {
            QRectF r(QPointF(xMin, xMax), QPointF(yMin, yMax));
            zoom_stack.push(r);
            setAutoXLimits(false);
            setAutoYLimits(false);
            zoomByRect(zoom_rect);
            update();
        }
    }
    else if (event->button() == Qt::RightButton) {
        rmb_pressed = false;
    }
    else if (event->button() == Qt::MiddleButton) {
        mmb_pressed = false;
        update();
    }

    zoom_rect_draw_enable = false;
    lmb_pressed = false;
}

void CoolChart::DrawInf(QPainter& p)
{
    QPen pn;
    pn.setWidth(1);
    pn.setCapStyle(Qt::SquareCap);
    pn.setJoinStyle(Qt::MiterJoin);

    QFont font = p.font();
    font.setPixelSize(10);
    p.setFont(font);

    for (int i = 0; i < series.size(); i++) {
            double avg = series[i].getAvgY();
            double avg_vis = series[i].avg_vis_y;
            p.scale(1, -1);
            QPoint p_txt(5, (-h_f+10) + (font.pixelSize()*i));
            pn.setColor(series[i].getPen().color());
            p.setPen(pn);
            p.drawText(p_txt, series[i].getName() + ": " + "avg: " + QString::number(avg, 'f', 3) + "; avg_vis: " + QString::number(avg_vis, 'f', 3));
            p.scale(1, -1);
        }
}

void CoolChart::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space)
    {
        draw_inf_enabled = !draw_inf_enabled;
        update();
    }

    if (event->key() == Qt::Key_Control) {
        this->zoom_by_wheel_x = true;
        this->zoom_by_wheel_y = false;
    }

    if (event->key() == Qt::Key_Shift) {
       this->zoom_by_wheel_x = false;
       this->zoom_by_wheel_y = true;
    }

    if (event->key() == Qt::Key_Z) {
       this->smooth_scale = true;
    }
}

void CoolChart::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control) {
        this->zoom_by_wheel_x = false;
    }
    if (event->key() == Qt::Key_Shift) {
        this->zoom_by_wheel_y = false;
    }
    if (event->key() == Qt::Key_Z) {
       this->smooth_scale = false;
    }
}

bool CoolChart::do_lines_cross(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
double Ua, Ub, numerator_a, numerator_b, denominator;
    denominator=(y4-y3)*(x1-x2)-(x4-x3)*(y1-y2);
    if (denominator == 0){
        if ( (x1*y2-x2*y1)*(x4-x3) - (x3*y4-x4*y3)*(x2-x1) == 0 && (x1*y2-x2*y1)*(y4-y3) - (x3*y4-x4*y3)*(y2-y1) == 0)
            return true;
        else return false;
    }
    else{
        numerator_a=(x4-x2)*(y4-y3)-(x4-x3)*(y4-y2);
        numerator_b=(x1-x2)*(y4-y2)-(x4-x2)*(y1-y2);
        Ua=numerator_a/denominator;
        Ub=numerator_b/denominator;
        return  (Ua >=0 && Ua <=1 && Ub >=0 && Ub <=1 ? true : false);
    }
}

void CoolChart::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->angleDelta().y();
    int x = event->position().x() - this->x_f;
    int y = this->h_f - (event->position().y() - this->y_f);
    this->setAutoXLimits(false);
    this->setAutoYLimits(false);
    QPointF ph_p = this->pixPointToPhisycal(QPoint(x, y));

    double ww = this->xMax - this->xMin;
    double hh = this->yMax - this->yMin;

    double scale_factor = 0;

    if (!smooth_scale) {
        scale_factor = numDegrees > 0 ? 0.9 : 1.1;
    }
    else {
        scale_factor = numDegrees > 0 ? 0.99 : 1.01;
    }

    if (this->zoom_by_wheel_x && !this->zoom_by_wheel_y) {
        this->xMin = ph_p.x() - (ww / 2. * scale_factor);
        this->xMax = ph_p.x() + (ww / 2. * scale_factor);
    }
    else if (this->zoom_by_wheel_y and not this->zoom_by_wheel_x) {
        this->yMin = ph_p.y() - (hh / 2. * scale_factor);
        this->yMax = ph_p.y() + (hh / 2. * scale_factor);
    }
    else if (!this->zoom_by_wheel_y and !this->zoom_by_wheel_x) {
        this->xMin = ph_p.x() - (ww / 2. * scale_factor);
        this->xMax = ph_p.x() + (ww / 2. * scale_factor);
        this->yMin = ph_p.y() - (hh / 2. * scale_factor);
        this->yMax = ph_p.y() + (hh / 2. * scale_factor);
    }



    int p_c_x = this->x_f + (this->w_f / 2.);
    int p_c_y = this->y_f + (this->h_f / 2.);
    QPoint glob_p = this->mapToGlobal(QPoint(p_c_x, p_c_y));
    QCursor::setPos(glob_p);
    this->update();
}

QSize CoolChart::sizeHint() const
{
    return this->size();
}

void CoolChart::showLegend(QLayout*lay, bool b)
{
    lw->setVisible(b);
    lay->addWidget(lw);
}

QListWidget* CoolChart::getLegend()
{
    return lw;
}

void CoolChart::showContextMenu(const QPoint &pos)
{
    selectedItem = lw->itemAt(pos);
    if (selectedItem == nullptr) return;

    for (int i = 0; i < lw->count(); i++) {
        if (lw->item(i) == selectedItem) {
            selectedInd = i;
            break;
        }
    }
    QPoint globalPos = lw->mapToGlobal(pos);
    QMenu myMenu;
    myMenu.addAction("Удалить",  this, &CoolChart::deleteSeies);
    myMenu.addAction("Свойства",  this, &CoolChart::openColorDialog);
    myMenu.exec(globalPos);
}

void CoolChart::deleteSeies()
{
    deleteSeriesById(series[selectedInd].getID());
}

void CoolChart::openColorDialog()
{
    clrDlg->setCurrentColor(series[selectedInd].getPen().color());
    edName->setText(series[selectedInd].getName());
    ed->setValue(series[selectedInd].getPen().width());
    cb->setCurrentIndex(series[selectedInd].getType() == Line ? 0 : 1);
    clrDlg->show();
}

void CoolChart::colorSelected(const QColor &color)
{
    QPen p = series[selectedInd].getPen();
    p.setColor(color);
    p.setWidth(ed->value());
    series[selectedInd].setType(cb->currentIndex() == 0 ? Line : Circles);
    lw->item(selectedInd)->setText(edName->text());
    series[selectedInd].setName(edName->text());
    series[selectedInd].setPen(p);
    getSelectedSeriesItem()->setForeground(color);
}

QListWidgetItem* CoolChart::getSelectedSeriesItem()
{
    return selectedItem;
}

int CoolChart::plotByFile(QString fn, bool firstRowIsTitle, bool firstColumnIsX)
{
    QFile F(fn);
    if (!F.open(QIODevice::ReadOnly)) return 1;

    QTextStream ts(&F);

    QList<Series> s_list;

    srand(time(0));
    if (firstRowIsTitle) {
        QString title = ts.readLine();
        QStringList splitTitle = title.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        int start = 0;
        if (firstColumnIsX) start = 1;
        for (int i = start; i < splitTitle.length(); ++i) {
            Series s(this);
            s.setName(splitTitle[i]);
            QPen pen = s.getPen();
            if (i < PredefColors_NUM) {
                pen.setColor(PredefColors[i]);
            }
            else {
                pen.setColor(QColor(rand()%255, rand()%255, rand()%255));
            }
            s.setPen(pen);
            s_list.append(s);
        }
    }
    else {
        QString title = ts.readLine();
        QStringList splitTitle = title.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        int start = 0;
        if (firstColumnIsX) start = 1;
        for (int i = start; i < splitTitle.length(); ++i) {
            Series s(this);
            s.setName("Series " + QString::number(i));
            QPen pen = s.getPen();
            if (i < PredefColors_NUM) {
                pen.setColor(PredefColors[i]);
            }
            else {
                pen.setColor(QColor(rand()%255, rand()%255, rand()%255));
            }
            s.setPen(pen);
            s_list.append(s);
        }
        ts.seek(0);
    }

    unsigned int cnt = 0;
    while(!ts.atEnd()) {
        QString row = ts.readLine();
        QStringList splitRow = row.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        int start = 0, len = s_list.length();
        if (firstColumnIsX) {start = 1; ++len;}
        for (int i = start; i < splitRow.length() && i < len; ++i) {
            bool ok = false;
            bool ok2 = true;
            double val = QStringToNumber(splitRow[i], &ok);
            double X = 0;
            if (firstColumnIsX) {
                X = QStringToNumber(splitRow[0], &ok2);
            }
            else {
                X = cnt;
            }
            if (ok && ok2) {
                int ind = firstColumnIsX ? i-1 : i;
                s_list[ind].addXY(X, val);
            }
        }
        cnt++;
    }

    for (int i = 0; i < s_list.length(); i++) {
        this->addSeries(s_list[i]);
    }

    return 0;
}

double CoolChart::QStringToNumber(QString s, bool* ok)
{
    s = s.trimmed();
    double val = 0;
    if (s.contains(',')) {
        s = s.replace(',', '.');
    }

    if (s.contains('.')) {
        val = s.toDouble(ok);
        return val;
    }


    if (s.contains("0x")) {
        val = s.toInt(ok, 16);
        return val;
    }
    else {
        val = s.toInt(ok, 10);
        return val;
    }
}
