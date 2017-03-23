/******************************************************************************
 * File:    dlgshowtaskinfo.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.QPF.dlgshowtaskinfo
 *
 * Version:  1.1
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   
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
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/
#include "dlgshowtaskinfo.h"
#include "ui_dlgshowtaskinfo.h"

DlgShowTaskInfo::DlgShowTaskInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgShowTaskInfo)
{
    ui->setupUi(this);
    ui->ptxtedTaskInfo->setVisible(false);
}

DlgShowTaskInfo::~DlgShowTaskInfo()
{
    delete ui;
}

QTreeWidget * DlgShowTaskInfo::getTreeTaskInfo()
{
    return ui->treeTaskInfo;
}

void DlgShowTaskInfo::setTaskInfo(QString info)
{
    ui->ptxtedTaskInfo->setPlainText(info);
}
