#include "hurst.h"
#include <stdio.h>
#include <math.h>

Hurst::Hurst(int dataNum):m_totalDataNum(dataNum)
{
    m_DataArry = new int[dataNum];
    for(int i = 0; i < m_totalDataNum; i++)
    {
        m_DataArry[i] = i + 1;
    }
}

Hurst::~Hurst()
{
    delete[] m_DataArry;
    m_DataArry = NULL;
}

void Hurst::mShowData()
{
    for(int i = 0, index = 1; i < m_totalDataNum; i++, index++)
    {
        printf("%-3d", m_DataArry[i]);
        if(index % 10 == 0)printf("\n");
    }
    printf("\n");
}

void Hurst::mGetH()
{
    //double *hurst = new double[m_totalDataNum];
    for(int dataLength = 2; dataLength <= m_totalDataNum; dataLength++ )
    {
        printf("n: %d, log(n): %f, log(R/S)n: %f\n", dataLength, log10(dataLength),log10(mGetAverageRS(dataLength)));
    }
}

double Hurst::mGetAverage(int start, int dataLength)
{
    if(start < 0 || m_totalDataNum - start + 1 < dataLength)
    {
        printf("平均值求取失败, start: %d, len: %d\n", start, dataLength);
        return 0;
    }
    double sum = 0.0;
    for(int i = start; i < start + dataLength; i++)
    {
        sum += m_DataArry[i];
    }
    return sum/1.0/dataLength;
}

double Hurst::mGetStdDeviation(int start, int dataLength, double average)
{
    double sum = 0.0;
    double square = 0.0;
    for(int i = start; i < start + dataLength; i++)
    {
        square = (m_DataArry[i] - average) * (m_DataArry[i] - average);
        sum += square;
    }

    return sqrt(sum * 1.0 / dataLength);
}

double Hurst::mGetSingleDataAccumulatedDeviation(int start, int dataLength, double average, int index)
{
    if(index < start || index > dataLength + start - 1)
    {
        printf("求单个累计方差出错, start: %d, len: %d, index: %d\n", start, dataLength ,index);
        return 0;
    }

    double diff = 0.0;
    double sum = 0.0;
    for(int i = start; i <= index; i++)
    {
        diff = m_DataArry[i] - average;
        sum += diff;
    }
    return (sum * 1.0);
}

double Hurst::mGetMaxDiffOfAccumulatedDeviation(int start, int dataLength, double average)
{
    if(start < 0 || m_totalDataNum - start + 1 < dataLength)
    {
        printf("最大累计方差之差求取失败, start: %d, len: %d\n", start, dataLength);
        return 0;
    }

    double *diff = new double[dataLength];
    for(int i = 0; i < dataLength; i++)
    {
        diff[i] = mGetSingleDataAccumulatedDeviation(start, dataLength, average, start + i);
    }
    double maxDiff = mGetMaxDiff(diff, dataLength);
    delete[] diff;
    diff = NULL;
    return maxDiff;
}

double Hurst::mGetMaxDiff(double diff[], int amount)
{
    double max = diff[0], min = diff[0];
    for(int i = 0; i < amount; i++)
    {
        if(max < diff[i]) max = diff[i];
    }
    for(int i = 0; i < amount; i++)
    {
        if(min > diff[i]) min = diff[i];
    }

    return (max-min) * 1.0;
}

double Hurst::mGetAverageRS(int dataLength)
{
    if(dataLength > m_totalDataNum)
    {
        printf("数据集过少，初始化失败\n");
        return 0.0;
    }

    int arryNum = m_totalDataNum - dataLength + 1;
    double *singleRS = new double[arryNum];
    double average = 0.0;
    double stdDev = 0.0;
    double maxDiff = 0.0;
    double sum = 0.0;

    for(int i = 0; i < arryNum; i++)
    {
        average = mGetAverage(i, dataLength);
        stdDev = mGetStdDeviation(i, dataLength, average);
        maxDiff = mGetMaxDiffOfAccumulatedDeviation(i, dataLength, average);
        singleRS[i] = maxDiff * 1.0 / stdDev;
        sum += singleRS[i];
    }
    double averageRS = sum * 1.0 / arryNum;
    delete [] singleRS;
    singleRS = NULL;
    return averageRS;
}
