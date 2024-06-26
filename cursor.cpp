#include "cursor.h"
#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QtDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

QRectF Cursor::boundingRect() const
{
    return QRectF(-15,5,30,1200);
}

void Cursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(Qt::white);
    QBrush brush(Qt::white);
    pen.setWidth(m_width);
    pen.setStyle(Qt::DashLine);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawLine(0,0,0,m_height);
    painter->drawEllipse(-3,m_oscilloscopeChart->mapToPosition(QPointF(m_cursorData.time,m_cursorData.value)).y(),6,6);
}

Cursor::Cursor(QChart *oscilloscopeChart)
{
    m_width = 2;
    m_height = 1000;
    m_sideMarginSize = 30;

    m_oscilloscopeChart = oscilloscopeChart;
    m_xAxis = dynamic_cast<QValueAxis*>(oscilloscopeChart->axes(Qt::Horizontal)[0]);
}

///
/// Funkcja przydziela kursorowi wskazany kanał i serię danych na podstawie których dokonuje pomiarów
/// \param observedChannel - wskazany kanał do obserwacji
/// \param observedDataSeries - seria danych wskazanyego kanału
///
void Cursor::setObservedDataSeries(Channel observedChannel ,QLineSeries *observedDataSeries)
{
    m_observedDataSeries = observedDataSeries;
    m_cursorData.observedChannel = observedChannel;
}

///
/// Funkcja aktualizuje informacje o wartości chwilowej sygnału we wskazanym przez kursor punkcie
///
void Cursor::updateCursorData()
{
    if(m_cursorData.observedChannel == Channel::NONE || m_observedDataSeries->count() < 2) return;
    m_cursorData.time = m_oscilloscopeChart->mapToValue(QPointF(pos().x(),0)).x();

    unsigned closestValueIndex = (m_cursorData.time / m_xAxis->max()) * m_observedDataSeries->count();
    if(closestValueIndex > (unsigned)m_observedDataSeries->count() - 1 || closestValueIndex < 1) return;

    m_cursorData.value = interpolateVaule(m_observedDataSeries->at(closestValueIndex - 1),
                                          m_observedDataSeries->at(closestValueIndex),
                                          m_cursorData.time);
    emit cursorDataUpdated();
}

///
/// Funkcja dokonuje interpolacji liniowej na podstawie dwóch punktów i pozwala wyzncaczyć wartość dowlonego
/// punktu we wskazanej chwili pomiędzy podanymi próbkami
/// \param A próbka z wcześniejszej chwili
/// \param B próbka z późniejszej chwili
/// \param time chwila któRej wartość chce się uzyskać
/// \return oszacowana wartość
///
float Cursor::interpolateVaule(QPointF A, QPointF B, float time)
{
    float line_a = (B.y() - A.y()) / (B.x() - A.x());
    float line_b = A.y() - line_a * A.x();
    return line_a * time + line_b;
}

///
/// \return Funkcja zwraca strukturę z informacji o lokalnych własnościah sygnału
///
CursorData Cursor::cursorData()
{
    return m_cursorData;
}

void Cursor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void Cursor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(scene()->views().size() == 0) return;

    if(event->buttons() == Qt::LeftButton){
        int xPos = std::min((int)event->scenePos().x(), scene()->views()[0]->width() - m_sideMarginSize);
        xPos = std::max(xPos, m_sideMarginSize);
        setPos(xPos,0);
    }
    updateCursorData();
    update();
}

