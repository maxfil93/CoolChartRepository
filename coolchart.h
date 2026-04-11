#ifndef COOLCHART_H
#define COOLCHART_H

#include <QPen>
#include <QStack>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QColorDialog>
#include <QOpenGLWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QString>
#include <QPointF>
#include <QPoint>
#include <QBrush>
#include <QFont>
#include <QColor>
#include <QRectF>
#include <QRect>
#include <QLineF>
#include <QLine>
#include <QSize>
#include <QLayout>

// Constants
constexpr int FONT_NUM = 5;
constexpr int PREDEF_COLORS_NUM = 16;

enum SeriesType {Line, Circles};
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

    explicit Series(CoolChart* parent);

    void addXY(QPointF p);
    void addXY(double x, double y);
    void clear();
    const QList<QPointF>& getXY() const;

    void setType(SeriesType type);
    void setBrush(const QBrush& brush);
    void setPen(const QPen& pen);
    void setVisible(bool v);
    void setXYPix(const QList<QPoint>& xyP);
    void setName(const QString& n);

    SeriesType getType() const;
    QBrush getBrush() const;
    QPen getPen() const;
    bool getVisible() const;
    const QList<QPoint>& getXYPix() const;
    QString getName() const;

    int getID() const;
    double getAvgY() const;
};

class CoolChart : public QOpenGLWidget
{
    Q_OBJECT

private:
    bool updateSuspended;
    bool antialiased;

    QPen outerRectPen;
    QPen gridPen;
    QPen crossPen;
    QBrush outerRectBrush;

    QFont textFont[FONT_NUM];
    QColor textColor[FONT_NUM];

    int marginTop, marginRight, marginBottom, marginLeft;
    double xMin, xMax, yMin, yMax;
    int gridLineCountX, gridLineCountY;

    // Coordinates of the outer rectangle lines
    int x_l, y_l, w_l, h_l;
    // Coordinates of the fill rectangle (drawing area)
    int x_f, y_f, w_f, h_f;

    // Mouse events
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

    int maxPointsPerPixel;

    QString title;
    QString xTitle;
    QString yTitle;

    // Private helper methods with proper const-correctness and safety checks
    bool doesPhysicalPointBelongToChart(const QPointF& p) const;
    bool doesPhysicalLineBelongToChart(const QLineF& l) const;
    int calcPixDist(const QLine& l) const;
    QPoint physicalPointToPix(const QPointF& point) const;
    QPointF pixPointToPhysical(const QPoint& point) const;
    QPointF findNearestPointByX(const Series& s, double x) const;
    QPixmap grabScreenshot() const;
    void zoomByRect(const QRect& r);
    void drawChartRectangle(QPainter& p) const;
    void drawChartGridAndNumbers(QPainter& p) const;
    void drawAllSeries(QPainter& p) const;
    void drawSeries(int i, QPainter& p) const;
    void drawLineSeries(int i, QPainter& p) const;
    void drawCircleSeries(int i, QPainter& p) const;
    void drawXNumber(QPainter& painter, int x) const;
    void drawYNumber(QPainter& painter, int y) const;
    void drawInfo(QPainter& p) const;
    void drawTitle(QPainter& painter) const;
    void drawAxisTitle(QPainter& painter) const;

    QListWidget* lw;
    QListWidgetItem* selectedItem;
    int selectedInd;

    QColorDialog* clrDlg;
    QLineEdit* edName;
    QSpinBox* ed;
    QComboBox* cb;
    QHBoxLayout* l1;
    QHBoxLayout* l2;
    QHBoxLayout* l3;
    QLabel* lab1;
    QLabel* lab2;
    QLabel* lab3;

    int start_px_line_x;
    int start_px_line_y;
    int min_x_y_number;  // Minimum X coordinate of Y-axis labels (for drawing Y-axis title)

public:
    explicit CoolChart(QWidget *parent = nullptr);

    void requestUpdate();
    void beginBulkUpdate();
    void endBulkUpdate(bool forceRepaint = true);

    void setAntialiased(bool antialiased);
    void setOuterRectPen(const QPen& pen);
    void setOuterRectBrush(const QBrush& brush);
    void setGridPen(const QPen& pen);
    void setGridLineCountX(int X);
    void setGridLineCountY(int Y);
    void setTextFont(const QFont& font, const QColor& color, FontOfWhat what);
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
    void setCrossPen(const QPen& p);
    void setTitle(const QString& tit);
    void setXTitle(const QString& tit);
    void setYTitle(const QString& tit);
    void setMaxPointsPerPixel(int points);

    bool getAntialiased() const;
    QPen getOuterRectPen() const;
    QBrush getOuterRectBrush() const;
    QPen getGridPen() const;
    int getGridLineCountX() const;
    int getGridLineCountY() const;
    QFont getTextFont(FontOfWhat what) const;
    QColor getTextColor(FontOfWhat what) const;
    char getTextXFormat() const;
    char getTextYFormat() const;
    int getTextXPrecision() const;
    int getTextYPrecision() const;
    int getMarginTop() const;
    int getMarginLeft() const;
    int getMarginRight() const;
    int getMarginBottom() const;
    double getXMin() const;
    double getXMax() const;
    double getYMin() const;
    double getYMax() const;
    bool getAutoXLimits() const;
    bool getAutoYLimits() const;
    QPen getCrossPen() const;
    QListWidget* getLegend() const;
    const QList<Series>& getSeries() const;
    QString getTitle() const {return title;}
    QString getXTitle() const {return xTitle;}
    QString getYTitle() const {return yTitle;}
    int getMaxPointsPerPixel() const {return maxPointsPerPixel;}

    int addSeries(const Series& s);
    Series* getSeriesByID(int id);
    void deleteSeriesById(int id);
    void clear();
    void showLegend(QLayout* lay, bool b);
    QListWidgetItem* getSelectedSeriesItem() const;
    static bool doLinesCross(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);

    int plotByFile(const QString& fn, bool firstRowIsTitle, bool firstColumnIsX);
    double stringToNumber(const QString& s, bool* ok) const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    QSize sizeHint() const override;

private slots:
    void showContextMenu(const QPoint &pos);
    void deleteSeries();
    void openColorDialog();
    void colorSelected(const QColor &color);
};

#endif // COOLCHART_H
