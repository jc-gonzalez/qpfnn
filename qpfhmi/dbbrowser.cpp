/******************************************************************************
 * File:    dbbrowser.cpp
 *          Implementation of class DBBrowser
 *
 * Domain:  QPF.qpfgui.dbbrowser
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
 *   Implementation of class DBBrowser
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

#include "dbbrowser.h"
#include "ui_dbbrowser.h"
#include "config.h"

using Configuration::cfg;

DBBrowser::DBBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DBBrowser)
{
    ui->setupUi(this);

    if (QSqlDatabase::connectionNames().isEmpty()) {
        QString databaseName ( Config::DBName.c_str() );
        QString userName     ( Config::DBUser.c_str() );
        QString password     ( Config::DBPwd.c_str() );
        QString hostName     ( Config::DBHost.c_str() );
        QString port         ( Config::DBPort.c_str() );

        Browser::DBConnection connection = { "QPSQL",        // .driverName
                                             databaseName,
                                             userName,
                                             password,
                                             hostName,
                                             port.toInt() };
        ui->browser->addConnection(connection);
    }
}

DBBrowser::~DBBrowser()
{
    delete ui;
}
