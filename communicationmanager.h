    #ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
/*
#include "sample.h"

enum Channel {CH_1, CH_2};
enum Slope {RISING, FALLING};

/// Klasa managera komunikacji
///
/// Klasa zajmuje się komunikacją z mikrokontrolerem i zapisywaniem nieprzetworzonych danych w strukturach
class CommunicationManager : public QObject
{
    Q_OBJECT

    QString m_currentBuffor;
    QSerialPort m_port;

    int m_frameSize;

    QList<RelativeSample> m_Ch1rawDataBuffor;
    QList<RelativeSample> m_Ch2rawDataBuffor;

    QList<RelativeSample> m_Ch1rawDataList;
    QList<RelativeSample> m_Ch2rawDataList;

    int m_timeBase_us;
    Slope m_triggerSlope;
    float m_triggerVoltageLevel;

    /// Funkcja wyodrębniająca wartości poiarowe z ciągu znaków dla jednej paczki pomiarowej
    /// @param package to ciąg znaków jednej paczki czyli od znaku '#'
    /// @return funkcja zwraca 1 w przypadku popraenego doania nowego pomiaru lub 0
    /// w przypadku niepowodzenia
    int parseDataPackage(QString package);

public:
    /// Konstruktor klasy
    /// @param sessionBufforSize określa ilość pobieranych pomiarów do aktualizacji wykresu
    explicit CommunicationManager(QObject *parent = nullptr,
                                  int sessionBufforSize = 100);
    /// Ustawienie konfiguracji komuniakcji portu z mikrokontrolerem
    /// @param baudRate to szybkość komunikacji
    /// @param dataBits to ilość bitów  wramce
    /// @param parity to opcja czy ma pojawić się bit parzystości
    void setPortConfiguration(uint baudRate,
                              QSerialPort::DataBits dataBits,
                              QSerialPort::Parity parity,
                              QSerialPort::StopBits stopBits);
    /// Ustawineie portu z który ma być wykorzystywany do odczytu danych
    /// @param serialPortInfo to klasa zawierająca informacje o porcie który ma zostać użyty
    void setPort(QSerialPortInfo serialPortInfo);
    /// Rozpoczyna połąćzenie z mikrokontrolerem na porcie szeregowym
    void startConnection();
    /// Kończy połąćzenie z mikrokontrolerem na porcie szeregowym
    void stopConnection();
    /// Zwraca listę z aktualnymi wartościami sesji dla kanału 1
    QList<RelativeSample> getRawDataList(Channel channel);
signals:
    /// Sygnał nadawany w momencie zebranie nowej sesji danych o określonej wartośći
    /// @see sessionBufforSize
    void newDataSessionReady();

public slots:
    /// Slot wywoływany gdy na porcie szeregowym pojawią się nowe przychodzące dane
    ///
    /// Zapisuje je do bufora, dzieli przychodzące dane na poszczególne paczki danych
    /// i przekazuje je do funkcji wyodrębniającej właściwe wartości z ciągu znakóW
    void dataReady();
    /// ustawia tryb działania mikrokontrolera
    ///
    /// 1 - wysyłaj tylko z kanału 1 <br>
    /// 2 - wysyłaj tylko z kanału 2 <br>
    /// 3 - wysyłąj z obu kanałów jednoczesnie
    void setChannelMode();
};*/

#endif // COMMUNICATIONMANAGER_H
