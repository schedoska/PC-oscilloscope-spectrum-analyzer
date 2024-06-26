#ifndef CURSOR_H
#define CURSOR_H

#include <QGraphicsObject>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include "cursordata.h"

using namespace QtCharts;

///
/// \brief Klasa kursora pozwaljącego wykonywać pomiary sygnału określonych chwil czasu
///
/// Kursor jest wyświetlany na obszarze oscyloskopu i może być dowolnie przesuwany wzdłóż osi X,
/// Każde przesunięcie będzie skutkowało oszacowaniem wartości napięcia w danej chwili czasu za pomocą funkcji interpolacji próbki
/// oraz wysłaniem sygnału informującym o aktualizacji danych
///
class Cursor : public QGraphicsObject
{
    Q_OBJECT

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    Cursor(QChart *oscilloscopeChart);
    ///
    /// \brief setObservedDataSeries - funkcja pozwala ustawić kanał jaki jest aktualnie obserwowany przez kursor
    ///
    void setObservedDataSeries(Channel observedChannel ,
                               QLineSeries *observedDataSeries = nullptr);
    ///
    /// \brief funkcja pozwaljąca wyznaczyć wartość pomiędzy próbkami za pomocą intrpolacji
    ///
    float interpolateVaule(QPointF A, QPointF B, float time);
    ///
    /// \brief cursorData - funkcja zwracająca zmienną zawierającą informacje o danych pomiarowych z kursora
    ///
    CursorData cursorData();
    ///
    /// \brief aktualizuje dane kursora
    ///
    void updateCursorData();

private:
    int m_height;
    int m_width;
    int m_sideMarginSize;

    CursorData m_cursorData;
    QLineSeries *m_observedDataSeries;
    QChart *m_oscilloscopeChart;
    QValueAxis *m_xAxis;

    ///
    /// \brief mousePressEvent - zaimplementowana funkcja eventu naciśnięcia klawisza myszki
    ///
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    ///
    /// \brief mouseMoveEvent - zaimplementowana funkcja eventu poruszania kursorem myszki
    ///
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void cursorDataUpdated();
};

#endif // CURSOR_H
