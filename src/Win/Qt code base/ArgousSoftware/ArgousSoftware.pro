QT += core
QT -= gui
CONFIG += c++11
TARGET = test
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
QT += network
QT += datavisualization


android|ios|winrt {
    error( "This example is not supported for android, ios, or winrt." )
}

#!include( ../examples.pri ) {
#    error( "Couldn't find the examples.pri file!" )
#}

QT += widgets
requires(qtConfig(combobox))
requires(qtConfig(fontcombobox))

OTHER_FILES += doc/src/* \
               doc/images/*

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    datamodel.cpp \
    dronespecs.cpp \
    main.cpp \
    argous.cpp \
    plotter.cpp \
    tripdata.cpp

HEADERS += \
    argous.h \
    datamodel.h \
    dronespecs.h \
    plotter.h \
    tripdata.h

FORMS += \
    argous.ui \
    datamodel.ui \
    dronespecs.ui \
    tripdata.ui

QT += widgets
requires(qtConfig(combobox))
requires(qtConfig(fontcombobox))

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    C:/Qt/Examples/Qt-6.3.2/datavisualization/examples.pri
