/******************************************************************************
 * File:    frmagentstatus.cpp
 *          Implementation of class FrmAgentStatus
 *
 * Domain:  QPF.qpfgui.frmagentstatus
 *
 * Version:  1.2
 *
 * Date:    2016-11-03
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
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

void FrmAgentStatus::updateInfo(AgentInfo & a)
{
    ui->lblTaskAgentName->setText(QString::fromStdString(a.name));


    TaskStatusSpectra & d = a.taskStatus;
    ui->lblRun->setText(QString("%1").arg(d.running));
    ui->lblWait->setText(QString("%1").arg(d.scheduled));
    ui->lblPau->setText(QString("%1").arg(d.paused));
    ui->lblSto->setText(QString("%1").arg(d.stopped));
    ui->lblFail->setText(QString("%1").arg(d.failed));
    ui->lblFin->setText(QString("%1").arg(d.finished));

    int total = d.running + d.scheduled + d.paused + d.stopped + d.failed + d.finished;
    ui->lblTotalTasks->setText(QString("%1").arg(total));

    if (total > 0) {
        QVBoxLayout * vb = qobject_cast<QVBoxLayout*>(ui->vlyStatus);
        vb->setStretch(0, d.running    * 1000);
        vb->setStretch(1, d.scheduled  * 1000);
        vb->setStretch(2, d.paused     * 1000);
        vb->setStretch(3, d.stopped    * 1000);
        vb->setStretch(4, d.failed     * 1000);
        vb->setStretch(5, d.finished   * 1000);
//        ui->vlyStatus->setVisible(true);
//    } else {
//        ui->vlyStatus->setVisible(false);
    }
}

}
