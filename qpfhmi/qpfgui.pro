#-------------------------------------------------
#
# Project created by QtCreator 2016-06-24T12:33:20
#
#-------------------------------------------------

include(../defaults.pri)

QT       += core gui sql network xml

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qpfgui
TEMPLATE = app


SOURCES += \
    browser.cpp \
    configtool.cpp \
    connectionwidget.cpp \
    dbbrowser.cpp \
    dbmng.cpp \
    dbtblmodel.cpp \
    dbtreemodel.cpp \
    dlgalert.cpp \
    dlgdataing.cpp \
    dlgshowtaskinfo.cpp \
    exttooledit.cpp \
    exttoolsdef.cpp \
    frmagentstatus.cpp \
    logframe.cpp \
    logwatcher.cpp \
    main.cpp \
    mainwindow.cpp \
    procalertmodel.cpp \
    proctskstatmodel.cpp \
    productsmodel.cpp \
    progbardlg.cpp \
    sysalertmodel.cpp \
    textview.cpp \
    txtblmodel.cpp \
    types.cpp \
    verbleveldlg.cpp \
    txitem.cpp \
    txitemdlg.cpp \
    qjsonitem.cpp \
    qjsonmodel.cpp \
    xmlsyntaxhighlight.cpp

HEADERS  += \
    browser.h \
    configtool.h \
    connectionwidget.h \
    dbbrowser.h \
    dbmng.h \
    dbtblmodel.h \
    dbtreemodel.h \
    dlgalert.h \
    dlgdataing.h \
    dlgshowtaskinfo.h \
    exttooledit.h \
    exttoolsdef.h \
    frmagentstatus.h \
    hmitypes.h \
    init.h \
    logframe.h \
    logwatcher.h \
    mainwindow.h \
    procalertmodel.h \
    proctskstatmodel.h \
    productsmodel.h \
    progbardlg.h \
    sysalertmodel.h \
    textview.h \
    txtblmodel.h \
    types.h \
    verbleveldlg.h \
    version.h \
    txitem.h \
    txitemdlg.h \
    qjsonitem.h \
    qjsonmodel.h \
    xmlsyntaxhighlight.h

FORMS    += \
    mainwindow.ui \
    dlgdataing.ui \
    dlgshowtaskinfo.ui \
    configtool.ui \
    browserwidget.ui \
    dbbrowser.ui \
    dlgalert.ui \
    exttooledit.ui \
    exttoolsdef.ui \
    frmagentstatus.ui \
    verbleveldlg.ui \
    logframe.ui \
    txitem.ui

RESOURCES += \
    configtool.qrc \
    icon.qrc \
    tx.qrc \
    jsontree.qrc

INCLUDEPATH += . ../src $$LIBCOMMINC $$SDCINC $$INFIXINC $$JSONCPPINC $$PSQLCPPINC

LIBS += -L../src -lQPF \
    -L$$LIBCOMMLIB -llibcomm \
    -L$$JSONCPPLIB -ljson \
    -L$$SDCLIB -lsdc \
    -L$$INFIXLIB -linfix \
    -L$$PSQLCPPLIB -l$$PSQLLIB \
    -L/opt/cots/lib -lzmq -lcurl -luuid
