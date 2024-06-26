#ifndef SPECTRUMCHART_H
#define SPECTRUMCHART_H

#include <QObject>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>

#include "sample.h"
#include "cursor.h"

#define SPECTRUM_COLOR_RGB_CHANNEL_1 83,237,83
#define SPECTRUM_COLOR_RGB_CHANNEL_2 74,182,212
#define SPECTRUM_COLOR_RGB_CHANNEL_1_STRING QString("83,237,83")
#define SPECTRUM_COLOR_RGB_CHANNEL_2_STRING QString("74,182,212")


///
/// \brief Klasa odpowidjąca za poprawne wyświetlanie sygnału w dizediznie częstotliwości na wykresie, wykorzystuję listę danych wyliczonych
/// z FFT
///
class SpectrumChart : public QObject
{
    Q_OBJECT

private:
    QChartView *m_chartVeiw;

    QChart *m_spectrumChart;
    QLineSeries *m_spectrumSeries;

    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;

    Channel m_observedChannels;
    Cursor *m_cursor_1;
    Cursor *m_cursor_2;

    int m_numberOfVerticalTicks;
    int m_numberOfHorizontalTicks;

    unsigned int m_frequencyRange;

public:
    explicit SpectrumChart(QChartView *chartView ,QObject *parent = nullptr);
    ///
    /// \brief Funkcja aktualizaująca zestaw danych wyświetlanych na wykresach wykorzystując listy z próbkami.
    ///
    void updateChart(QList<FrequencySample> dataListCh_1, QList<FrequencySample> dataListCh_2);

    ///
    /// \brief Funkcja azwraca aktuaklny zakres wyśweitlanje częstotliwości
    ///
    unsigned int frequencyRange();
    ///
    /// \brief Funkcja azwraca aktuaklnie obserwowany kanał
    ///
    Channel observedChannels();

signals:
    ///
    /// \brief Sygnał informujący o zmiaie wartości aktualnie obserwowanych przez którykolwiek z kurosrów
    ///
    void cursorsDataUpdated(CursorData cursor1, CursorData cursor2);

public slots:
    ///
    /// \brief Slot pozwaljący na ustawienie widoczności kursora 1 - umożliwia wyłączneie lub aktywację kursora
    ///
    void setEnableCursor1(int index);
    ///
    /// \brief Slot pozwaljący na ustawienie widoczności kursora 2 - umożliwia wyłączneie lub aktywację kursora
    ///
    void setEnableCursor2(int index);

    ///
    /// \brief Slot pozwaljący skonfigurawać który kanał jest aktualnie wyświetlany na wykresie
    ///
    void setObservedChannels(int index);
    ///
    /// \brief Slot wywoływany przez kursory gdy zaktualizowana zostaała ich obserwowana wartość
    ///
    void updateCursorsData();

    ///
    /// \brief Slot pozwalający wybrać zakres częstotliwości wyświetlany  na wykresie częsrtotliwosći.
    ///
    void setFrequencyRange(QString rangeString);
};

#endif // SPECTRUMCHART_H
