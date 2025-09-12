QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
win32:LIBS += -user32


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ImageView.cpp \
    MyListViewMenu.cpp \
    ShowImageInfo.cpp \
    TipLabel.cpp \
    main.cpp \
    AlbumWindow.cpp

HEADERS += \
    AlbumWindow.h \
    ImageView.h \
    MyListViewMenu.h \
    ShowImageInfo.h \
    TipLabel.h

FORMS += \
    AlbumWindow.ui \
    MyListViewMenu.ui

TRANSLATIONS += \
    My_Album_yue_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = res/icon.ico

RESOURCES += \
    res.qrc
