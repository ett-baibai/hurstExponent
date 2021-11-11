#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtGui>
#include <QPaintEvent>
#include <QMainWindow>
#include <ctime>
#include <cstdlib>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void mSetCanvas();
    void mDrawCoordinateAxes();
    void mSetAxisSpace();
    void mResetAxis(double xMin, double xMax, double yMin, double yMax);
    void mDrawPoint(double x, double y);

public slots:
    void onRefresh();

protected:
     void paintEvent(QPaintEvent *);

private slots:
     void on_startBtn_clicked();

private:
    Ui::Widget *ui;
    QImage m_image;
    QPainter *m_painter;

    int m_startX;//x轴起始点
    int m_startY;//y轴起始点
    int m_chartWidth;
    int m_chartHeight;

    double m_xMin;
    double m_xMax;
    double m_yMin;
    double m_yMax;
    double m_kx; // x轴的系数
    double m_ky;

    const int m_constAxisPointNum;
    const int m_constWindowWidth;
    const int m_constWindowHeight;
};
#endif // WIDGET_H
