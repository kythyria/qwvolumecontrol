QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    volmainwindow.cpp \
    devicemixerlistwidget.cpp \
    devicemixerwidget.cpp \
    util.cpp \
    geticonfromresource.cpp \
    abstractvolumemodel.cpp \
    devicevolumemodel.cpp

RESOURCES += \
    resources.qrc

LIBS += "-LC:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x64" -lOle32 -lUser32 -lcomsuppw

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    volmainwindow.h \
    devicemixerlistwidget.h \
    devicemixerwidget.h \
    util.h \
    geticonfromresource.h \
    abstractvolumemodel.h \
    devicevolumemodel.h

DISTFILES += \
    .astylerc \
    appstyle.qss

FORMS +=
