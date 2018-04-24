QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    volmainwindow.cpp \
    devicemixerlistwidget.cpp \
    devicemixerwidget.cpp \
    util.cpp \
    abstractvolumemodel.cpp \
    devicevolumemodel.cpp \
    volumesliderwidget.cpp \
    volumesliderwidgetinternal.cpp \
    wheellessslider.cpp \
    sessioncreationsource.cpp \
    sessionmixerlistwidget.cpp \
    getresource.cpp \
    sessionvolumemodel.cpp \
    devicecollectionmodel.cpp \
    devicemixerbankwidget.cpp

RESOURCES += \
    resources.qrc

LIBS += "-LC:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x64" -lOle32 -lUser32 -lcomsuppw -lShlwapi -lMincore

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    volmainwindow.h \
    devicemixerlistwidget.h \
    devicemixerwidget.h \
    util.h \
    abstractvolumemodel.h \
    devicevolumemodel.h \
    volumesliderwidget.h \
    volumesliderwidgetinternal.h \
    speakerformats.h \
    wheellessslider.h \
    sessioncreationsource.h \
    comstuff.h \
    sessionmixerlistwidget.h \
    getresource.h \
    sessionvolumemodel.h \
    devicecollectionmodel.h \
    devicemixerbankwidget.h

DISTFILES += \
    .astylerc \
    appstyle.qss \
    README.md

FORMS +=
