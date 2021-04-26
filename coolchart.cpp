#include "coolchart.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <math.h>
#include <float.h>

int Series::cnt = 0;

Series::Series(CoolChart* parent)
{
    this->parent = parent;

    type = Line;
    xy.clear();

    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);

    pen.setColor(Qt::red);
    pen.setStyle(Qt::SolidLine);

    id = cnt++;

    max_x = min_x = max_y = min_y = 0;
}

void Series::addXY(QPointF p)
{
    xy.append(p);

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



CoolChart::CoolChart(QWidget *ob) : QWidget(ob)
{
    antialiased = false;

    //Внешний квадрат
    outerRectPen.setWidth(1);
    outerRectPen.setColor(Qt::white);
    outerRectPen.setCapStyle(Qt::SquareCap);
    outerRectPen.setJoinStyle(Qt::MiterJoin);
    outerRectBrush.setColor(Qt::black);
    outerRectBrush.setStyle(Qt::SolidPattern);
    marginTop = 20, marginRight = 10, marginBottom = 20, marginLeft = 100;
    //marginTop = 0, marginRight = 0, marginBottom = 0, marginLeft = 0;

    //Сетка
    gridPen.setWidth(1);
    gridPen.setColor(Qt::white);
    gridPen.setCapStyle(Qt::SquareCap);
    gridPen.setJoinStyle(Qt::MiterJoin);
    gridPen.setStyle(Qt::DashLine);
    gridLineCountX = 5, gridLineCountY = 5;

    //Физические границы построения
    xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX, yMax = -DBL_MAX;

    textFont.setFamily("Consolas");
    textFont.setPointSize(10);
    textColor = Qt::white;

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

void CoolChart::setTextFont(QFont font)
{
    this->textFont = font;
    update();
}

void CoolChart::setTextColor(QColor cl)
{
    textColor = cl;
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

QFont CoolChart::getTextFont()
{
    return textFont;
}

QColor CoolChart::getTextColor()
{
    return textColor;
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





//*********************************************************************
//------------------------public-Functions-----------------------------
//*********************************************************************

int CoolChart::addSeries(Series s)
{
    this->series.append(s);
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
        }
    }
    update();
}

void CoolChart::clear()
{
    series.clear();
    if (autoXLimit) {
        xMin = DBL_MAX, xMax = -DBL_MAX;
    }
    if (autoYLimit) {
        yMin = DBL_MAX, yMax = -DBL_MAX;
    }
    update();
}





//*********************************************************************
//------------------------private-Functions----------------------------
//*********************************************************************

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
    QPoint r2 = mapToGlobal(QPoint(wr.x() + wr.width(), wr.y() + wr.height()));
    QRect gwr(r1,r2);

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
    for (int i = 0; i < gridLineCountX; i++) {
        int x = px_w_sector * (i+1);
        p.setPen(gridPen);
        p.drawLine(QLine(x, 0, x, h_f));
        drawXNumber(p, x);
    }
    drawXNumber(p, w_f);

    drawYNumber(p, 0);
    int px_h_sector = h_f / (gridLineCountY+1);
    for (int i = 0; i < gridLineCountY; i++) {
        int y = px_h_sector * (i+1);
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
    if (series[i].getType() == Line) {
        drawLineSeries(i, p);
    }
    else if (series[i].getType() == Circles) {
        drawCircleSeries(i, p);
    }
}

void CoolChart::drawLineSeries(int i, QPainter& p)
{
    p.setPen(series[i].getPen());
    QLine l;
    if (series[i].getXY()->size() > 1) {
        for (int j = 0; j < series[i].getXY()->size() - 1; j++) {

            QLineF ph_l(series[i].getXY()->operator[](j), series[i].getXY()->operator[](j+1));
            if (/*DoesPhisycalLineBelongToChart(ph_l)*/1) {
                QPoint p1 = phisycalPointToPix(series[i].getXY()->operator[](j));
                QPoint p2 = phisycalPointToPix(series[i].getXY()->operator[](j+1));
                l.setP1( p1 );
                l.setP2( p2 );
                if (calcPixDist(l) >= 1)
                    p.drawLine(l);
            }
        }
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
    painter.setFont(textFont);
    painter.setPen(textColor);
    QPointF xy = pixPointToPhisycal(QPoint(x, 0));
    QString s = QString::number( xy.x(), textX_fmt, textX_prec );
    painter.setFont(textFont);
    QFontMetrics fm(textFont);
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
    painter.setFont(textFont);
    painter.setPen(textColor);
    QPointF xy = pixPointToPhisycal(QPoint(0, y));
    QString s = QString::number( xy.y(), textY_fmt, textY_prec );
    painter.setFont(textFont);
    QFontMetrics fm(textFont);
    int fontheight = fm.height();
    int fontwidth = fm.horizontalAdvance(s);
    QPoint p_txt( -(fontwidth + outerRectPen.width() + 4), -(y - fontheight/2 + 3));
    painter.scale(1, -1);
    painter.drawText(p_txt, s);
    painter.scale(1, -1);
}





//*********************************************************************
//-------------------------------Events--------------------------------
//*********************************************************************

void CoolChart::paintEvent(QPaintEvent * /* event */)
{
    QPainter Painter(this);

    if (antialiased) {
        Painter.setRenderHint(QPainter::Antialiasing, true);
    }

    //Координаты линий внешнего квадрата
    x_l = marginLeft+outerRectPen.width()/2;
    y_l = marginTop+outerRectPen.width()/2;
    w_l = this->width() - marginLeft - marginRight - outerRectPen.width();
    h_l = this->height() - marginTop - marginBottom - outerRectPen.width();

    //Координаты квадрата заполнения
    x_f = marginLeft+outerRectPen.width();
    y_f = marginTop+outerRectPen.width();
    w_f = this->width() - marginLeft - marginRight - outerRectPen.width()-outerRectPen.width();
    h_f = this->height() - marginTop - marginBottom - outerRectPen.width()-outerRectPen.width();

    drawChartRectangle(Painter);

    Painter.translate(x_f , y_f + h_f);
    Painter.scale(1, -1);

    //Начинаем рисовать
    drawChartGridAndNumbers(Painter);

    Painter.setClipRect(0, 0, w_f, h_f);

    if (!lmb_pressed && !mmb_pressed)
        drawAllSeries(Painter);

    if (lmb_pressed) {
        Painter.scale(1,-1);
        Painter.drawPixmap(0, -h_f, w_f, h_f, img, 0, 0, img.width(), img.height());
        Painter.scale(1,-1);

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
        Painter.scale(1,-1);
        Painter.drawPixmap(0, -h_f, w_f, h_f, img, 0, 0, img.width(), img.height());
        Painter.scale(1,-1);

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
            p.setWidth(/*series[i].getPen().width()*/2);
            Painter.setPen(p);
            Painter.drawEllipse(pp, series[i].getPen().width()*2, series[i].getPen().width()*2);

            QString s = "(" + QString::number(nr.x()) + "; " + QString::number(nr.y()) + ")";

            QFontMetrics fm(textFont);
            int fontheight = fm.height();
            int fontwidth = fm.horizontalAdvance(s);

            int r = r, g, b;
            p.color().getRgb(&r, &g, &b);

            int intclr = (r<<16) | (g <<8) | b;

            Painter.scale(1,-1);
            Painter.fillRect(pp.x()+15, -pp.y()+20-(fontheight-(fontheight/3)), fontwidth, fontheight, 0xFFFFFF - intclr);
            Painter.drawText(pp.x()+15, -pp.y()+20, s);
            Painter.scale(1,-1);
        }
    }
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

        update();
    }
    if (lmb_pressed) {
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
    }
    else if (event->button() == Qt::MiddleButton) {
        mmb_pressed = true;
        img = grabScreenshot();
    }
}

void CoolChart::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        lmb_pressed = false;

        int x1, x2, y1, y2;
        zoom_rect.getCoords(&x1, &y1, &x2, &y2) ;

        if (x1 > x2) {
            if (!zoom_stack.isEmpty()) {
                QRectF r = zoom_stack.pop();
                double x1, x2, y1, y2;
                r.getCoords(&x1, &x2, &y1, &y2) ;
                setLimits(x1,x2,y1,y2);
                update();
            }
            else {
                update();
            }
        }
        else if (x1 < x2) {
            QRectF r(QPointF(xMin, xMax), QPointF(yMin, yMax));
            zoom_stack.push(r);
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
}
