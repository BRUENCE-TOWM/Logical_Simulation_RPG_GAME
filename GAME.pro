QT       += core gui
QT += multimedia multimediawidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
FORMS += mainwindow.ui
CONFIG += c++17
RESOURCES += \
    resources.qrc

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LianliankanGame.cpp \
    MinesweeperGame.cpp \
    draggablegate.cpp \
    gatemanager.cpp \
    logiceditordialog.cpp \
    main.cpp \
    mainwindow.cpp \
    wireitem.cpp

HEADERS += \
    LianliankanGame.h \
    MinesweeperGame.h \
    draggablegate.h \
    gatemanager.h \
    logiceditordialog.h \
    mainwindow.h \
    wireitem.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    GAME_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
