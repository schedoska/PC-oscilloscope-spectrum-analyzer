#include "portsettingsdialog.h"
#include <QFormLayout>
#include <QLayout>
#include <QDebug>

///
/// \brief Fubckja wypełniająca możliwe opcje do wyboru w obiektach typu QComboBox.
///
void PortSettingsDialog::fillCobmboBoxOptions()
{
    m_selectPortComboBox->clear();
    m_serialPortInfoList =  QSerialPortInfo::availablePorts();
    for(int i = 0; i < m_serialPortInfoList.count(); i++){
        m_selectPortComboBox->addItem(m_serialPortInfoList[i].portName(), i);
    }

    m_selectDataBitsComboBox->addItem(QObject::tr("5 bitów"), 5);
    m_selectDataBitsComboBox->addItem(QObject::tr("6 bitów"), 6);
    m_selectDataBitsComboBox->addItem(QObject::tr("7 bitów"), 7);
    m_selectDataBitsComboBox->addItem(QObject::tr("8 bitów"), 8);

    m_selectParityComboBox->addItem(QObject::tr("Brak bitu parzystości"));
    m_selectParityComboBox->addItem(QObject::tr("sprawdzanie parzystości"));
    m_selectParityComboBox->addItem(QObject::tr("sprawdzanie nieparzystości"));

    m_selectSpeedComboBox->addItem(QString::number(1200));
    m_selectSpeedComboBox->addItem(QString::number(2400));
    m_selectSpeedComboBox->addItem(QString::number(4800));
    m_selectSpeedComboBox->addItem(QString::number(9600));
    m_selectSpeedComboBox->addItem(QString::number(19200));
    m_selectSpeedComboBox->addItem(QString::number(38400));
    m_selectSpeedComboBox->addItem(QString::number(57600));
    m_selectSpeedComboBox->addItem(QString::number(115200));

    m_selectStopBitsComboBox->addItem(QObject::tr("1 bit stopu"));
    m_selectStopBitsComboBox->addItem(QObject::tr("2 bity stopu"));
}

PortSettingsDialog::PortSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QObject::tr("Konfiguracja komunikacji"));
    setFixedSize(400, 300);

    setAttribute(Qt::WA_DeleteOnClose, true);

    m_selectPortLabel = new QLabel(QObject::tr("Port:"), this);
    m_selectSpeedLabel = new QLabel(QObject::tr("Szybkość komunikacji:"), this);
    m_selectDataBitsLabel = new QLabel(QObject::tr("Ilość bitów"), this);
    m_selectParityLabel = new QLabel(QObject::tr("bit parzystości"), this);
    m_selectStopBitsLabel = new QLabel(QObject::tr("Ilość bitów stopu"), this);

    m_selectPortComboBox = new QComboBox(this);
    QObject::connect(m_selectPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setPort(int)));
    m_selectSpeedComboBox = new QComboBox(this);
    QObject::connect(m_selectSpeedComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setSpeed(QString)));
    m_selectDataBitsComboBox = new QComboBox(this);
    QObject::connect(m_selectDataBitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDataBits(int)));
    m_selectParityComboBox = new QComboBox(this);
    QObject::connect(m_selectParityComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setParity(int)));
    m_selectStopBitsComboBox = new QComboBox(this);
    QObject::connect(m_selectStopBitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setStopBits(int)));

    m_doneButton = new QPushButton(QObject::tr("Zatwierdź"), this);
    QObject::connect(m_doneButton, SIGNAL(released()), this, SLOT(close()));

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(m_selectPortLabel, m_selectPortComboBox);
    formLayout->addRow(m_selectSpeedLabel, m_selectSpeedComboBox);
    formLayout->addRow(m_selectDataBitsLabel, m_selectDataBitsComboBox);
    formLayout->addRow(m_selectParityLabel, m_selectParityComboBox);
    formLayout->addRow(m_selectStopBitsLabel, m_selectStopBitsComboBox);
    formLayout->setLabelAlignment(Qt::AlignRight);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addLayout(formLayout);
    vBoxLayout->addWidget(m_doneButton);
    setLayout(vBoxLayout);

    fillCobmboBoxOptions();
    setDefaultValues();
}

///
/// Event który jest wyzwalaany przy zamknięciu okna, powoduje wywyołanie tej funkcji przed dokonaniem zamknięcia.
/// Funkcja emituje sygnał informujący o zminaie konfigutacji komunikacji z mikrokontolerem
///
void PortSettingsDialog::closeEvent(QCloseEvent *event)
{
    close();
    qDebug() << m_currentPortSettings.serialPortInfo.portName() << m_currentPortSettings.dataBits
             << m_currentPortSettings.parity << m_currentPortSettings.baudRate << m_currentPortSettings.stopBits;
    emit portSettingsUpdated(m_currentPortSettings);
}

///
/// Funkcja ustawia domyślne wartości w każdej liście wyboru. Na tych ustawieniach jest róWnież skonfigurowany mikrokontroler
///
void PortSettingsDialog::setDefaultValues()
{
    m_selectDataBitsComboBox->setCurrentIndex(3);
    m_selectSpeedComboBox->setCurrentIndex(7);
    m_selectParityComboBox->setCurrentIndex(0);
    m_selectStopBitsComboBox->setCurrentIndex(0);
}

///
/// \param index - określa numer wybranego portu z listy dostępnych portów szeregowych
///
void PortSettingsDialog::setPort(int index)
{
    m_currentPortSettings.serialPortInfo = m_serialPortInfoList.at(m_selectPortComboBox->currentData().toInt());
}

///
/// \param index - określa numer wybranej prędkości komunikacji z listy
///
void PortSettingsDialog::setDataBits(int index)
{
    switch(m_selectDataBitsComboBox->currentData().toInt()){
    case 5:
        m_currentPortSettings.dataBits = QSerialPort::Data5;
        break;
    case 6:
        m_currentPortSettings.dataBits = QSerialPort::Data6;
        break;
    case 7:
        m_currentPortSettings.dataBits = QSerialPort::Data7;
        break;
    default:
        m_currentPortSettings.dataBits = QSerialPort::Data8;
        break;
    }
}

///
/// \param index - określa numer wybranej opcji dotyczącej ilości bitów parzystość
///
void PortSettingsDialog::setParity(int index)
{
    switch(index){
    case 0:
        m_currentPortSettings.parity = QSerialPort::NoParity;
        break;
    case 1:
        m_currentPortSettings.parity = QSerialPort::EvenParity;
        break;
    case 2:
        m_currentPortSettings.parity = QSerialPort::OddParity;
        break;
    }
}

void PortSettingsDialog::setSpeed(QString baudRateText)
{
    m_currentPortSettings.baudRate = baudRateText.toUInt();
}

void PortSettingsDialog::setStopBits(int index)
{
    if(index == 0) m_currentPortSettings.stopBits = QSerialPort::OneStop;
    else m_currentPortSettings.stopBits = QSerialPort::TwoStop;
}

