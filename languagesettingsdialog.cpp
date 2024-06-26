#include <QApplication>
#include <QLayout>
#include <QTranslator>
#include "languagesettingsdialog.h"

void LanguageSettingsDialog::closeEvent(QCloseEvent *event)
{
    close();
    emit languageSettingsUpdated(m_selectLanguageComboBox->currentText());
}

LanguageSettingsDialog::LanguageSettingsDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle(QApplication::translate("LanguageSettingsDialog","Konfiguracja Języka"));
    setFixedSize(400, 130);

    setAttribute(Qt::WA_DeleteOnClose, true);

    m_selectLanguageLabel = new QLabel(QApplication::translate("LanguageSettingsDialog", "Wybierz język:", nullptr), this);
    m_selectLanguageComboBox = new QComboBox(this);
    m_selectLanguageComboBox->addItem("polski");
    m_selectLanguageComboBox->addItem("english");
    m_selectLanguageImage = new QLabel(this);

    languageChanged(m_selectLanguageComboBox->currentIndex());
    QObject::connect(m_selectLanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged(int)));

    m_doneButton = new QPushButton(QApplication::translate("LanguageSettingsDialog", "Zatwierdź", nullptr), this);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(m_selectLanguageLabel);
    hBoxLayout->addWidget(m_selectLanguageComboBox,1);
    hBoxLayout->addWidget(new QWidget(), 0.3);
    hBoxLayout->addWidget(m_selectLanguageImage);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(m_doneButton);
    setLayout(vBoxLayout);

    QObject::connect(m_doneButton, SIGNAL(released()), this, SLOT(close()));
}

///
/// Slot powoduje aktualizację zawartości obrazka wyświetlanego po prawej stronie do listy wyboru, obrazek przedstawia flagę kraju
/// reprezentującego wybrany aktualnie język.
///
void LanguageSettingsDialog::languageChanged(int index)
{
    QPixmap flag_image;
    if(index == 0){
        flag_image.load(":/Flag_of_Poland.png");
    }
    else{
        flag_image.load(":/Flag_of_the_United_Kingdom.png");
    }
    m_selectLanguageImage->setPixmap(flag_image.scaled(50,20,Qt::KeepAspectRatio));
}

