/******************************************************************************
 * File:    proctskmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.proctskmng
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Provides object implementation for some declarations
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

#include "proctskmng.h"

namespace QPF {

ProcTaskManager::ProcTaskManager(QObject *parent) : QObject(parent)
{
    if (QSqlDatabase::drivers().isEmpty()) {
        qFatal("This application requires at least one Qt database driver.\n"
               "Please check the documentation how to build the "
               "Qt SQL plugins.");
    }
}

ProcTaskManager::~ProcTaskManager()
{

}

void ProcTaskManager::checkTaskStatusInfo()
{

}

QSqlError ProcTaskManager::addConnection(const QString &driver, const QString &connectionName,
                                   const QString &dbName, const QString &host,
                                   const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;

    QSqlError err;
    db = QSqlDatabase::addDatabase(driver, connectionName);
    ++cCount;
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
        --cCount;
    } else {
        connections[connectionName] = cCount;
        activeDb = connectionName;
        qDebug(qPrintable("ActiveDb: " + activeDb));
    }

    return err;
}

void ProcTaskManager::addConnection(const QString &connectionName, DBConnection & conn)
{
    QSqlError err = addConnection(conn.driverName, connectionName,
                                  conn.databaseName, conn.hostName,
                                  conn.userName, conn.password, conn.port);
    if (err.type() != QSqlError::NoError) {
        qWarning(qPrintable(QString("An error occurred while "
                                    "opening the connection: ") + err.text()));
    }
}

QString ProcTaskManager::getState()
{
    QSqlQuery qry("SELECT state FROM qpfstates ORDER BY qpfstate_id DESC LIMIT 1;", db);
    if (qry.next()) {
        return qry.value(0).toString();
    } else {
        return QString("OFF");
    }
}


bool ProcTaskManager::getTasksInfo(int offset = 0, QMap<QString, QJsonObject> & newTasks)
{
    QString cmd =
        QSqlQuery qry(QString("SELECT * FROM tasks_info ORDER BY id OFFSET %1;")
                      .arg(offset),
                      db);
    if (qry.size() > 0) {
        QSqlRecord rec = qry.record();
        const int pos_id                = rec.indexOf("id");
        /*
        const int pos_task_id           = rec.indexOf("task_id");
        const int pos_task_status_id    = rec.indexOf("task_status_id");
        const int pos_task_exitcode     = rec.indexOf("task_exitcode");
        const int pos_task_path         = rec.indexOf("task_path");
        const int pos_task_size         = rec.indexOf("task_size");
        const int pos_registration_time = rec.indexOf("registration_time");
        const int pos_start_time        = rec.indexOf("start_time");
        const int pos_end_time          = rec.indexOf("end_time");
        */
        const int pos_task_data         = rec.indexOf("task_data");

        while (qry.next()) {
            /*
            QStringList row;
            row << qry.value(pos_id).toString()
                << qry.value(pos_task_id).toString()
                << qry.value(pos_task_status_id).toString()
                << qry.value(pos_task_exitcode).toString()
                << qry.value(pos_task_path).toString()
                << qry.value(pos_task_size).toString()
                << qry.value(pos_registration_time).toString()
                << qry.value(pos_start_time).toString()
                << qry.value(pos_end_time).toString()
                << qry.value(pos_task_data).toString()
                row << qry.value(nameCol).toString()
            */
            QString key = qry.value(pos_id).toString();
            QJsonObject v = qry.value(pos_task_data).toJsonValue();
            v["ZUpdatable"] = true;
            newTasks[key] = v;
        }
        return true;
    }

    return false;
}


}
