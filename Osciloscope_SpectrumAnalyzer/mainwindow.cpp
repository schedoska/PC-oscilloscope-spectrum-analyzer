
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signalglobalinformation.h"
#include <QLabel>
#include <QLineSeries>
#include <QPushButton>

#define COLOR_RGB_DISABLED_STRING QString("186,186,186")
#define STYLESHEET_ENABLED_LABEL_STRING QString("color: black")
#define STYLESHEET_DISABLED_LABEL_STRING QString("color: grey")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("Oscyloskop i analizator widmowy"));

    /* Domyślne skonfigurowanie portu komunikacyjnego */
    if(QSerialPortInfo::availablePorts().size() > 0)
    {
        dataCommunication.setPort(QSerialPortInfo::availablePorts().at(0));
        dataCommunication.setPortConfiguration(115200, QSerialPort::Data8, QSerialPort::NoParity, QSerialPort::OneStop);
        dataCommunication.startConnection();
    }

    m_oscilloscopeChart = new OscilloscopeChart(ui->chart, this);
    m_spectrumChart = new SpectrumChart(ui->chart_spectrum, this);

    QObject::connect(&dataCommunication, SIGNAL(newDataFrameReady(QList<Sample>,QList<Sample>)),
                     this, SLOT(updateData(QList<Sample>,QList<Sample>)));

    QObject::connect(ui->tirggerSlopeComboBox, SIGNAL(currentIndexChanged(int)), &dataCommunication, SLOT(setTriggerSlope(int)));
    QObject::connect(ui->triggerLevelSpinBoc, SIGNAL(valueChanged(double)), &dataCommunication, SLOT(setTriggerLevel(double)));
    QObject::connect(ui->triggerSourceComboBox, SIGNAL(currentIndexChanged(int)), &dataCommunication, SLOT(setTriggerSource(int)));
    dataCommunication.setTriggerLevel(ui->triggerLevelSpinBoc->value());
    dataCommunication.setTriggerSource(ui->triggerSourceComboBox->currentIndex());
    dataCommunication.setTriggerSlope(ui->tirggerSlopeComboBox->currentIndex());

    QObject::connect(ui->timeBaseComboBox, SIGNAL(currentIndexChanged(QString)), m_oscilloscopeChart, SLOT(setTimeBase(QString)));
    QObject::connect(ui->timeBaseComboBox, SIGNAL(currentIndexChanged(QString)), &dataCommunication, SLOT(setTimeBase(QString)));
    m_oscilloscopeChart->setTimeBase(ui->timeBaseComboBox->currentText());

    QObject::connect(ui->ResumeButton, SIGNAL(pressed()), this, SLOT(setEnabled()));
    QObject::connect(ui->StopButton, SIGNAL(pressed()), this, SLOT(setDisabled()));

    ui->ResumeButton->pressed();

    QObject::connect(ui->channe1ActivePushButton, SIGNAL(pressed()), this, SLOT(changeActive_Ch1()));
    QObject::connect(ui->channe2ActivePushButton, SIGNAL(pressed()), this, SLOT(changeActive_Ch2()));

    m_isActive_Ch1 = false;
    m_isActive_Ch2 = false;
    updateObservedChannels();

    QObject::connect(ui->Cursor1_ComboBox, SIGNAL(currentIndexChanged(int)), m_oscilloscopeChart, SLOT(setObservingChannelCursor_1(int)));
    QObject::connect(ui->Cursor2_ComboBox, SIGNAL(currentIndexChanged(int)), m_oscilloscopeChart, SLOT(setObservingChannelCursor_2(int)));
    QObject::connect(m_oscilloscopeChart, SIGNAL(cursorsDataUpdated(CursorData,CursorData)), this, SLOT(updateCursorInfo(CursorData,CursorData)));
    m_oscilloscopeChart->setObservingChannelCursor_1(0);
    m_oscilloscopeChart->setObservingChannelCursor_2(0);
    updateData(QList<Sample>(), QList<Sample>());

    QObject::connect(ui->CH1_VerticalDiv, SIGNAL(currentIndexChanged(QString)), m_oscilloscopeChart, SLOT(setVerticalDivCh_1(QString)));
    QObject::connect(ui->CH1_VerticalOffset, SIGNAL(valueChanged(double)), m_oscilloscopeChart, SLOT(setVerticalOffsetCh_1(double)));
    QObject::connect(ui->CH2_VerticalDiv, SIGNAL(currentIndexChanged(QString)), m_oscilloscopeChart, SLOT(setVerticalDivCh_2(QString)));
    QObject::connect(ui->CH2_VerticalOffset, SIGNAL(valueChanged(double)), m_oscilloscopeChart, SLOT(setVerticalOffsetCh_2(double)));
    m_oscilloscopeChart->setVerticalOffsetCh_1(ui->CH1_VerticalOffset->value());
    m_oscilloscopeChart->setVerticalOffsetCh_2(ui->CH2_VerticalOffset->value());
    m_oscilloscopeChart->setVerticalDivCh_1(ui->CH1_VerticalDiv->currentText());
    m_oscilloscopeChart->setVerticalDivCh_2(ui->CH2_VerticalDiv->currentText());

    // ---------------------------------------------------------------------------------------------------------------------------- SPEKTRUM
    //
    // ---------------------------------------------------------------------------------------------------------------------------- SPEKTRUM

    QObject::connect(m_spectrumChart, SIGNAL(cursorsDataUpdated(CursorData,CursorData)),this, SLOT(updateCursorInfoSpectrum(CursorData,CursorData)));
    QObject::connect(ui->spectrum_cursor1_activeCombo, SIGNAL(currentIndexChanged(int)), m_spectrumChart, SLOT(setEnableCursor1(int)));
    QObject::connect(ui->spectrum_cursor2_activeCombo, SIGNAL(currentIndexChanged(int)), m_spectrumChart, SLOT(setEnableCursor2(int)));
    ui->spectrum_cursor1_activeCombo->setCurrentIndex(1);
    ui->spectrum_cursor2_activeCombo->setCurrentIndex(1);

    QObject::connect(ui->spectrumChannel_comboBox, SIGNAL(currentIndexChanged(int)), m_spectrumChart, SLOT(setObservedChannels(int)));
    ui->spectrumChannel_comboBox->setCurrentIndex(2);

    QObject::connect(ui->spectrumFrequencyBase_comboBox, SIGNAL(currentTextChanged(QString)), m_spectrumChart, SLOT(setFrequencyRange(QString)));
    ui->spectrumFrequencyBase_comboBox->setCurrentIndex(3);

    // ---------------------------------------------------------------------------------------------------------------------------- dialog
    //
    // ---------------------------------------------------------------------------------------------------------------------------- dialog

    QObject::connect(ui->menuPort, SIGNAL(aboutToShow()), this, SLOT(showPortSettingsDialog()));
    QObject::connect(ui->menuPomoc, SIGNAL(aboutToShow()), this, SLOT(showLanguageSettingsDialog()));
    m_translator = new QTranslator(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    dataCommunication.stopConnection();
}

///
/// Fununkcja wymuszająca zaktualizowanie i zsychronizowania między poszczególnymi modułami obserwowane kanały
///
void MainWindow::updateObservedChannels(){
    Channel observedChannel;
    if(m_isActive_Ch1 && m_isActive_Ch2) observedChannel = Channel::CH_1_AND_CH_2;
    else if(m_isActive_Ch1 && !m_isActive_Ch2) observedChannel = Channel::CH_1;
    else if(!m_isActive_Ch1 && m_isActive_Ch2) observedChannel = Channel::CH_2;
    else observedChannel = Channel::NONE;

    dataCommunication.setObservedChannels(observedChannel);
    m_oscilloscopeChart->setObservedChannels(observedChannel);

    setChannel_1_GUI_Enabled(m_isActive_Ch1);
    setChannel_2_GUI_Enabled(m_isActive_Ch2);
}

///
/// Aktywacja/dezaktywacja zakładki kontrolującej sposób wyświetlania sygnału z kanału 1
/// \param set - wartość bool określająca stan zakładki kontrolującej sygnał z kanału 1
///
void MainWindow::setChannel_1_GUI_Enabled(bool set)
{
    if(set){
        ui->CH1_settings->setStyleSheet("QFrame#CH1_settings { border: 3px solid rgb(" + COLOR_RGB_CHANNEL_1_STRING + "); }" +
                                        "QLabel { color: balck}");
        ui->CH1_VerticalDiv->setEnabled(true);
        ui->CH1_VerticalOffset->setEnabled(true);
        ui->channe1ActivePushButton->setText(QObject::tr("Wyłącz"));

        ui->CH1_signalInfo->setStyleSheet("QFrame#CH1_signalInfo { border: 3px solid rgb(" + COLOR_RGB_CHANNEL_1_STRING + "); }");
    }
    else{
        ui->CH1_settings->setStyleSheet("QFrame#CH1_settings { border: 3px solid rgb(" + COLOR_RGB_DISABLED_STRING + "); }" +
                                        "QLabel { color: grey; }");
        ui->CH1_VerticalDiv->setEnabled(false);
        ui->CH1_VerticalOffset->setEnabled(false);
        ui->channe1ActivePushButton->setText(QObject::tr("Włącz"));

        ui->CH1_signalInfo->setStyleSheet("QFrame#CH1_signalInfo { border: 3px solid rgb(" + COLOR_RGB_DISABLED_STRING + "); }" +
                                          "QLabel { color: grey; }");
    }
}

///
/// Aktywacja/dezaktywacja zakładki kontrolującej sposób wyświetlania sygnału z kanału 2
/// \param set - wartość bool określająca stan zakładki kontrolującej sygnał z kanału 2
///
void MainWindow::setChannel_2_GUI_Enabled(bool set)
{
    if(set){
        ui->CH2_settings->setStyleSheet("QFrame#CH2_settings { border: 3px solid rgb(" + COLOR_RGB_CHANNEL_2_STRING + "); }" +
                                        "QLabel { color: balck}");
        ui->CH2_VerticalDiv->setEnabled(true);
        ui->CH2_VerticalOffset->setEnabled(true);
        ui->channe2ActivePushButton->setText(QObject::tr("Wyłącz"));

        ui->CH2_signalInfo->setStyleSheet("QFrame#CH2_signalInfo { border: 3px solid rgb(" + COLOR_RGB_CHANNEL_2_STRING + "); }");
    }
    else{
        ui->CH2_settings->setStyleSheet("QFrame#CH2_settings { border: 3px solid rgb(" + COLOR_RGB_DISABLED_STRING + "); }" +
                                        "QLabel { color: grey; }");
        ui->CH2_VerticalDiv->setEnabled(false);
        ui->CH2_VerticalOffset->setEnabled(false);
        ui->channe2ActivePushButton->setText(QObject::tr("Włącz"));

        ui->CH2_signalInfo->setStyleSheet("QFrame#CH2_signalInfo { border: 3px solid rgb(" + COLOR_RGB_DISABLED_STRING + "); }" +
                                          "QLabel { color: grey; }");
    }
}

///
/// funkcja zmienjąca stan aktuwnosci kanału 1
/// \warning Funkcja zmienia stan cyklycznie, każde wywołanie powoduje przełączenie stanu na przeciwny
///
void MainWindow::changeActive_Ch1(){
    m_isActive_Ch1 = !m_isActive_Ch1;
    updateObservedChannels();
}

///
/// funkcja zmienjąca stan aktuwnosci kanału 2
/// \warning Funkcja zmienia stan cyklycznie, każde wywołanie powoduje przełączenie stanu na przeciwny
///
void MainWindow::changeActive_Ch2(){
    m_isActive_Ch2 = !m_isActive_Ch2;
    updateObservedChannels();
}

void MainWindow::setDisabled(){
    dataCommunication.setDisabled();
    ui->StopButton->setEnabled(false);
    ui->ResumeButton->setEnabled(true);
}

void MainWindow::setEnabled(){
    dataCommunication.setEnabled();
    ui->ResumeButton->setEnabled(false);
    ui->StopButton->setEnabled(true);
}

///
/// \param cursor1 - struktura z informacjami pomiarowymi z kursora 1
/// \param cursor2 - struktura z informacjami pomiarowymi z kursora 2
///
void MainWindow::updateCursorInfo(CursorData cursor1, CursorData cursor2)
{
    if(cursor1.observedChannel != Channel::NONE)  {
        ui->Cursor1_timeLabel->setText(QString::number(((float)cursor1.time),'f',3) + " ms");
        ui->Cursor1_valueLabel->setText(QString::number(((float)cursor1.value),'f',3)+ " V");
    }
    else{
        ui->Cursor1_timeLabel->setText("-");
        ui->Cursor1_valueLabel->setText("-");
    }
    if(cursor2.observedChannel != Channel::NONE)  {
        ui->Cursor2_timeLabel->setText(QString::number(((float)cursor2.time),'f',3) + " ms");
        ui->Cursor2_valueLabel->setText(QString::number(((float)cursor2.value),'f',3)+ " V");
    }
    else{
        ui->Cursor2_timeLabel->setText("-");
        ui->Cursor2_valueLabel->setText("-");
    }

    if(cursor1.observedChannel != Channel::NONE && cursor2.observedChannel != Channel::NONE){
        float deltaTime = std::abs(cursor1.time-cursor2.time);
        float deltaValue = std::abs(cursor1.value-cursor2.value);
        ui->CursorsDeltaTime->setText(QString::number((float)deltaTime, 'f', 3) + " ms");
        ui->CursorsDeltaValue->setText(QString::number((float)deltaValue, 'f', 3) + " V");
        ui->CursorsDiff->setText(QString::number((float)(deltaValue/(deltaTime / 1000.0)), 'f', 3) + " V/s");
        ui->CursorsDeltaFreq->setText(QString::number((float)(1/(deltaTime / 1000.0)), 'f', 3) + " Hz");
    }
    else{
        ui->CursorsDeltaTime->setText("-");
        ui->CursorsDeltaValue->setText("-");
        ui->CursorsDiff->setText("-");
        ui->CursorsDeltaFreq->setText("-");
    }
}

void MainWindow::updateCursorInfoSpectrum(CursorData cursor1, CursorData cursor2)
{
    if(cursor1.observedChannel != Channel::NONE){
        ui->spectrum_cursor1_signalstrengthLabel->setText(QString::number((float)cursor1.value,'f',3) + "");
        ui->spectrum_cursor1_frequencyLabel->setText(QString::number((float)cursor1.time,'f',3) + " Hz");
    }
    else{
        ui->spectrum_cursor1_signalstrengthLabel->setText("-");
        ui->spectrum_cursor1_frequencyLabel->setText("-");
    }
    if(cursor2.observedChannel != Channel::NONE){
        ui->spectrum_cursor2_signalstrengthLabel->setText(QString::number((float)cursor2.value,'f',3) + "");
        ui->spectrum_cursor2_frequencyLabel->setText(QString::number((float)cursor2.time,'f',3) + " Hz");
    }
    else{
        ui->spectrum_cursor2_signalstrengthLabel->setText("-");
        ui->spectrum_cursor2_frequencyLabel->setText("-");
    }

    if(cursor1.observedChannel != Channel::NONE && cursor2.observedChannel != Channel::NONE){
        float deltaFreq = std::abs(cursor1.time - cursor2.time);
        float deltaValue = std::abs(cursor1.value - cursor2.value);
        ui->spectrum_cursorDeltaFreq->setText(QString::number((float)deltaFreq,'f',3) + "");
        ui->spectrum_cursorDeltaMagnitude->setText(QString::number((float)deltaValue,'f',3) + " Hz");
    }
    else{
        ui->spectrum_cursorDeltaFreq->setText("-");
        ui->spectrum_cursorDeltaMagnitude->setText("-");
    }
}

void MainWindow::updatePortSettings(SerialPortSettings portSettings)
{
    ui->menuPort->setEnabled(true);
    ui->menuPomoc->setEnabled(true);
    dataCommunication.stopConnection();
    if(QSerialPortInfo::availablePorts().count() == 0) return;
    dataCommunication.setPort(portSettings.serialPortInfo);
    dataCommunication.setPortConfiguration(portSettings.baudRate,
                                           portSettings.dataBits,
                                           portSettings.parity,
                                           portSettings.stopBits);
    dataCommunication.startConnection();
}

///
/// Funkcja aktualizująca interfejs z nowymi odebranymi seriami danych pomiarowych w noej serii pomiarowej
/// \param dataListCh_1 - Lista próbek odebranych z kanału 1
/// \param dataListCh_2 - Lista próbek odebranych z kanału 2
///
void MainWindow::updateData(QList<Sample> dataListCh_1, QList<Sample> dataListCh_2){
    m_oscilloscopeChart->updateChart(dataListCh_1, dataListCh_2);
    SignalGlobalInformation signalInformation;

    QList<FrequencySample> ch1; QList<FrequencySample> ch2;

    if(m_isActive_Ch1){
        signalInformation.setSignalData(dataListCh_1);
        signalInformation.calculateBasicInfo();
        ui->CH1_VmaxLabelValue->setText(QString::number(signalInformation.maxValue(), 'f', 3) + " V");
        ui->CH1_VminLabelValue->setText(QString::number(signalInformation.minValue(), 'f', 3) + " V");
        ui->CH1_VppLabelValue->setText(QString::number(signalInformation.peakToPeakValue(), 'f', 3) + " V");
        ui->CH1_VrmsLabelValue->setText(QString::number(signalInformation.rmsValue(), 'f', 3) + " V");
        ui->CH1_VavgLabelValue->setText(QString::number(signalInformation.avgValue(), 'f', 3) + " V");
        signalInformation.calculateFFT(100.0 * 1000.0 / dataCommunication.timeBase(), m_spectrumChart->frequencyRange());
        ui->CH1_FreqLabelValue->setText(QString::number(signalInformation.frequenciesDataRanking().at(0).frequency, 'f', 3) + " Hz");

        ch1 = signalInformation.frequenciesDataList();
    }
    else{
        ui->CH1_VmaxLabelValue->setText("-");
        ui->CH1_VminLabelValue->setText("-");
        ui->CH1_VppLabelValue-> setText("-");
        ui->CH1_VrmsLabelValue->setText("-");
        ui->CH1_VavgLabelValue->setText("-");
        ui->CH1_FreqLabelValue->setText("-");
    }
    if(m_isActive_Ch2){
        signalInformation.setSignalData(dataListCh_2);
        signalInformation.calculateBasicInfo();
        ui->CH2_VmaxLabelValue->setText(QString::number(signalInformation.maxValue(), 'f', 3) + " V");
        ui->CH2_VminLabelValue->setText(QString::number(signalInformation.minValue(), 'f', 3) + " V");
        ui->CH2_VppLabelValue->setText(QString::number(signalInformation.peakToPeakValue(), 'f', 3) + " V");
        ui->CH2_VrmsLabelValue->setText(QString::number(signalInformation.rmsValue(), 'f', 3) + " V");
        ui->CH2_VavgLabelValue->setText(QString::number(signalInformation.avgValue(), 'f', 3) + " V");
        signalInformation.calculateFFT(100.0 * 1000.0 / dataCommunication.timeBase(), m_spectrumChart->frequencyRange());
        ui->CH2_FreqLabelValue->setText(QString::number(signalInformation.frequenciesDataRanking().at(0).frequency, 'f', 3) + " Hz");

        ch2 = signalInformation.frequenciesDataList();
    }
    else{
        ui->CH2_VmaxLabelValue->setText("-");
        ui->CH2_VminLabelValue->setText("-");
        ui->CH2_VppLabelValue-> setText("-");
        ui->CH2_VrmsLabelValue->setText("-");
        ui->CH2_VavgLabelValue->setText("-");
        ui->CH2_FreqLabelValue->setText("-");
    }

    if(m_spectrumChart->observedChannels() != Channel::NONE){
        ui->spectrum_Freq1Label->setText(QString::number(signalInformation.frequenciesDataRanking().at(0).frequency, 'f', 3) +
                                         " Hz   (" + QString::number(signalInformation.frequenciesDataRanking().at(0).value, 'f', 3) + ")");
        ui->spectrum_Freq2Label->setText(QString::number(signalInformation.frequenciesDataRanking().at(1).frequency, 'f', 3) +
                                         " Hz   (" + QString::number(signalInformation.frequenciesDataRanking().at(1).value, 'f', 3) + ")");
        ui->spectrum_Freq3Label->setText(QString::number(signalInformation.frequenciesDataRanking().at(2).frequency, 'f', 3) +
                                         " Hz   (" + QString::number(signalInformation.frequenciesDataRanking().at(2).value, 'f', 3) + ")");
    }
    else{
        ui->spectrum_Freq1Label->setText("-");
        ui->spectrum_Freq2Label->setText("-");
        ui->spectrum_Freq3Label->setText("-");
    }

    m_spectrumChart->updateChart(ch1, ch2);
}

///
/// Funkcja wyświetla okno dialogowe i łączy sygnał zamkniecia okna ze slotem ustawiającym konfiguracje komunikacji
/// i blokuję pasek menu by zapobiec wielkrtonemu otwioeraniu okien
///
void MainWindow::showPortSettingsDialog()
{
    m_posrtSelecrionDialogWindow = new PortSettingsDialog(this);
    m_posrtSelecrionDialogWindow->open();
    QObject::connect(m_posrtSelecrionDialogWindow, SIGNAL(portSettingsUpdated(SerialPortSettings)),
                     this, SLOT(updatePortSettings(SerialPortSettings)));
    ui->menuPort->setEnabled(false);
    ui->menuPomoc->setEnabled(false);
}

///
/// Funkcja która tworzy instancję okna dialogowego pozwaljacego na wybór języka, wykonuje odpowiednie wymagane połaćzenia
/// sygnałów z oknem i blokuję pasek menu by zapobiec wielkrtonemu otwioeraniu okien
///
void MainWindow::showLanguageSettingsDialog()
{
    m_languageSettingsDialog = new LanguageSettingsDialog(this);
    m_languageSettingsDialog->open();
    QObject::connect(m_languageSettingsDialog, SIGNAL(languageSettingsUpdated(QString)),
                     this, SLOT(updateLanguageSettings(QString)));
    ui->menuPort->setEnabled(false);
    ui->menuPomoc->setEnabled(false);
}

///
/// Slot realizuje opcję wybraną w oknie dialogowym wyboru języka. W zależności od wybrango języka instaluje lub usuwa
/// plik z tłumaczeniem napisów wewnątrz aplikacji
/// \param language - To łąńcuch znaków z nazwą wybranego języka.
///
void MainWindow::updateLanguageSettings(QString language)
{
    if(language == "english"){
        m_translator->load(":/test_newQT3_en.qm");
        QApplication::instance()->installTranslator(m_translator);
        ui->retranslateUi(this);
    }
    else{
        QApplication::instance()->removeTranslator(m_translator);
        ui->retranslateUi(this);
    }
    ui->menuPort->setEnabled(true);
    ui->menuPomoc->setEnabled(true);
}







