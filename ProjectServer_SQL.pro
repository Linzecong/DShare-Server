QT += core
QT += gui

CONFIG += c++11
QT+=sql
QT+=network

TARGET = ProjectServer_SQL
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    RegistServer.h \
    LoginServer.h \
    DBTimer.h \
    DataServer.h \
    PostSystem.h \
    RecordServer.h \
    ReportServer.h \
    NewsServer.h \
    RecommendServer.h \
    ImageServer.h
