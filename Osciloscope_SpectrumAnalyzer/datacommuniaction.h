#ifndef DATACOMMUNIACTION_H
#define DATACOMMUNIACTION_H

#include <QObject>
//#include <QtSerialPort/QSerialPort>
//#include <QtSerialPort/QSerialPortInfo>
#include "serialportsettings.h"
#include <QString>

#include "sample.h"

///
/// \brief klasa odpowiadjąca za komunikację z mikrokontrolwerem poprzez port szeregowy.
///
/// Klasa stale odbiera kolejne przychodzące paczki z danymi pomiarowymi przez port szeregowy, określa warunek wyzwolenia
/// i zapisuje koljene próbki w ospowiednie struktury o odpowieniej długości. Zarządza komunikacją i archowizacją pomiarów w aplikacji.
///
class DataCommuniaction : public QObject
{
    Q_OBJECT

    QString m_currentBuffor;
    QSerialPort m_port;

    ///
    /// \brief Przekształca łańcuch znaków pobrany z portu szeregowego i wyciąga z niego wartości
    ///
    bool parseDataPackage(QString package);

    bool m_isDisabled;
    Channel m_observedChannels;
    Channel m_triggerSource;
    Slope m_triggerSlope;
    float m_triggerLevel;   // określone w woltach

    Sample m_prevSampleCh_1;
    Sample m_prevSampleCh_2;

    ///
    /// \brief processSamples - przekształca odebrane wartości na próbki gotowe do dalszego przetwarzania
    ///
    void processSamples(int rawValueCh_1, int rawValueCh_2, int deltaTime);
    ///
    /// \brief tirggerConditionMet - Funkcja oceniająca czy spęłniony został warunek wyzwolenia
    ///
    bool tirggerConditionMet(Sample currentSample_Ch1, Sample currentSample_Ch2);

    QList<Sample> m_dataListCh_1;
    QList<Sample> m_dataListCh_2;

    bool m_isRecoding;
    int m_timeSinceBeginRec;    // w mikrosekundach
    int m_frameRecordingTime;   // Ile czasu miesci się w jednej sesji pomiarowej w mikrosekundach

    int m_maxVoltValue;
    int m_maxRawValue;

public:
    explicit DataCommuniaction(QObject *parent = nullptr);
    ///
    /// \brief setPortConfiguration - Ustawienie konfiguracji portu szeregowego
    ///
    void setPortConfiguration(uint baudRate,
                              QSerialPort::DataBits dataBits,
                              QSerialPort::Parity parity,
                              QSerialPort::StopBits stopBits);
    ///
    /// \brief setPort - Określa z którego portu będą pobierane dane
    ///
    void setPort(QSerialPortInfo serialPortInfo);
    ///
    /// \brief startConnection - rozpocznuma połączenie i odbieranie paczek z portu szeregowego
    ///
    void startConnection();
    ///
    /// \brief stopConnection - kończy połączenie i odbieranie paczek z portu szeregowego
    ///
    void stopConnection();
    ///
    /// \brief setObservedChannels - funkcja wymuszająca tryb pomiaru sygnałów
    ///
    void setObservedChannels(Channel observedChannels);
    ///
    /// \brief setDisabled - Wstrzymuje pobieranie nowych paczek
    ///
    void setDisabled();
    ///
    /// \brief setEnabled - Wznawia pobieranie nowych paczek
    ///
    void setEnabled();

    int timeBase();

signals:
    void newDataFrameReady(QList<Sample> dataListCh_1, QList<Sample> dataListCh_2);

public slots:
    ///
    /// \brief dataReady - slot wyzwalanay gdy dostępne są do odebrania nowe dane na porcie szeregowym
    ///
    void dataReady();
    ///
    /// \brief setTriggerSlope - slot pozwaljący wybrać rodzaj zbocza wyzwaljącego
    ///
    void setTriggerSlope(int index);
    ///
    /// \brief setTriggerLevel - slot pozwaljący wybrać poziom napiecia wyzwolenia
    ///
    void setTriggerLevel(double voltageLevel);
    ///
    /// \brief setTriggerSource - slot pozwaljący wybrać kanał wyzwaljący
    ///
    void setTriggerSource(int index);
    ///
    /// \brief setTimeBase - slot pozwaljący wybrać podstawę czasową i długość jednej sesji pobierania danych
    ///
    void setTimeBase(QString timeBase);
};

#endif // DATACOMMUNIACTION_H
