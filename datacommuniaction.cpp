#include <QtDebug>
#include "datacommuniaction.h"

#define FRAME_TIME_BASES_COUNT 10

///
/// \param package - łańcuch znaków odebrany z portu szeregowego
/// \return zwraca wrtość bool, 1 - gdy odebrano poprawną paczkę danych lub 0 w przeciwnym razie
///
bool DataCommuniaction::parseDataPackage(QString package)
{
    if(package.length() < 5) return false;
    if(package.at(1) != ':') return false;

    QChar packageType = package.at(0);
    package.remove(0, 2);
    QStringList packageValues = package.split('/');

    int rawValueCh_1 = 0;
    int rawValueCh_2 = 0;
    int deltaTime = 0;

    if(packageType == '1')
    {
        if(packageValues.size() != 2) return false;
        rawValueCh_1 = packageValues[0].toInt();
        deltaTime = packageValues[1].toInt();
    }
    else if(packageType == '2')
    {
        if(packageValues.size() != 2) return false;
        rawValueCh_2 = packageValues[0].toInt();
        deltaTime = packageValues[1].toInt();
    }
    else if(packageType == '3')
    {
        if(packageValues.size() != 3) return false;
        rawValueCh_1 = packageValues[0].toInt();
        rawValueCh_2 = packageValues[1].toInt();
        deltaTime = packageValues[2].toInt();
    }

    processSamples(rawValueCh_1, rawValueCh_2, deltaTime);
}

///
/// \param rawValueCh_1 - wartość napięcia (0-1024) z kanału 1
/// \param rawValueCh_2 - wartość napięcia (0-1024) z kanału 2
/// \param deltaTime - czas w mikrosekundach od poprzedniej paczki danych
///
void DataCommuniaction::processSamples(int rawValueCh_1, int rawValueCh_2, int deltaTime){
    if(m_observedChannels == Channel::NONE) return;
    if(m_isDisabled) return;

    m_timeSinceBeginRec += deltaTime;
    Sample sample_Ch1(rawValueCh_1, m_maxRawValue, m_maxVoltValue, m_timeSinceBeginRec);
    Sample sample_Ch2(rawValueCh_2, m_maxRawValue, m_maxVoltValue, m_timeSinceBeginRec);

    if(m_isRecoding){
        if(m_observedChannels == Channel::CH_1 || m_observedChannels == Channel::CH_1_AND_CH_2)
            m_dataListCh_1.push_back(sample_Ch1);
        if(m_observedChannels == Channel::CH_2 || m_observedChannels == Channel::CH_1_AND_CH_2)
            m_dataListCh_2.push_back(sample_Ch2);

        if(m_timeSinceBeginRec >= m_frameRecordingTime){
            m_isRecoding = false;
            emit newDataFrameReady(m_dataListCh_1, m_dataListCh_2);
        }
    }
    else{
        if(tirggerConditionMet(sample_Ch1, sample_Ch2)){
            m_isRecoding = true;
            m_timeSinceBeginRec = 0;
            m_dataListCh_1.clear();
            m_dataListCh_2.clear();
            sample_Ch1.time = 0;
            sample_Ch2.time = 0;
            m_dataListCh_1.push_back(sample_Ch1);
            m_dataListCh_2.push_back(sample_Ch2);
        }
    }
    m_prevSampleCh_1 = sample_Ch1;
    m_prevSampleCh_2 = sample_Ch2;
}

///
/// \param currentSample_Ch1 - ostatnia próbka pobrana z kanału 1
/// \param currentSample_Ch2 - ostatnia próbka pobrana z kanału 2
/// \return zwraca wartość bool (1 w przypadku powodzenia)
///
bool DataCommuniaction::tirggerConditionMet(Sample currentSample_Ch1, Sample currentSample_Ch2)
{
    Sample prevSourceSample    = (m_triggerSource == Channel::CH_1 ? m_prevSampleCh_1 : m_prevSampleCh_2);
    Sample currentSourceSample = (m_triggerSource == Channel::CH_1 ? currentSample_Ch1 : currentSample_Ch2);

    if(m_triggerSlope == Slope::RISING){
        if(prevSourceSample.value < m_triggerLevel &&
           currentSourceSample.value >= m_triggerLevel){
            //prevSourceSample = currentSourceSample;
            return true;
        }
    }
    else{
        if(prevSourceSample.value > m_triggerLevel &&
           currentSourceSample.value <= m_triggerLevel){
            //prevSourceSample = currentSourceSample;
            return true;
        }
    }
    return false;
}

DataCommuniaction::DataCommuniaction(QObject *parent)
    : QObject{parent}
{
    m_isRecoding = false;
    m_timeSinceBeginRec = 0;

    m_maxRawValue = 1024;
    m_maxVoltValue = 5.0;

    m_triggerLevel = 2;
    m_triggerSlope = Slope::RISING;
    m_triggerSource = Channel::CH_1;

    m_frameRecordingTime = 1000 * 1000;
    m_observedChannels = Channel::CH_1;

    QObject::connect(&m_port, &QSerialPort::readyRead, this, &DataCommuniaction::dataReady);
}

///
/// \param baudRate - ilosć bitów na sekundę (prędkość komunikacji)
/// \param dataBits - ilość bitów w ramce
/// \param parity - Tryb parzystośći
/// \param stopBits - ilość bitów stopu
///
void DataCommuniaction::setPortConfiguration(uint baudRate,
                                         QSerialPort::DataBits dataBits,
                                         QSerialPort::Parity parity,
                                         QSerialPort::StopBits stopBits)
{
    m_port.setBaudRate(baudRate);
    m_port.setDataBits(dataBits);
    m_port.setParity(parity);
    m_port.setStopBits(stopBits);
}

///
/// \param serialPortInfo - port z któRrego będą pobierane dane
///
void DataCommuniaction::setPort(QSerialPortInfo serialPortInfo)
{
    m_port.setPort(serialPortInfo);
}

void DataCommuniaction::startConnection()
{
    m_port.open(QIODevice::ReadWrite);
}

void DataCommuniaction::stopConnection()
{
    m_port.close();
}

///
/// \param observedChannels - jaki kanał/kanały mają być aaktualnie obserwowane i odbierane dane
///
/// 0 - żaden
/// 1 - Kanał 1
/// 2 - kanał 2
/// 3 - oba kanały
///
void DataCommuniaction::setObservedChannels(Channel observedChannels)
{
    m_isRecoding = false;
    m_observedChannels = observedChannels;
    char message[2];

    int mode;
    switch (m_observedChannels) {
    case Channel::NONE:
        mode = 0;
        break;
    case Channel::CH_1:
        mode = 1;
        break;
    case Channel::CH_2:
        mode = 2;
        break;
    case Channel::CH_1_AND_CH_2:
        mode = 3;
        break;
    }

    sprintf(message, "#%d", mode);
    m_port.write(message);

    if(m_observedChannels == Channel::NONE) qDebug() << "None chnnel";
    if(m_observedChannels == Channel::CH_1) qDebug() << "chnnel CH1";
    if(m_observedChannels == Channel::CH_1_AND_CH_2) qDebug() << "chnnel CH2 i CH1";
    if(m_observedChannels == Channel::CH_2) qDebug() << "chnnel CH2";
}

void DataCommuniaction::dataReady()
{
    QByteArray temp = m_port.readAll();
    m_currentBuffor.append(QString(temp));

    QStringList packageList = m_currentBuffor.split('#');
    if(packageList.size() == 0)
    {
        m_currentBuffor.clear();
        return;
    }
    QString lastPackage = packageList.last();
    packageList.removeLast();

    foreach (QString packageString, packageList)
    {
        parseDataPackage(packageString);
    }

    m_currentBuffor.clear();
    m_currentBuffor.append('#');
    m_currentBuffor.append(lastPackage);
}

///
/// \param index - indeks zbocza z lizty (0 - Narastające, 1 - Opadające)
///
void DataCommuniaction::setTriggerSlope(int index){
    m_triggerSlope = index == 0 ? Slope::RISING : Slope::FALLING;
}
///
/// \param voltageLevel - napięcie wyzwolenia
///
void DataCommuniaction::setTriggerLevel(double voltageLevel){
    m_triggerLevel = voltageLevel;
}

///
/// \param index - indeks zbocza z lizty (0 - kanał 1, 1 - kanał 2)
///
void DataCommuniaction::setTriggerSource(int index){
    m_triggerSource = index == 0 ? Channel::CH_1 : Channel::CH_2;
}

///
/// \param timeBase - łancuch znaków podstawy czasowej
/// \warning przyjmuje jedynie format [val] ms/div lub [val] us/div
///
void DataCommuniaction::setTimeBase(QString timeBase)
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
        m_frameRecordingTime = FRAME_TIME_BASES_COUNT * textNumber * 1000;
    }
    else{
        m_frameRecordingTime = FRAME_TIME_BASES_COUNT * textNumber;
    }
}

void DataCommuniaction::setDisabled() {m_isDisabled = true;}
void DataCommuniaction::setEnabled() {m_isDisabled = false; m_isRecoding = false;}

int DataCommuniaction::timeBase()
{
    return (float)m_frameRecordingTime / (float)FRAME_TIME_BASES_COUNT;
}




