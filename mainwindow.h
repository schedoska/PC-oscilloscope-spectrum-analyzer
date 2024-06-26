
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QTranslator>
#include "oscilloscopechart.h"
#include "datacommuniaction.h"
#include "spectrumchart.h"
#include "portsettingsdialog.h"
#include "languagesettingsdialog.h"

using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

///
/// \brief Główne okno aplikacji zarządzające elemnatmi interfejsu
///
class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DataCommuniaction dataCommunication;
    OscilloscopeChart *m_oscilloscopeChart;
    SpectrumChart *m_spectrumChart;
    QTranslator *m_translator;
    PortSettingsDialog *m_posrtSelecrionDialogWindow;
    LanguageSettingsDialog *m_languageSettingsDialog;

    bool m_isActive_Ch1;
    bool m_isActive_Ch2;
    ///
    /// \brief updateObservedChannels - Funkcja pozwaljąca określić kanał/y jaki ma być aktualnie obserwowany
    ///
    void updateObservedChannels();

    ///
    /// \brief setChannel_1_GUI_Enabled - Aktywuje kawałek interfejsu odpowiedzialny za okontrolę sygnału kanału 1
    ///
    void setChannel_1_GUI_Enabled(bool set);
    ///
    /// \brief setChannel_2_GUI_Enabled - Aktywuje kawałek interfejsu odpowiedzialny za okontrolę sygnału kanału 2
    ///
    void setChannel_2_GUI_Enabled(bool set);

public slots:
    ///
    /// \brief changeActive_Ch1 - slot zmieniający stan aktywności kanału 1
    ///
    void changeActive_Ch1();
    ///
    /// \brief changeActive_Ch1 - slot zmieniający stan aktywności kanału 2
    ///
    void changeActive_Ch2();
    ///
    /// \brief setDisabled - slot wstrzymujący odbieranie paczek i zamrażający wykres oscyloskopu
    ///
    void setDisabled();
    ///
    /// \brief setEnabled - slot wznawiający odbieranie paczek i odmrażający wykres oscyloskopu
    ///
    void setEnabled();
    ///
    /// \brief updateCursorInfo - slot wymuszający aktualizacje interfejsu z nowymi danymi pochodzącymi od kursorów
    ///
    void updateCursorInfo(CursorData cursor1, CursorData cursor2);
    ///
    /// \brief updateCursorInfoSpectrum - Slot wyzwalanay w momencie gdy dostępne są nowe dane pochodzące od kursoróW
    ///
    void updateCursorInfoSpectrum(CursorData cursor1, CursorData cursor2);
    ///
    /// \brief updatePortSettings - slot oizwalający ostawić nowe ustawienia portu kounikacyjnego
    ///
    void updatePortSettings(SerialPortSettings portSettings);

    ///
    /// \brief updateData - slot odbierający nowe serie danych z obu kanałów
    ///
    void updateData(QList<Sample> dataListCh_1, QList<Sample> dataListCh_2);
    ///
    /// \brief showPortSettingsDialog - Wyświetlenie okna dialogowego służącego do skonfigurowania komunikacji portu szeregowego
    /// z mikrokontrolerem
    ///
    void showPortSettingsDialog();
    ///
    /// \brief showLanguageSettingsDialog - Wyświetlenie okna dialogowego służącego do skonfigurowania języka wykorzystywywanego
    /// w aplikacji (angielski lub polski)
    ///
    void showLanguageSettingsDialog();
    ///
    /// \brief updateLanguageSettings - Slot aktualizujący wykorzystywany w aplikacji język, wykonuje konieczne operacje
    /// do przetłumaczenia wszytskich napisów w aplikacji.
    ///
    void updateLanguageSettings(QString language);
};

#endif // MAINWINDOW_H
