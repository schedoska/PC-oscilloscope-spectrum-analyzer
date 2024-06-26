QT       += core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cursor.cpp \
    cursordata.cpp \
    datacommuniaction.cpp \
    languagesettingsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    oscilloscopechart.cpp \
    portsettingsdialog.cpp \
    sample.cpp \
    serialportsettings.cpp \
    signalglobalinformation.cpp \
    spectrumchart.cpp

HEADERS += \
    cursor.h \
    cursordata.h \
    datacommuniaction.h \
    fftw3.h \
    languagesettingsdialog.h \
    mainwindow.h \
    oscilloscopechart.h \
    portsettingsdialog.h \
    sample.h \
    serialportsettings.h \
    signalglobalinformation.h \
    spectrumchart.h

FORMS += \
    mainwindow.ui

LIBS += "$$PWD/libfftw3-3.dll"

TRANSLATIONS = test_newQT3_en.ts

DEFAULTCODEC = ISO-8859-2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    test_newQT3_en.qm

RESOURCES += \
    translations.qrc
