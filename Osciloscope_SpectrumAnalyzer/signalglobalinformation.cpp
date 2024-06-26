 #include "signalglobalinformation.h"
#include <cmath>
#include <QtDebug>

void SignalGlobalInformation::updateFrequencyRanking()
{
    if(m_frequenciesDataList.size() < 3) return;

    unsigned currentIndex = m_frequenciesDataList.size() - 1;

    if(m_frequenciesDataList[currentIndex - 2].value < m_frequenciesDataList[currentIndex - 1].value &&
       m_frequenciesDataList[currentIndex].value < m_frequenciesDataList[currentIndex - 1].value)
    {
        insertToRankingList(m_frequenciesDataList[currentIndex - 1]);
    }
}

///
/// \param sample - próbka będąca jednym z maksimów lokalnych sygnału  wdziedzinie częstotliwości
///
void SignalGlobalInformation::insertToRankingList(FrequencySample sample)
{
    if(m_frequenciesDataRanking[FREQUENCY_RANKING_SIZE - 1].value > sample.value) return;

    for(int i = FREQUENCY_RANKING_SIZE - 2; i >= 0; i--)
    {
        if(m_frequenciesDataRanking[i].value > sample.value){
            m_frequenciesDataRanking.insert(i + 1, sample);
            m_frequenciesDataRanking.removeLast();
            return;
        }
    }
    m_frequenciesDataRanking.push_front(sample);
    m_frequenciesDataRanking.removeLast();
}

SignalGlobalInformation::SignalGlobalInformation(){
    m_maxValue = 0;
    m_minValue = 0;
    m_rmsValue = 0;
    m_avgValue = 0;

    m_refValue = 10;
}

///
/// \param signalDataList - lista danych do analizy i wydobycia informacji o sygnale
///
void SignalGlobalInformation::setSignalData(const QList<Sample> &signalDataList){
    m_signalDataList = signalDataList;
    m_maxValue = 0;
    m_minValue = 0;
    m_rmsValue = 0;
    m_avgValue = 0;
}
///
/// Funkcja oblicza wartości Vmin - najmniejszego napiecia, Vmax - największego napięcia,
/// V_rms - wartości skutefcznej, V_avg - wartości średniej
///
void SignalGlobalInformation::calculateBasicInfo()
{
    int dataListSize = m_signalDataList.size();
    m_maxValue = std::numeric_limits<float>::min();
    m_minValue = std::numeric_limits<float>::max();
    m_avgValue = 0;

    int sumOfSqueredVoltages = 0;

    for(int i = 0; i < dataListSize; i++){
        float currentValue = m_signalDataList[i].value;
        m_avgValue += currentValue;
        if(currentValue > m_maxValue)
            m_maxValue = currentValue;
        if(currentValue < m_minValue)
            m_minValue = currentValue;
        sumOfSqueredVoltages += std::pow(currentValue, 2);
    }
    m_avgValue /= dataListSize;
    m_rmsValue = std::sqrt(sumOfSqueredVoltages / dataListSize);
}
///
///
/// Funkcja oblicza częstotliwośc o największe sile występującą w sygnale oraz wartości sily poszczególnych skłądowych częstotliwości
/// \param frequencyStrengthScale - zmienna pozwalająca znormalizować siłę skłądych poprzez skalowanie wartości
/// \param frequencyRange - zakres wyświetlanych częstotliwości pozwala zooptymalizować czas kopionoania wartośći do list poprzez
/// obcięcie wartośći, i tak nie widocznych na wykresie
///
void SignalGlobalInformation::calculateFFT(float frequencyStrengthScale, int frequencyRange)
{
    m_frequenciesDataList.clear();
    m_frequenciesDataRanking.clear();
    m_frequenciesDataRanking.resize(FREQUENCY_RANKING_SIZE);

    int dataListSize = m_signalDataList.size();
    int dataListHalfSize = dataListSize / 2;
    double max_Frequency = (1.0 / ((double)m_signalDataList[1].time / 1000000.0)) / 2.0;

    fftw_complex *in, *out;
    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * dataListSize);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * dataListSize);
    fftw_plan p;

    for(int i = 0; i < dataListSize; i++){
        in[i][0] = (double)m_signalDataList[i].value;
        in[i][1] = 0;
    }

    p = fftw_plan_dft_1d(dataListSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    for(int i = 1; i < dataListHalfSize; i++){
        FrequencySample sample;
        // NOrmlaizacja
        sample.value = std::sqrt(std::pow(out[i][0], 2) + std::pow(out[i][1], 2)) * frequencyStrengthScale;
        // Konwersja na częstotliwość
        sample.frequency = (double)i / (double)dataListHalfSize * max_Frequency;
        //qDebug() << sample.frequency;
        //m_frequenciesDataList.push_back(FrequencySample((double)sample.value, (double)sample.frequency));
        m_frequenciesDataList.push_back(sample);
        updateFrequencyRanking();
        if(sample.frequency > frequencyRange) break;
    }

    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
    fftw_cleanup();
}

///
/// \return Funckja zwraca najmniejszą zmierzoną wartość w sygnale
///
float SignalGlobalInformation::minValue(){
    return m_minValue;
}
///
/// \return Funckja zwraca największą zmierzoną wartość w sygnale
///
float SignalGlobalInformation::maxValue(){
    return m_maxValue;
}
///
/// \return Funckja zwraca wartość skuteczną sygnału
///
float SignalGlobalInformation::rmsValue(){
    return m_rmsValue;
}
///
/// \return Funckja zwraca wartość średnią sygnału
///
float SignalGlobalInformation::avgValue(){
    return m_avgValue;
}
///
/// \return Funckja zwraca amplitudę min i max sygnałów
///
float SignalGlobalInformation::peakToPeakValue(){
    return m_maxValue - m_minValue;
}
///
/// \return Funckja zwraca listę próbek sygnału w dziediznie częstotliwości
///
QList<FrequencySample> SignalGlobalInformation::frequenciesDataList(){
    return m_frequenciesDataList;
}
///
/// \return Funckja zwraca ranking najsilniejszyhc skłądowych częstotliwości
/// \attention w rankingu znajduje się < b>FREQUENCY_RANKING_SIZE< /b> wartości.
///
QVector<FrequencySample> SignalGlobalInformation::frequenciesDataRanking(){
    return m_frequenciesDataRanking;
}





