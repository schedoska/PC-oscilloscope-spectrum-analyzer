
#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

///
/// \brief SerialPortSettings - struktura zawierająca informacje konieczne do poporawneg oskonfigurowania komunikacji szergowej.
/// \param serialPortInfo to klasa zawierająca informacje na temat portu z któRym powinno zostać nawiązane połaćzneine
/// \param dataBits informajca na temat ilośći bitów z jakij skąłda się jedna rmaka komunkiacji UART
/// \param parity informacja dotycząca sposobu wykreślanai bitu parzystości wykorzystywanego przy komunikacji
/// \param stopBits informacja o ilości bitów stopu
/// \param baudRate szybkość komuikacji wyrażona w bitahc na sekunde
///
struct SerialPortSettings{
    QSerialPortInfo serialPortInfo;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    unsigned baudRate;
};

#endif // SERIALPORTSETTINGS_H
