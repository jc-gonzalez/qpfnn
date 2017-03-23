/******************************************************************************
 * File:    dbmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbmng
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

#include "dbmng.h"

namespace QPF {

QSqlDatabase       DBManager::db;
QString            DBManager::activeDb;
QMap<QString, int> DBManager::connections;

DBManager::DBManager()
{
    if (QSqlDatabase::drivers().isEmpty()) {
        qFatal("This application requires at least one Qt database driver.\n"
               "Please check the documentation how to build the "
               "Qt SQL plugins.");
    }
}

DBManager::~DBManager()
{
    close();
}

void DBManager::close()
{
    QStringList cnctNames = db.connectionNames();

    db.close();
    db = QSqlDatabase();
    foreach (QString cnctName, cnctNames) {
        db.removeDatabase(cnctName);
    }
}

void DBManager::checkTaskStatusInfo()
{

}

QSqlError DBManager::addConnection(const QString &driver, const QString &connectionName,
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
    }

    return err;
}

void DBManager::addConnection(const QString &connectionName, DBConnection & conn)
{
    QSqlError err = addConnection(conn.driverName, connectionName,
                                  conn.databaseName, conn.hostName,
                                  conn.userName, conn.password, conn.port);
    if (err.type() != QSqlError::NoError) {
        qWarning(qPrintable(QString("An error occurred while "
                                    "opening the connection: ") + err.text()));
    }
}

QString DBManager::getState()
{
    QSqlQuery qry("SELECT state FROM qpfstates ORDER BY qpfstate_id DESC LIMIT 1;", db);
    if (qry.next()) {
        return qry.value(0).toString();
    } else {
        return QString("OFF");
    }
}

void DBManager::setState(QString newState)
{
    QDateTime now(QDateTime::currentDateTime());
    QDateTime nowUTC(now);
    nowUTC.setTimeSpec(Qt::UTC);

    QString sqry(QString("INSERT INTO qpfstates (timestmp, state) VALUES ('%1', '%2');")
                 .arg(nowUTC.toString(Qt::ISODate))
                 .arg(newState));
    QSqlQuery qry(sqry, db);

    if (qry.lastError().type() != QSqlError::NoError) {
        qErrnoWarning(qPrintable(qry.lastError().nativeErrorCode() + ": " +
                                 qry.lastError().text()));
    }
}

QMap<QString,QString> DBManager::getCurrentStates(QString session)
{
    QSqlQuery qry(QString("SELECT nodename, state FROM qpfstates "
                          "WHERE sessionname = '%1' "
                          "ORDER BY qpfstate_id;").arg(session), db);
    QMap<QString,QString> result;
    while (qry.next()) {
        result[qry.value(0).toString()] = qry.value(1).toString();
    }

    return result;
}

int DBManager::numOfRowsInDbTable(QString tableName)
{
    QSqlQuery qry(QString("SELECT reltuples::bigint AS estimate "
                          "FROM   pg_class "
                          "WHERE  oid = 'public.%1'::regclass;")
                  .arg(tableName), db);
    if (qry.next()) {
        return qry.value(0).toInt();
    } else {
        return 0L;
    }
}

void DBManager::addICommand(QString cmd)
{
    QDateTime now(QDateTime::currentDateTime());
    QDateTime nowUTC(now);
    nowUTC.setTimeSpec(Qt::UTC);

    QString sqry(QString("INSERT INTO icommands "
                         "(cmd_date, cmd_source, cmd_target, cmd_executed, cmd_content) "
                         "VALUES ('%1', '%2', '%3', false, '%4');")
                 .arg(nowUTC.toString(Qt::ISODate))
                 .arg("QPFHMI")
                 .arg("EvtMng")
                 .arg(cmd));
    QSqlQuery qry(sqry, db);

    if (qry.lastError().type() != QSqlError::NoError) {
        qErrnoWarning(qPrintable(qry.lastError().nativeErrorCode() + ": " +
                                 qry.lastError().text()));
    }
}

bool DBManager::getICommand(QString cmd, bool removeCmd)
{
    bool result = true;
    QString sqry(QString("SELECT cmd.id "
                         " FROM icommands cmd "
                         " WHERE cmd.cmd_source = '%1' "
                         "   AND cmd.cmd_target = '%2'"
                         "   AND cmd.cmd_content = '%3'"
                         "   AND cmd.cmd_executed = false "
                         " ORDER BY cmd.id LIMIT 1;")
                 .arg("EvtMng")
                 .arg("QPFHMI")
                 .arg(cmd));
    QSqlQuery qry(sqry, db);
    if (qry.next()) {
        QString id(qry.value(0).toString());
        if (removeCmd) {
           // Remove command from table
            QSqlQuery qry2(QString("DELETE FROM icommands "
                                   " WHERE id = %1;").arg(id), db);
        } else {
            // Deactivate answer, so it doesn't get used again
            QSqlQuery qry2(QString("UPDATE icommands SET cmd_executed = true "
                                   " WHERE id = %1;").arg(id), db);
        }
    } else if (qry.lastError().type() != QSqlError::NoError) {
        qErrnoWarning(qPrintable(qry.lastError().nativeErrorCode() + ": " +
                                 qry.lastError().text()));
        result = false;
    } else {
        result = false;
    }

    return result;
}

void DBManager::removeICommands(QString cmd)
{
    QString sqry(QString("DELETE FROM icommands cmd "
                         " WHERE cmd.cmd_content = '%1';").arg(cmd));
    QSqlQuery qry(sqry, db);
}

/*
bool DBManager::getTasksInfo(QMap<QString, QJsonObject> & newTasks, int offset)
{
    QSqlQuery qry(QString("SELECT * FROM tasks_info ORDER BY id OFFSET %1;")
                  .arg(offset), db);

    if (qry.size() > 0) {
        QSqlRecord rec = qry.record();
        const int pos_id                = rec.indexOf("id");
        / *
        const int pos_task_id           = rec.indexOf("task_id");
        const int pos_task_status_id    = rec.indexOf("task_status_id");
        const int pos_task_exitcode     = rec.indexOf("task_exitcode");
        const int pos_task_path         = rec.indexOf("task_path");
        const int pos_task_size         = rec.indexOf("task_size");
        const int pos_registration_time = rec.indexOf("registration_time");
        const int pos_start_time        = rec.indexOf("start_time");
        const int pos_end_time          = rec.indexOf("end_time");
        * /
        const int pos_task_data         = rec.indexOf("task_data");

        while (qry.next()) {
            / *
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
            * /
            QString key = qry.value(pos_id).toString();
            QJsonObject v = qry.value(pos_task_data).toJsonObject();
            v["ZUpdatable"] = true;                                                                   newTasks[key] = v;
        }
        return true;
    }

    return false;
}
*/

}
