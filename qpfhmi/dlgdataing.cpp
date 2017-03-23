/******************************************************************************
 * File:    dlgdataing.cpp
 *          Implementation of class DlgDataIngestion
 *
 * Domain:  QPF.qpfgui.dlgdataing
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
 *   Implementation of class DlgDataIngestion
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

#include "dlgdataing.h"
#include "ui_dlgdataing.h"

DlgDataIngestion::DlgDataIngestion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDataIngestion)
{
    ui->setupUi(this);
}

DlgDataIngestion::~DlgDataIngestion()
{
    delete ui;
}
