#ifndef OSCILLOSCOPECHART_H
#define OSCILLOSCOPECHART_H

#include <QObject>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>

#include "sample.h"
#include "cursor.h"

enum class TriggerSlope {RISING, FALLING};

#define COLOR_RGB_CHANNEL_1 83,237,83
#define COLOR_RGB_CHANNEL_2 74,182,212
#define COLOR_RGB_CHANNEL_1_STRING QString("83,237,83")
#define COLOR_RGB_CHANNEL_2_STRING QString("74,182,212")

///
/// \brief Klasa odpowidjąca za poprawne wyświetlanie odebranych danych na wykresie wraz z uwzględnieniem konfiguracji sposobu wyświetlania
/// dla poszczególnych kanałów.
///
class OscilloscopeChart : public QObject
{
private:
    QChartView *m_chartVeiw;

    QChart *m_oscilloscopeChart;
    QLineSeries *m_oscilloscopeSeries_Ch1;
    QLineSeries *m_oscilloscopeSeries_Ch2;

    float m_verticalDivCh_1;
    float m_verticalDivCh_2;
    float m_verticalOffsetCh_1;
    float m_verticalOffsetCh_2;
    float parseStringToValue(QString value);

    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;

    int m_timeBase_us;
    Channel m_observedChannels;

    Cursor *m_cursor_1;
    Cursor *m_cursor_2;
    ///
    /// \brief correctCursorData - Funkcja zmieniająca wartość bezpośrednio odczytaną z wykresu i zmeiniająca ją na wartość rzeczyczywostego sygnału
    /// uwzględniając użyte przesunięcia i skalowania w pionie
    ///
    CursorData correctCursorData(CursorData cursorData, float verticalDiv, float verticalOffset);

    int m_numberOfVerticalTicks;
    int m_numberOfHorizontalTicks;
    float m_normalScaleDiv;             //standardowo jedna podziałka to 1,25V

    Q_OBJECT
public:
    explicit OscilloscopeChart(QChartView *chartView ,QObject *parent = nullptr);
    ///
    /// \brief setObservedChannels - Funkcja pozwalająca ustalić aktualnie wyświetlane na ekranie kanały
    ///
    void setObservedChannels(Channel observedChannels);
    ///
    /// \brief timeBase - Funkcja zwracająca aktualnie ustawioną podstawę czasową
    ///
    int timeBase();

signals:
    ///
    /// \brief cursorsDataUpdated - Sygnał informujący o tym że dane któregokoliwek z kursorów zostaKu zaktualizowane
    ///
    void cursorsDataUpdated(CursorData cursor1, CursorData cursor2);

public slots:
    ///
    /// \brief updateChart - Slot aktualizujący zawartośc wykresu najnowyszymi danymi z sygnału
    ///
    void updateChart(QList<Sample> dataListCh_1, QList<Sample> dataListCh_2);
    ///
    /// \brief setTimeBase - Slot ustawiający aktualną podstawę czasową
    ///
    void setTimeBase(QString timeBase);

    ///
    /// \brief setVerticalDivCh_1 - Slot ustawiający aktualną podziałkę wertykalną wyświetlango sygmnału (skalowanie wzdłóż osi Y)
    /// kanału 1
    ///
    void setVerticalDivCh_1(QString valueDiv);
    ///
    /// \brief setVerticalDivCh_2 - Slot ustawiający aktualną podziałkę wertykalną wyświetlango sygmnału (skalowanie wzdłóż osi Y)
    /// kanału 2
    ///
    void setVerticalDivCh_2(QString valueDiv);
    ///
    /// \brief setVerticalOffsetCh_1 - Slot ustawiający prz3sunięcie sygnału na wykresie w pionie (Offset na osi Y) dla kanału 1
    ///
    void setVerticalOffsetCh_1(double verticalOffsetCh_1);
    ///
    /// \brief setVerticalOffsetCh_2 - Slot ustawiający prz3sunięcie sygnału na wykresie w pionie (Offset na osi Y) dla kanału 2
    ///
    void setVerticalOffsetCh_2(double verticalOffsetCh_2);

    ///
    /// \brief setObservingChannelCursor_1 - Slot pozwalający zadać kursorowi 1 jaki kanał powinien aktualnie obserwować i śledzić jego wartość
    ///
    void setObservingChannelCursor_1(int index);
    ///
    /// \brief setObservingChannelCursor_2 - Slot pozwalający zadać kursorowi 1 jaki kanał powinien aktualnie obserwować i śledzić jego wartość
    ///
    void setObservingChannelCursor_2(int index);
    ///
    /// \brief updateCursorsData - SLot aktualizujący wyświetalnie kursorów
    ///
    void updateCursorsData();
};

#endif // OSCILLOSCOPECHART_H
