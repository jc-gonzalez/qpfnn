/******************************************************************************
 * File:    logframe.cpp
 *          Implementation of class LogFrame
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
 *   Implementation of class LogFrame
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

#include "logframe.h"
#include "ui_logframe.h"

#include "datatypes.h"

#include <QTimer>

LogFrame::LogFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LogFrame)
{
    ui->setupUi(this);
}

LogFrame::~LogFrame()
{
    delete ui;
}

void LogFrame::setMsg(QString msg)
{
    ui->lblMsg->setText(msg);
    QTimer::singleShot(100, this, SLOT(clearMsg()));
}

void LogFrame::clearMsg()
{
    ui->lblMsg->clear();
}

void LogFrame::displayLed(QString msgType)
{
    /*
    std::string msgt(msgType.toStdString());
    if (msgt == MSG_INDATA) {
        ui->lblMsgData->setStyleSheet("{bgcolor: #white;}");
    } else if (msgt == MSG_DATA_INFO) {
        ui->lblMsgData->setStyleSheet("{bgcolor: #cyan;}");
    } else if (msgt == MSG_DATA_RQST) {
        ui->lblMsgData->setStyleSheet("{bgcolor: #purple;}");
    } else if (msgt == MSG_CMD) {
        ui->lblMsgCmd->setStyleSheet("{bgcolor: #blue;}");
    } else if (msgt == MSG_MONIT_INFO) {
        ui->lblMsgMonit->setStyleSheet("{bgcolor: #pink;}");
    } else if (msgt == MSG_MONIT_RQST) {
        ui->lblMsgMonit->setStyleSheet("{bgcolor: #magenta;}");
    } else if (msgt == MSG_TASK_PROC) {
        ui->lblMsgTsk->setStyleSheet("{bgcolor: #green;}");
    } else if (msgt == MSG_TASK_RES) {
        ui->lblMsgTsk->setStyleSheet("{bgcolor: #red;}");
        }*/
    QTimer::singleShot(10, this, SLOT(clearLed()));
}

void LogFrame::clearLed()
{
    ui->lblMsgData->setStyleSheet("");
    ui->lblMsgCmd->setStyleSheet("");
    ui->lblMsgMonit->setStyleSheet("");
    ui->lblMsgTsk->setStyleSheet("");
}

QPlainTextEdit * LogFrame::getTextEditor()
{
    return ui->pltxtLog;
}
