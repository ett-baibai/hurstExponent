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
    //�������������
    m_startX = 100;
    m_startY = m_constWindowHeight - 100;

    //�������Ⱥ͸߶�
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
    resize(m_constWindowWidth, m_constWindowHeight); // �����С ���1000 �߶�730
    m_image = QImage(m_constWindowWidth, m_constWindowHeight, QImage::Format_RGB32);  // �����ĳ�ʼ����С��Ϊ��ʹ��32λ��ɫ
    QColor backColor = qRgb(255, 255, 255);    // ������ʼ������ɫʹ�ð�ɫ
    m_image.fill(backColor); // �Ի����������

    m_painter = new QPainter(&m_image);
    m_painter->setRenderHint(QPainter::Antialiasing, true); // ���÷����ģʽ
}

void Widget::mDrawCoordinateAxes()
{
    // ����X��Y��
    QPointF xStartPoint(m_startX, m_startY);
    QPointF xEndPoint(m_chartWidth + m_startX, m_startY);
    m_painter->drawLine(xStartPoint, xEndPoint); // ������x���Ϊwidth
    m_painter->drawText(m_startX + m_chartWidth + 10, m_startY + 35, QString("logn"));

    QPointF yStartPoint(m_startX, m_startY - m_chartHeight);
    QPointF yEndPoint(m_startX, m_startY);
    m_painter->drawLine(yStartPoint, yEndPoint); // ������y�߶�Ϊheight
    m_painter->drawText(m_startX - 45, m_startY - m_chartHeight - 20, QString("log(R/S)n"));

    //��������
    QPen penDotLine;
    penDotLine.setStyle(Qt::DotLine);
    m_painter->setPen(penDotLine);
    for (int i = 0; i < m_constAxisPointNum; ++i)
    {
        // ��ֱ��
        m_painter->drawLine(m_startX + (i+1)* m_chartWidth/m_constAxisPointNum, m_startY,
                         m_startX + (i+1)* m_chartWidth/m_constAxisPointNum, m_startY - m_chartHeight);
        // ˮƽ��
        m_painter->drawLine(m_startX, m_startY-(i+1)*m_chartHeight/m_constAxisPointNum,
                         m_startX + m_chartWidth, m_startY-(i+1)*m_chartHeight/m_constAxisPointNum);
    }
}

void Widget::mSetAxisSpace()
{
    //��ʾx��̶�ֵ
    m_painter->drawText(m_startX + 3, m_startY + 12, QString::number(0, 'f', 3));//��һ���̶�
    for (int i = 0; i < m_constAxisPointNum - 1; ++i)
    {
        m_painter->drawText(m_startX + (i+0.9) * m_chartWidth / m_constAxisPointNum, m_startY + 12,
                         QString::number(m_xMin + (i+1) * ((m_xMax-m_xMin)/m_constAxisPointNum), 'f', 3));
    }
    m_painter->drawText(m_startX + (m_constAxisPointNum - 1 + 0.8) * m_chartWidth / m_constAxisPointNum, m_startY + 12,
                        QString::number(m_xMax, 'f', 3));//���һ���̶�

    //��ʾy��̶�ֵ
    m_painter->drawText(m_startX - 45, m_startY - 3, QString::number(m_yMin, 'f', 3));//��һ���̶�
    for (int i = 0; i < m_constAxisPointNum - 1; ++i)
    {
        m_painter->drawText(m_startX - 45, m_startY - (i+0.85) * m_chartHeight/m_constAxisPointNum,
                         QString::number(m_yMin + (i+1) * ((m_yMax-m_yMin)/m_constAxisPointNum), 'f', 3));
    }
    m_painter->drawText(m_startX - 45, m_startY - (m_constAxisPointNum - 1 + 0.85) * m_chartHeight/m_constAxisPointNum,
                     QString::number(m_yMax, 'f', 3));//���һ���̶�
}

void Widget::mResetAxis(double xMin, double xMax, double yMin, double yMax)
{
    //onRefresh();
    m_xMin = xMin;
    m_xMax = xMax;
    m_yMin = yMin;
    m_yMax = yMax;

    mSetAxisSpace();

    m_kx = (double)(m_chartWidth / (xMax-xMin)); // x��ı���ϵ��
    m_ky = (double)(m_chartHeight / (yMax-yMin));  // y��ı���ϵ��
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

