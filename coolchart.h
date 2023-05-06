#ifndef COOLCHART_H
#define COOLCHART_H

#include <QPen>
#include <QStack>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QColorDialog>
#include <QOpenGLWidget>

enum SeriesType {Line, Circles};

#define Font_NUM 5
enum FontOfWhat {FTitle, FAxisXNumbers, FAxisYNumbers, FAxisXTitle, FAxisYTitle};

class CoolChart;
class Series;

class Series
{
private:
    SeriesType type;
    QList<QPointF> xy;
    QList<QPoint> xyPix;
    QBrush brush;
    QPen pen;
    CoolChart* parent;
    static int cnt;
    int id;
    double max_x, min_x, max_y, min_y;
    bool visible;

    double avg_sum_y;
    double avg_n_y;
    double avg_y;

    QString name;



public:
    double avg_vis_sum_y;
    double avg_vis_n_y;
    double avg_vis_y;

    int first_drawable_point_ind;
    double first_drawable_point_x;

    Series(CoolChart* parent);

    void addXY(QPointF p);
    void addXY(double x, double y);
    void clear();
    QList<QPointF>* getXY();

    void setType(SeriesType type);
    void setBrush(QBrush brush);
    void setPen(QPen pen);
    void setVisible(bool v);
    void setXYPix(QList<QPoint> xyP){xyPix = xyP;}
    void setName(QString n) {name = n;}

    SeriesType getType();
    QBrush getBrush();
    QPen getPen();
    bool getVisible();
    QList<QPoint>* getXYPix(){return &xyPix;}
    QString getName() {return name;}

    int getID(){return id;}
    double getAvgY() {return avg_y;}
};

class CoolChart : public QOpenGLWidget
{
    Q_OBJECT

private:
    bool antialiased;

    QPen outerRectPen;
    QPen gridPen;
    QPen crossPen;

    QBrush outerRectBrush;

    QFont textFont[Font_NUM];
    QColor textColor[Font_NUM];

    int marginTop, marginRight, marginBottom, marginLeft;

    double xMin, xMax, yMin, yMax;

    int gridLineCountX, gridLineCountY;

    //Координаты линий внешнего квадрата
    int x_l, y_l, w_l, h_l;

    //Координаты квадрата заполнения (область рисования)
    int x_f, y_f, w_f, h_f;

    //Для событий мыши
    bool lmb_pressed;
    bool rmb_pressed;
    bool mmb_pressed;
    QPointF rmb_pr_p_f;
    QPoint rmb_pr_p_p;
    QRect zoom_rect;
    QPixmap img;
    QStack<QRectF> zoom_stack;

    int crossLineX;
    int crossLineY;

    QList<Series> series;

    bool autoXLimit;
    bool autoYLimit;

    char textX_fmt;
    int textX_prec;
    char textY_fmt;
    int textY_prec;

    bool draw_inf_enabled;

    bool zoom_rect_draw_enable;

    bool zoom_by_wheel_x;
    bool zoom_by_wheel_y;
    bool smooth_scale;

    QString title;
    QString xTitle;
    QString yTitle;

    bool doesPhisycalPointBelongToChart(QPointF p);
    bool doesPhisycalLineBelongToChart(QLineF l);
    int calcPixDist(QLine l);
    QPoint  phisycalPointToPix(QPointF point);
    QPointF pixPointToPhisycal(QPoint point);
    QPointF findNearestPointByX(Series& s, double x);
    QPixmap grabScreenshot();
    void zoomByRect(QRect r);
    void drawChartRectangle(QPainter& p);
    void drawChartGridAndNumbers(QPainter& p);
    void drawAllSeries(QPainter& p);
    void drawSeries(int i, QPainter& p);
    void drawLineSeries(int i, QPainter& p);
    void drawCircleSeries(int i, QPainter& p);
    void drawXNumber(QPainter& painter, int x);
    void drawYNumber(QPainter& painter, int y);
    void DrawInf(QPainter& p);
    void drawTitle(QPainter& painter);
    void drawAxisTitle(QPainter& painter);

    QListWidget* lw;
    QListWidgetItem* selectedItem;
    int selectedInd;

    QColorDialog* clrDlg;

    int start_px_line_x;
    int start_px_line_y;

    int min_x_y_number;  //Минимальная координата X подписей по оси Y (Для того, чтобы узнать где рисовать заголовок оси Y)

public:
    CoolChart(QWidget *ob = 0);

    void setAntialiased(bool antialiased);
    void setOuterRectPen(const QPen &pen);
    void setOuterRectBrush(const QBrush &brush);
    void setGridPen(const QPen &pen);
    void setGridLineCountX(int X);
    void setGridLineCountY(int Y);
    void setTextFont(QFont font, QColor color, FontOfWhat what);
    void setTextXFormat(char fmt);
    void setTextYFormat(char fmt);
    void setTextXPrecision(int precision);
    void setTextYPrecision(int precision);
    void setMarginTop(int top);
    void setMarginLeft(int left);
    void setMarginRight(int right);
    void setMarginBottom(int bottom);
    void setMargins(int top, int left, int right, int bottom);
    void setXMin(double xMin);
    void setXMax(double xMax);
    void setYMin(double yMin);
    void setYMax(double yMax);
    void setLimits(double xMin, double xMax, double yMin, double yMax);
    void setAutoXLimits(bool autoX);
    void setAutoYLimits(bool autoY);
    void setCrossPen(QPen p);
    void setTitle(QString tit);
    void setXTitle(QString tit);
    void setYTitle(QString tit);

    bool getAntialiased();
    QPen getOuterRectPen();
    QBrush getOuterRectBrush();
    QPen getGridPen();
    int getGridLineCountX();
    int getGridLineCountY();
    QFont getTextFont(FontOfWhat what);
    QColor getTextColor(FontOfWhat what);
    char getTextXFormat();
    char getTextYFormat();
    int getTextXPrecision();
    int getTextYPrecision();
    int getMarginTop();
    int getMarginLeft();
    int getMarginRight();
    int getMarginBottom();
    double getXMin();
    double getXMax();
    double getYMin();
    double getYMax();
    bool getAutoXLimits();
    bool getAutoYLimits();
    QPen getCrossPen();
    QListWidget* getLegend();
    QList<Series>* getSeries();
    QString getTitle();
    QString getXTitle();
    QString getYTitle();

    int addSeries(Series s);
    Series* getSeriesByID(int id);
    void deleteSeriesById(int id);
    void clear();
    void showLegend(QLayout*lay, bool b);
    QListWidgetItem* getSelectedSeriesItem();
    bool do_lines_cross(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);

    int plotByFile(QString fn, bool firstRowIsTitle, bool firstColumnIsX);

    double QStringToNumber(QString s, bool* ok);

protected:
      void paintEvent(QPaintEvent *event) override;
      void mouseMoveEvent(QMouseEvent *event) override;
      void mousePressEvent(QMouseEvent *event) override;
      void mouseReleaseEvent(QMouseEvent *event) override;
      void wheelEvent(QWheelEvent* event)override;

      void keyPressEvent(QKeyEvent* event) override;
      void keyReleaseEvent(QKeyEvent* event) override;

      QSize sizeHint()const override;

private slots:
      void showContextMenu(const QPoint &pos);
      void deleteSeies();
      void openColorDialog();
      void colorSelected(const QColor &color);

};

#endif // COOLCHART_H
