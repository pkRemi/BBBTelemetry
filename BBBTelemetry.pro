#-------------------------------------------------
#
# Project created by QtCreator 2013-05-01T20:35:02
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BBBTelemetry
TEMPLATE = app


SOURCES +=  main.cpp\
            bbbtelemetry.cpp \
            hid_pnp.cpp \
         ../QCustomPlot/qcustomplot.cpp

HEADERS  += bbbtelemetry.h \
            hid_pnp.h \
         ../QCustomPlot/qcustomplot.h

FORMS    += bbbtelemetry.ui


#-------------------------------------------------
# Add the Signal11's hidapi library that was
# created
#-------------------------------------------------
macx: LIBS += -L../HIDAPI/mac -lHIDAPI
win32: LIBS += -L../HIDAPI/windows -lHIDAPI
unix: !macx: LIBS += -L../HIDAPI/linux -lHIDAPI

#-------------------------------------------------
# Make sure to add the required libraries or
# frameoworks for the hidapi to work depending on
# what OS is being used
#-------------------------------------------------
macx: LIBS += -framework CoreFoundation -framework IOkit
win32: LIBS += -lSetupAPI
unix: !macx: LIBS += -lusb-1.0

#-------------------------------------------------
# Make sure output directory for object file and
# executable is in the correct subdirectory
#-------------------------------------------------
macx {
    DESTDIR = mac
    OBJECTS_DIR = mac
    MOC_DIR = mac
    UI_DIR = mac
    RCC_DIR = mac
}
unix: !macx {
    DESTDIR = linux
    OBJECTS_DIR = linux
    MOC_DIR = linux
    UI_DIR = linux
    RCC_DIR = linux
}
win32 {
    DESTDIR = windows
    OBJECTS_DIR = windows
    MOC_DIR = windows
    UI_DIR = windows
    RCC_DIR = windows
}

