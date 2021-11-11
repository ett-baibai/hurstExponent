#ifndef HURST_H
#define HURST_H

class Hurst
{
public:
    Hurst(int totalDataNum);
    ~Hurst();

    void mShowData();
    void mGetH();
private:
    double mGetAverage(int start, int dataLength);
    double mGetStdDeviation(int start, int dataLength, double average);
    double mGetSingleDataAccumulatedDeviation(int start, int dataLength, double average, int index);
    double mGetMaxDiffOfAccumulatedDeviation(int start, int dataLength, double average);
    double mGetMaxDiff(double diff[], int amount);
    double mGetAverageRS(int dataLength);
    const int m_totalDataNum;
    int* m_DataArry;
};

#endif // HURST_H
