/******************************************************************************
 * File:    logframe.h
 *          Declaration of class LogFrame
 *
 * Domain:  QPF.qpfgui.logframe
 *
 * Version:  1.1
 *
 * Date:    2016-11-03
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declaration of class LogFrame
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <ChangeLog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef LOGFRAME_H
#define LOGFRAME_H

#include <QFrame>
#include <QPlainTextEdit>

namespace Ui {
class LogFrame;
}

class LogFrame : public QFrame
{
    Q_OBJECT

public:
    explicit LogFrame(QWidget *parent = 0);
    ~LogFrame();

    QPlainTextEdit * getTextEditor();

public slots:
    void setMsg(QString msg);
    void clearMsg();

    void displayLed(QString msgType);
    void clearLed();

private:
    Ui::LogFrame *ui;
};

#endif // LOGFRAME_H
