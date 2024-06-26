#ifndef SIGNALGLOBALINFORMATION_H
#define SIGNALGLOBALINFORMATION_H

#include "fftw3.h"
#include "sample.h"
#include <QList>

///
/// Makro określające dłogość listy nawisilniejszych składowych (mp. 3 najsileniejsze częstotliwości)
///
#define FREQUENCY_RANKING_SIZE 3

///
/// \brief klasa odpowiadająca za przeprowadzanie obliczeń w celu okreslenia globalnych informacji na temat sygnału takich jak
/// Vmin - najmniejszego napiecia, Vmax - największego napięcia, V_rms - wartości skutefcznej, V_avg - wartości średniej
/// Ponadto oblicza wartości składoych częstotliwości z których złożony jest sygnał za pomocą algorytmu FFT
///
class SignalGlobalInformation
{
    QList<Sample> m_signalDataList;
    float m_minValue;
    float m_maxValue;
    float m_rmsValue;
    float m_avgValue;
    QList<FrequencySample> m_frequenciesDataList;

    ///
    /// \brief Funkcja na bierzącao aktualizująca listę najsilnijeszych składowych częstotowtliwości w sygnale.
    /// \attention Długość listy zależy do zminennej  < b>FREQUENCY_RANKING_SIZE< /b>
    ///
    void updateFrequencyRanking();
    ///
    /// \brief Funkcja dodająca nowo znalezione maksima lokalne i umieszczająca je w rankingu skłądowych
    /// FUnkcja ta bada 3 ostatnie dodane skłądowe próbki częstotliwości i szuka maksimów lokalnych
    ///
    void insertToRankingList(FrequencySample sample);
    ///
    QVector<FrequencySample> m_frequenciesDataRanking;

    float m_refValue;

public:
    SignalGlobalInformation();
    ///
    /// \brief setSignalData - Dodaje listę próbek której dane będą analizowane w celu zdobycia infromacji o sygnale
    ///
    void setSignalData(const QList<Sample> &signalDataList);
    ///
    /// \brief calculateBasicInfo - Wykonuje obliczenia podstawowych informacji o sygnale
    ///
    void calculateBasicInfo();
    ///
    /// \brief calculateFFT - Wykonuje obliczenia w dziedzinie częstotliwości
    ///
    void calculateFFT(float frequencyStrengthScale, int frequencyRange);

    float minValue();
    float maxValue();
    float rmsValue();
    float avgValue();
    float peakToPeakValue();
    QList<FrequencySample> frequenciesDataList();
    QVector<FrequencySample> frequenciesDataRanking();
};

#endif // SIGNALGLOBALINFORMATION_H
