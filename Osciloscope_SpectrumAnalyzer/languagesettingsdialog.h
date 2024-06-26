
#ifndef LANGUAGESETTINGSDIALOG_H
#define LANGUAGESETTINGSDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

///
/// \brief LanguageSettingsDialog - klasa okana pozwaljącjego na konfigurację wyboru języka aplikacji. Pozwala ana dynamiczną zaminaę języka
/// w aplikacji
///
class LanguageSettingsDialog : public QDialog
{
    Q_OBJECT
private:
    QLabel *m_selectLanguageLabel;
    QComboBox *m_selectLanguageComboBox;
    QLabel *m_selectLanguageImage;
    QPushButton *m_doneButton;

    ///
    /// \brief Funkcja która została nadpisana z klasy bazowej QDialog. Jest wywyoływanaa przy zamknięciu okna przez użytkownika.
    /// Emituje ona sygnał informujący inne klasy o tym że nastąpiła możliwa zmiana języka.
    ///
    void closeEvent(QCloseEvent *event) override;

public: 
    explicit LanguageSettingsDialog(QWidget *parent = nullptr);

signals:
    ///
    /// \brief Sygnał emitowany po zakończeniu wyboru języka i zamknięciu okna za pomocą 'x' w prawy góRnym rogu lub za pomocą
    /// przycisku 'Zatwierdź'
    /// \param language - wybrany język w postaci łańcucha znaków QString
    ///
    void languageSettingsUpdated(QString language);

public slots:
    ///
    /// \brief Slot wyzwalnay w chwili gdy następiła zmiana wybranego elementu z listy wyboru języków, Nie oznacza
    /// jeszcze zmiany języka całej aplikacji, a jedynie zmianę wyświetlanej flagi.
    /// \param index - argument w postaci indeksu wewnątrz listy wyboru języków.
    ///
    void languageChanged(int index);
};

#endif // LANGUAGESETTINGSDIALOG_H
