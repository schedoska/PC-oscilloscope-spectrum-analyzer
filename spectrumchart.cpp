#include "spectrumchart.h"
#include <QGraphicsLayout>
#include <QDebug>

SpectrumChart::SpectrumChart(QChartView *chartView ,QObject *parent)
    : QObject{parent}, m_chartVeiw(chartView)
{
    m_numberOfHorizontalTicks = 9;
    m_numberOfVerticalTicks = 11;

    m_spectrumSeries = new QLineSeries();

    m_spectrumChart = new QChart();
    m_chartVeiw->setChart(m_spectrumChart);

    m_spectrumChart->addSeries(m_spectrumSeries);
    m_spectrumChart->legend()->hide();
    m_spectrumChart->setMargins(QMargins(2,12,12,2));

    m_spectrumChart->layout()->setContentsMargins(0,0,0,0);
    m_spectrumChart->setBackgroundRoundness(0);
    //m_spectrumChart->setBackgroundBrush(QBrush(Qt::black));
    //m_spectrumChart->setPlotAreaBackgroundPen(QPen(QColor(255,0,0)));
    m_spectrumChart->setTheme(QChart::ChartThemeDark);

    m_xAxis = new QValueAxis;
    m_xAxis->setRange(0,200);
    m_xAxis->setTickCount(m_numberOfVerticalTicks);
    //m_xAxis->setTickInterval(10);
    m_spectrumChart->addAxis(m_xAxis, Qt::AlignBottom);

    m_yAxis = new QValueAxis;
    m_yAxis->setRange(0,700);
    m_yAxis->setTickCount(m_numberOfHorizontalTicks);
    m_yAxis->setLabelsVisible(false);
    m_spectrumChart->addAxis(m_yAxis, Qt::AlignLeft);

    m_spectrumSeries->clear();
    m_spectrumSeries->attachAxis(m_xAxis);
    m_spectrumSeries->attachAxis(m_yAxis);

    m_observedChannels = Channel::CH_1;

    m_cursor_1 = new Cursor(m_spectrumChart);
    m_cursor_1->setObservedDataSeries(Channel::CH_1, m_spectrumSeries);
    m_cursor_1->setPos(100,0);
    chartView->scene()->addItem(m_cursor_1);
    QObject::connect(m_cursor_1, SIGNAL(cursorDataUpdated()), this, SLOT(updateCursorsData()));

    m_cursor_2 = new Cursor(m_spectrumChart);
    m_cursor_2->setObservedDataSeries(Channel::CH_1, m_spectrumSeries);
    m_cursor_2->setPos(100,0);
    chartView->scene()->addItem(m_cursor_2);
    QObject::connect(m_cursor_2, SIGNAL(cursorDataUpdated()), this, SLOT(updateCursorsData()));

    m_observedChannels = Channel::NONE;
}

///
/// \param dataListCh_1 - Lista struktur FrquencySample zawierjacych informacje o wartościach próbek częstotlwioaści i mocy składowych koniecznych
/// \param dataListCh_2 - Lista struktur FrquencySample zawierjacych informacje o wartościach próbek częstotlwioaści i mocy składowych koniecznych
///
void SpectrumChart::updateChart(QList<FrequencySample> dataListCh_1, QList<FrequencySample> dataListCh_2)
{
    m_spectrumSeries->clear();

    if(m_observedChannels == Channel::CH_1){
        foreach(FrequencySample sample, dataListCh_1){
            if(sample.frequency > m_frequencyRange) break;
            m_spectrumSeries->append(sample.frequency, sample.value);
            m_spectrumSeries->setColor(QColor(SPECTRUM_COLOR_RGB_CHANNEL_1));
        }
    }
    else if(m_observedChannels == Channel::CH_2){
        foreach(FrequencySample sample, dataListCh_2){
            if(sample.frequency > m_frequencyRange) break;
            m_spectrumSeries->append(sample.frequency, sample.value);
            m_spectrumSeries->setColor(QColor(SPECTRUM_COLOR_RGB_CHANNEL_2));
        }
    }

    m_cursor_1->updateCursorData();
    m_cursor_2->updateCursorData();
}

unsigned int SpectrumChart::frequencyRange()
{
    return m_frequencyRange;
}

Channel SpectrumChart::observedChannels()
{
    return m_observedChannels;
}

///
/// \param index to numer wybranego indeksu z elementu graficznego ComboBox umożliwiającego wybór trybu wyświetlanai kursora 1 (aktywny lub nie)
/// 0 - aktywny, 1 - wyłaczony
///
void SpectrumChart::setEnableCursor1(int index)
{
    if(index == 0){
        m_cursor_1->setVisible(true);
        m_cursor_1->setObservedDataSeries(Channel::CH_1_AND_CH_2, m_spectrumSeries);
    }
    else{
        m_cursor_1->setVisible(false);
        m_cursor_1->setObservedDataSeries(Channel::NONE);
    }
    updateCursorsData();
}

///
/// \param index to numer wybranego indeksu z elementu graficznego ComboBox umożliwiającego wybór trybu wyświetlanai kursora 2 (aktywny lub nie)
/// 0 - aktywny, 1 - wyłaczony
///
void SpectrumChart::setEnableCursor2(int index)
{
    if(index == 0){
        m_cursor_2->setVisible(true);
        m_cursor_2->setObservedDataSeries(Channel::CH_1_AND_CH_2, m_spectrumSeries);
    }
    else{
        m_cursor_2->setVisible(false);
        m_cursor_2->setObservedDataSeries(Channel::NONE);
    }
    updateCursorsData();
}

///
/// \param index to numer wybranego indeksu z elementu graficznego ComboBOx umożliwiający wybóR aktualnie wyświoetlanego na wykresie kanału
/// 0 - Kanał 1, 1 - Kanał 2, (inne) - Nie wyświetlaj żadnego kanaału
///
void SpectrumChart::setObservedChannels(int index)
{
    if(index == 0){
        m_observedChannels = Channel::CH_1;
    }
    else if(index == 1){
        m_observedChannels = Channel::CH_2;
    }
    else m_observedChannels = Channel::NONE;

    m_spectrumSeries->clear();
}

void SpectrumChart::updateCursorsData()
{
    emit cursorsDataUpdated(m_cursor_1->cursorData(), m_cursor_2->cursorData());
}

///
/// \param rangeString to łańcuch znakowy zawierający informacje o wybraneym z elemntu graficznego ComboBox zakresu
///
void SpectrumChart::setFrequencyRange(QString rangeString)
{
    QStringList stringList = rangeString.split(' ');
    if(stringList.count() < 2) return;
    m_frequencyRange = stringList[0].toUInt();
    if(stringList[1][0] == 'k') m_frequencyRange *= 1000;
    m_xAxis->setMax(m_frequencyRange);
}















