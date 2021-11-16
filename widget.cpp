#include "widget.h"
#include "ui_widget.h"
#include <iostream>

#include "hurst.h"

using namespace std;

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget),
    m_constAxisPointNum(20),
    m_constWindowWidth(900),
    m_constWindowHeight(900)
{
    ui->setupUi(this);
    //坐标轴起点坐标
    m_startX = 100;
    m_startY = m_constWindowHeight - 100;

    //坐标轴宽度和高度
    m_chartWidth = m_constWindowWidth - m_startX - 50;
    m_chartHeight = m_constWindowHeight - m_startX - 50;

    m_xMin = 0.0;
    m_xMax = 0.0;
    m_yMin = 0.0;
    m_yMax = 0.0;
    m_kx = 0.0;
    m_ky = 0.0;
}

Widget::~Widget()
{
    delete ui;
    delete m_painter;
}

void Widget::mSetCanvas()
{
    resize(m_constWindowWidth, m_constWindowHeight); // 窗体大小 宽度1000 高度730
    m_image = QImage(m_constWindowWidth, m_constWindowHeight, QImage::Format_RGB32);  // 画布的初始化大小设为，使用32位颜色
    QColor backColor = qRgb(255, 255, 255);    // 画布初始化背景色使用白色
    m_image.fill(backColor); // 对画布进行填充

    m_painter = new QPainter(&m_image);
    m_painter->setRenderHint(QPainter::Antialiasing, true); // 设置反锯齿模式
}

void Widget::mDrawCoordinateAxes()
{
    // 绘制X、Y轴
    QPointF xStartPoint(m_startX, m_startY);
    QPointF xEndPoint(m_chartWidth + m_startX, m_startY);
    m_painter->drawLine(xStartPoint, xEndPoint); // 坐标轴x宽度为width
    m_painter->drawText(m_startX + m_chartWidth + 10, m_startY + 35, QString("logn"));

    QPointF yStartPoint(m_startX, m_startY - m_chartHeight);
    QPointF yEndPoint(m_startX, m_startY);
    m_painter->drawLine(yStartPoint, yEndPoint); // 坐标轴y高度为height
    m_painter->drawText(m_startX - 45, m_startY - m_chartHeight - 20, QString("log(R/S)n"));

    //绘制网格
    QPen penDotLine;
    penDotLine.setStyle(Qt::DotLine);
    m_painter->setPen(penDotLine);
    for (int i = 0; i < m_constAxisPointNum; ++i)
    {
        // 垂直线
        m_painter->drawLine(m_startX + (i+1)* m_chartWidth/m_constAxisPointNum, m_startY,
                         m_startX + (i+1)* m_chartWidth/m_constAxisPointNum, m_startY - m_chartHeight);
        // 水平线
        m_painter->drawLine(m_startX, m_startY-(i+1)*m_chartHeight/m_constAxisPointNum,
                         m_startX + m_chartWidth, m_startY-(i+1)*m_chartHeight/m_constAxisPointNum);
    }
}

void Widget::mSetAxisSpace()
{
    //显示x轴刻度值
    m_painter->drawText(m_startX + 3, m_startY + 12, QString::number(0, 'f', 3));//第一个刻度
    for (int i = 0; i < m_constAxisPointNum - 1; ++i)
    {
        m_painter->drawText(m_startX + (i+0.9) * m_chartWidth / m_constAxisPointNum, m_startY + 12,
                         QString::number(m_xMin + (i+1) * ((m_xMax-m_xMin)/m_constAxisPointNum), 'f', 3));
    }
    m_painter->drawText(m_startX + (m_constAxisPointNum - 1 + 0.8) * m_chartWidth / m_constAxisPointNum, m_startY + 12,
                        QString::number(m_xMax, 'f', 3));//最后一个刻度

    //显示y轴刻度值
    m_painter->drawText(m_startX - 45, m_startY - 3, QString::number(m_yMin, 'f', 3));//第一个刻度
    for (int i = 0; i < m_constAxisPointNum - 1; ++i)
    {
        m_painter->drawText(m_startX - 45, m_startY - (i+0.85) * m_chartHeight/m_constAxisPointNum,
                         QString::number(m_yMin + (i+1) * ((m_yMax-m_yMin)/m_constAxisPointNum), 'f', 3));
    }
    m_painter->drawText(m_startX - 45, m_startY - (m_constAxisPointNum - 1 + 0.85) * m_chartHeight/m_constAxisPointNum,
                     QString::number(m_yMax, 'f', 3));//最后一个刻度
}

void Widget::mResetAxis(double xMin, double xMax, double yMin, double yMax)
{
    //onRefresh();
    m_xMin = xMin;
    m_xMax = xMax;
    m_yMin = yMin;
    m_yMax = yMax;

    mSetAxisSpace();

    m_kx = (double)(m_chartWidth / (xMax-xMin)); // x轴的比例系数
    m_ky = (double)(m_chartHeight / (yMax-yMin));  // y轴的比例系数
}

void Widget::mDrawPoint(double x, double y)
{
    QPen penPoint;
    penPoint.setColor(Qt::red);
    penPoint.setWidth(3);

    double dXStart = m_startX + m_kx * (x - m_xMin);
    m_painter->setPen(penPoint);
    m_painter->drawPoint(dXStart, m_startY - (y - m_yMin) * m_ky);
}

void Widget::onRefresh()
{
    m_painter->fillRect(0, 0, m_constWindowWidth, m_constWindowHeight, Qt::white);
    mSetCanvas();
    mDrawCoordinateAxes();
    mResetAxis(m_xMin, m_xMax, m_yMin, m_yMax);
    update();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, m_image);
}

void Widget::on_startBtn_clicked()
{
    mSetCanvas();
    mDrawCoordinateAxes();
    mResetAxis(0, 2.5, 0, 2.5);

    const int dataNum = 200;
    Hurst hurst(dataNum);
    double *ArryN = new double[dataNum + 1];
    double *ArryRS = new double[dataNum + 1];
    hurst.mGetH(ArryN, ArryRS);
    for(int i = 2; i <= dataNum; i++)
    {
        mDrawPoint(ArryN[i],ArryRS[i]);
    }

    delete [] ArryN;
    delete [] ArryRS;
    ArryN = NULL;
    ArryRS = NULL;
}

