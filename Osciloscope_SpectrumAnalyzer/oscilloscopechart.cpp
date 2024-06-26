#include <QGraphicsLayout>
#include <QDebug>
#include <QString>
#include <QLocale>

#include "oscilloscopechart.h"

#define MICRO_TO_MILLISECONDS(x) x/1000.0
#define MICRO_TO_SECONDS(x) x/1000000.0

///
/// \param value - wartośc w postaci łańcucha znaków z infromacją o wartości w postaci String i w okreslonym formacie
/// i zmieniający ją na wartość typu float
/// \return wartość odczytana w postaci String na wartość w postaci float
///
float OscilloscopeChart::parseStringToValue(QString value)
{
    float result = 0;;
    QStringList bufforList = value.split(' ');
    if(bufforList.count() < 2) return result;

    QLocale commaFloatFormat(QLocale::Polish);
    result = commaFloatFormat.toFloat(bufforList[0]);
    if(bufforList[1][0] == 'm') result /= 1000;
    return result;
}

///
/// \param cursorData - Struktura z wartościami danych odczytanych z wykresu z serii danych którą aktualnie śledzi
/// \param verticalDiv - Aktualnie zastosowane dla danego sygnału skalowanie wartości w pionie [V/div]
/// \param verticalOffset - Aktualnie zastosowane przesunięcie w pionie [V]
/// \return Znormalizowana, rzeczywosta wartośc napiecia w chwili okreslonej przez kursor
///
CursorData OscilloscopeChart::correctCursorData(CursorData cursorData, float verticalDiv, float verticalOffset)
{
    cursorData.value = (cursorData.value - verticalOffset) * verticalDiv / m_normalScaleDiv;
    return cursorData;
}

OscilloscopeChart::OscilloscopeChart(QChartView *chartView ,QObject *parent)
    : QObject{parent}, m_chartVeiw(chartView)
{
    m_numberOfHorizontalTicks = 5;
    m_numberOfVerticalTicks = 11;
    m_normalScaleDiv = 1.25;

    m_oscilloscopeSeries_Ch1 = new QLineSeries();
    m_oscilloscopeSeries_Ch2 = new QLineSeries();

    m_oscilloscopeChart = new QChart();
    m_chartVeiw->setChart(m_oscilloscopeChart);

    m_oscilloscopeChart->addSeries(m_oscilloscopeSeries_Ch1);
    m_oscilloscopeChart->addSeries(m_oscilloscopeSeries_Ch2);
    m_oscilloscopeChart->legend()->hide();
    m_oscilloscopeChart->setMargins(QMargins(2,12,12,2));

    m_oscilloscopeChart->layout()->setContentsMargins(0,0,0,0);
    m_oscilloscopeChart->setBackgroundRoundness(0);
    m_oscilloscopeChart->setTheme(QChart::ChartThemeDark);

    m_xAxis = new QValueAxis;
    m_xAxis->setRange(0,700);
    m_xAxis->setTickCount(m_numberOfVerticalTicks);
    m_oscilloscopeChart->addAxis(m_xAxis, Qt::AlignBottom);
    m_yAxis = new QValueAxis;
    m_yAxis->setRange(0,5.0);
    m_yAxis->setTickCount(m_numberOfHorizontalTicks);
    m_yAxis->setLabelsVisible(false);
    m_oscilloscopeChart->addAxis(m_yAxis, Qt::AlignLeft);

    m_oscilloscopeSeries_Ch1->setColor(QColor(COLOR_RGB_CHANNEL_1));
    m_oscilloscopeSeries_Ch1->clear();
    m_oscilloscopeSeries_Ch1->attachAxis(m_xAxis);
    m_oscilloscopeSeries_Ch1->attachAxis(m_yAxis);

    m_oscilloscopeSeries_Ch2->setColor(QColor(COLOR_RGB_CHANNEL_2));
    m_oscilloscopeSeries_Ch2->clear();
    m_oscilloscopeSeries_Ch2->attachAxis(m_xAxis);
    m_oscilloscopeSeries_Ch2->attachAxis(m_yAxis);

    m_observedChannels = Channel::CH_1;

    m_cursor_1 = new Cursor(m_oscilloscopeChart);
    m_cursor_1->setObservedDataSeries(Channel::CH_2, m_oscilloscopeSeries_Ch2);
    m_cursor_1->setPos(100,0);
    chartView->scene()->addItem(m_cursor_1);
    QObject::connect(m_cursor_1, SIGNAL(cursorDataUpdated()), this, SLOT(updateCursorsData()));

    m_cursor_2 = new Cursor(m_oscilloscopeChart);
    m_cursor_2->setObservedDataSeries(Channel::CH_2, m_oscilloscopeSeries_Ch2);
    m_cursor_2->setPos(100,0);
    chartView->scene()->addItem(m_cursor_2);
    QObject::connect(m_cursor_2, SIGNAL(cursorDataUpdated()), this, SLOT(updateCursorsData()));
}

///
/// \param observedChannels - akanał który ma być aktualnie wyświetlany na wykresie (CH1, CH2, Brak, Zarówno CH1 i CH2)
///
void OscilloscopeChart::setObservedChannels(Channel observedChannels){
    m_observedChannels = observedChannels;
    m_oscilloscopeSeries_Ch1->clear();
    m_oscilloscopeSeries_Ch2->clear();
}

///
/// \return zwraca aktualnie ustawianą podstawę czasową w postaci mikrosekund na przedziałkę
///
int OscilloscopeChart::timeBase()
{
    return m_timeBase_us;
}

///
/// \param dataListCh_1 - Lista danych sygnału zebranych z kanału pierwszego
/// \param dataListCh_2 - Lista danych sygnału zebranych z kanału drugiego
///
/// Funkcja wyświetla wszytskie punkty z listy na wykresie uwzględniając aktualne przesunięcie w pionie i skalowanie
///
void OscilloscopeChart::updateChart(QList<Sample> dataListCh_1, QList<Sample> dataListCh_2)
{
    m_oscilloscopeSeries_Ch1->clear();
    m_oscilloscopeSeries_Ch2->clear();

    if(m_observedChannels == Channel::CH_1 || m_observedChannels == Channel::CH_1_AND_CH_2)
    {
        foreach (Sample sample, dataListCh_1) {
            m_oscilloscopeSeries_Ch1->append(MICRO_TO_MILLISECONDS(sample.time),
                                             sample.value * m_normalScaleDiv / m_verticalDivCh_1 + m_verticalOffsetCh_1);
        }
    }
    if(m_observedChannels == Channel::CH_2 || m_observedChannels == Channel::CH_1_AND_CH_2)
    {
        foreach (Sample sample, dataListCh_2) {
            m_oscilloscopeSeries_Ch2->append(MICRO_TO_MILLISECONDS(sample.time),
                                             sample.value * m_normalScaleDiv / m_verticalDivCh_2 + m_verticalOffsetCh_2);
        }
    }
    m_oscilloscopeChart->update();

    m_cursor_1->updateCursorData();
    m_cursor_2->updateCursorData();
}

///
/// \param timeBase - Łańcuch znakóW pozwaljący określić aktualną poidstawę czasową
///
void OscilloscopeChart::setTimeBase(QString timeBase)
{
    int textNumber;
    bool conversionError;

    QStringList timeBaseTextList = timeBase.split(" ");
    if(timeBaseTextList.size() != 2)
        return;
    textNumber = timeBaseTextList[0].toInt(&conversionError);
    if(!conversionError)
        return;
    if(timeBaseTextList[1] == "ms/div"){
        m_xAxis->setMax(textNumber * (m_numberOfVerticalTicks - 1));
    }
    else{
        m_xAxis->setMax(textNumber * (m_numberOfVerticalTicks - 1) / 1000);
    }
}

///
/// \param valueDiv - Łańcuch znaków pozwaljący okreśłać aktualną podziałkę pionową dla kanału 1 (ile woltów na poedziałkę)
///
void OscilloscopeChart::setVerticalDivCh_1(QString valueDiv){
    m_verticalDivCh_1 = parseStringToValue(valueDiv);
}
///
/// \param valueDiv - Łańcuch znaków pozwaljący okreśłać aktualną podziałkę pionową dla kanału 2 (ile woltów na poedziałkę)
///
void OscilloscopeChart::setVerticalDivCh_2(QString valueDiv){
    m_verticalDivCh_2 = parseStringToValue(valueDiv);
}
///
/// \param verticalOffsetCh_1 - wartośc przesunięcia sygnału w pionie w postaci napiećIa któR zostanie dodane do rzeczywostego sygnału
/// z kanału 1
///
void OscilloscopeChart::setVerticalOffsetCh_1(double verticalOffsetCh_1){
    m_verticalOffsetCh_1 = verticalOffsetCh_1;
}
///
/// \param verticalOffsetCh_1 - wartośc przesunięcia sygnału w pionie w postaci napiećIa któR zostanie dodane do rzeczywostego sygnału
/// z kanału 2
///
void OscilloscopeChart::setVerticalOffsetCh_2(double verticalOffsetCh_2){
    m_verticalOffsetCh_2 = verticalOffsetCh_2;
}

///
/// \param index - indeks z elemntu graficznego klasy ComboBox wskazującey na wybrany kanał do obserwowania przez kursor 1
///
void OscilloscopeChart::setObservingChannelCursor_1(int index)
{
    if(index == 0){
        m_cursor_1->setObservedDataSeries(Channel::NONE);
        m_cursor_1->setVisible(false);
        updateCursorsData();
        return;
    }
    if(index == 1){
        m_cursor_1->setObservedDataSeries(Channel::CH_1, m_oscilloscopeSeries_Ch1);
    }
    else{
        m_cursor_1->setObservedDataSeries(Channel::CH_2, m_oscilloscopeSeries_Ch2);
    }
    m_cursor_1->setVisible(true);
    updateCursorsData();
}
///
/// \param index - indeks z elemntu graficznego klasy ComboBox wskazującey na wybrany kanał do obserwowania przez kursor 2
///
void OscilloscopeChart::setObservingChannelCursor_2(int index)
{
    if(index == 0){
        m_cursor_2->setObservedDataSeries(Channel::NONE);
        m_cursor_2->setVisible(false);
        updateCursorsData();
        return;
    }
    if(index == 1){
        m_cursor_2->setObservedDataSeries(Channel::CH_1, m_oscilloscopeSeries_Ch1);
    }
    else{
        m_cursor_2->setObservedDataSeries(Channel::CH_2, m_oscilloscopeSeries_Ch2);
    }
    m_cursor_2->setVisible(true);
    updateCursorsData();
}

///
/// Funkcja aktualizująca wyświtlanie kursorów, wykorzystywana gdy pozycja kursorów pozostaje niezmieniona ale
/// zmieniają się wartości sugnałów któRe obserwóje, funkcja ta informuje kursory o zmianie wartości
///
void OscilloscopeChart::updateCursorsData()
{
    CursorData dataCursor_1 = m_cursor_1->cursorData();
    CursorData dataCursor_2 = m_cursor_2->cursorData();

    if(dataCursor_1.observedChannel == Channel::CH_1){
        dataCursor_1 = correctCursorData(dataCursor_1, m_verticalDivCh_1, m_verticalOffsetCh_1);
    }
    else if(dataCursor_1.observedChannel == Channel::CH_2){
        dataCursor_1 = correctCursorData(dataCursor_1, m_verticalDivCh_2, m_verticalOffsetCh_2);
    }
    if(dataCursor_2.observedChannel == Channel::CH_1){
        dataCursor_2 = correctCursorData(dataCursor_2, m_verticalDivCh_1, m_verticalOffsetCh_1);
    }
    else if(dataCursor_2.observedChannel == Channel::CH_2){
        dataCursor_2 = correctCursorData(dataCursor_2, m_verticalDivCh_2, m_verticalOffsetCh_2);
    }
    emit cursorsDataUpdated(dataCursor_1, dataCursor_2);
}



