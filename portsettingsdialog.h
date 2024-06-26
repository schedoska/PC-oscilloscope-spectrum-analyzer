#ifndef PORTSETTINGSDIALOG_H
#define PORTSETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "serialportsettings.h"

///
/// \brief PortSettingsDialog - klasa okna dialogowego w którym można skonfigurować sposób komunikacji komputera z mikrokontrolerem, pozwlaa na
/// ustawienien informaji dotyczących wyborwu portu z którym będiz enastępowałą wymiana dnaych, szybkości komunikacji, ilości bitów stopu, ustawienia
/// bitu parzystości
///
class PortSettingsDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *m_selectPortLabel;
    QLabel *m_selectSpeedLabel;
    QLabel *m_selectDataBitsLabel;
    QLabel *m_selectParityLabel;
    QLabel *m_selectStopBitsLabel;

    QComboBox *m_selectPortComboBox;
    QComboBox *m_selectSpeedComboBox;
    QComboBox *m_selectDataBitsComboBox;
    QComboBox *m_selectParityComboBox;
    QComboBox *m_selectStopBitsComboBox;

    QPushButton *m_doneButton;

    ///
    /// \brief Funkcja pozwalajaca na uzupełenienie elementów graficznych wszystkicmi możliwymi opcjami konfiguracji
    ///
    void fillCobmboBoxOptions();

    SerialPortSettings m_currentPortSettings;
    QList<QSerialPortInfo> m_serialPortInfoList;

public:
    explicit PortSettingsDialog(QWidget *parent = nullptr);
    ///
    /// \brief Nadpisana funkcja closeEvent wywwoływana przy każdorazowym zamknięciu okna dialogu, jest wykorzystywana do zapisania i
    /// zastosowania wybranych w interfejsie graificznym opcji konfiguracji komunikacji zanim nastapi zamknięcie okna
    ///
    void closeEvent(QCloseEvent *event) override;
    ///
    /// \brief Funckja ustawiająca domyślne i zalecane wartości konumikacji w interfejsie okna (ustawione domyśłnie dane pomijaąc wybór
    /// portu) są ustalone do konfiguracji komunikacji na mikrokntrolerze
    /// \warning Domyślne ustawienine poza wyborem portu nie powinny być ruszane
    ///
    void setDefaultValues();

signals:
    ///
    /// \brief Sygnał wysyłąjacy ustaione skonfigurowne informacje dotyczące sposobu komunikacji (w postaci struktury SerialPOrtSettings)
    /// Sygnał jest wysyłąny przed każdny mzamknięciem okna dialogowego
    /// \param currentPortSettings - struktura danych zawierającyh wytycze konfiguracji portu
    ///
    void portSettingsUpdated(SerialPortSettings currentPortSettings);

public slots:
    ///
    /// \brief Slot zapisujący do struktury konfiguracji komunikacji wybrany z listy port, funkcja przesyła wybór w postaci indeksu z listy
    /// elemntu interfejsu
    ///
    void setPort(int);
    ///
    /// \brief Slot zapisujący do struktury konfiguracji komunikacji wybraną z listy ilosć bitów, funkcja przesyła wybóR w postaci
    /// indeksu z listy elemntu interfejsu
    ///
    void setDataBits(int);
    ///
    /// \brief Slot zapisujący do struktury konfiguracji komunikacji wybraną z listy opcej bitu parzystości, funkcja przesyła wybóR w postaci
    /// indeksu z listy elemntu interfejsu
    ///
    void setParity(int);
    ///
    /// \brief Slot - zapisujący do struktury konfiguracji komunikacji wybraną z listy szybkośc komunikacji, funkcja przesyła wybóR w postaci
    /// łańuchu znakóW tekstu wybranej opcji z elemntu interfejsu.
    ///
    void setSpeed(QString);
    ///
    /// \brief Slot - zapisujący do struktury konfiguracji komunikacji wybraną z listy ilość bitów stopu, funkcja przesyła wybóR w postaci
    /// indeksu z listy elemntu interfejsu
    ///
    void setStopBits(int);
};

#endif // PORTSETTINGSDIALOG_H
