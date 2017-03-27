/******************************************************************************
 * File:    frmagentstatus.cpp
 *          Implementation of class FrmAgentStatus
 *
 * Domain:  QPF.qpfgui.frmagentstatus
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
 *   Implementation of class FrmAgentStatus
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

#include "frmagentstatus.h"
#include "ui_frmagentstatus.h"

namespace QPF {

FrmAgentStatus::FrmAgentStatus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrmAgentStatus)
{
    ui->setupUi(this);
}

FrmAgentStatus::~FrmAgentStatus()
{
    delete ui;
}

void FrmAgentStatus::updateInfo(TaskAgentInfo & d)
{
    ui->lblTaskAgentName->setText(QString::fromStdString(d.name()));
    ui->lblTotalTasks->setText(QString("%1").arg(d.total()));

    ui->lblRun->setText(QString("%1").arg(d.running()));
    ui->lblWait->setText(QString("%1").arg(d.waiting()));
    ui->lblPau->setText(QString("%1").arg(d.paused()));
    ui->lblSto->setText(QString("%1").arg(d.stopped()));
    ui->lblFail->setText(QString("%1").arg(d.failed()));
    ui->lblFin->setText(QString("%1").arg(d.finished()));

    ui->lblMaxTasks->setText(QString("%1").arg(d.maxnum()));
    ui->lblClient->setText(QString::fromStdString(d.client()));
    ui->lblServer->setText(QString::fromStdString(d.server()));

    ui->lblLoadAvg1->setText(QString("%1").arg(((double)(d.load1min())) * 0.01, 0, 'f', 2));
    ui->lblLoadAvg2->setText(QString("%1").arg(((double)(d.load5min())) * 0.01, 0, 'f', 2));
    ui->lblLoadAvg3->setText(QString("%1").arg(((double)(d.load15min())) * 0.01, 0, 'f', 2));

    ui->lblUptime->setText(QString("%1").arg(d.uptimesecs()));

    if (d.total() > 0) {
        QHBoxLayout * hb = qobject_cast<QHBoxLayout*>(ui->frmStatusBar->layout());
        hb->setStretch(0, d.running()  * 1000);
        hb->setStretch(1, d.waiting()  * 1000);
        hb->setStretch(2, d.paused()   * 1000);
        hb->setStretch(3, d.stopped()  * 1000);
        hb->setStretch(4, d.failed()   * 1000);
        hb->setStretch(5, d.finished() * 1000);
        ui->frmStatusBar->setVisible(true);
    } else {
        ui->frmStatusBar->setVisible(false);
    }
}

}
