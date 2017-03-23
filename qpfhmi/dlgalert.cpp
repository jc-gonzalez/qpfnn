/******************************************************************************
 * File:    dlgalert.cpp
 *          Implementation of class DlgAlert
 *
 * Domain:  QPF.qpfgui.dlgalert
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
 *   Implementation of class DlgAlert
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

#include "dlgalert.h"
#include "ui_dlgalert.h"

namespace QPF {

DlgAlert::DlgAlert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAlert)
{
    ui->setupUi(this);
}

DlgAlert::~DlgAlert()
{
    delete ui;
}

void DlgAlert::setAlert(Alert & alert)
{
    ui->edTimestamp->setText(QString::fromStdString(alert.timeStampString()));
    ui->edID->setText(QString::fromStdString(Alert::TypeName[alert.getType()]));
    ui->edSeverity->setText(QString::fromStdString(Alert::SeverityName[alert.getSeverity()]));
    ui->edComponent->setText(QString::fromStdString(alert.getOrigin()));
    ui->textedDescription->setPlainText(QString::fromStdString(alert.what()));
}

}
