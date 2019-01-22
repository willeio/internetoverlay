#-------------------------------------------------
#
# Project created by QtCreator 2016-12-02T12:43:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IOChatExample
TEMPLATE = app
LIBS += -L/usr/lib/ -lIOClient -lIO
INCLUDEPATH += "/home/wille/Desktop/dev/prv_dev/InternetOverlay/src/"
QMAKE_CXXFLAGS += -std=c++11


SOURCES += main.cpp\
    worker.cpp \
    overview.cpp \
    contactsmodel.cpp \
    addcontact.cpp \
    conversation.cpp

HEADERS  += worker.h \
    overview.h \
    contactsmodel.h \
    addcontact.h \
    contact.h \
    conversation.h \
    message.h

FORMS    += overview.ui \
    addcontact.ui \
    conversation.ui
