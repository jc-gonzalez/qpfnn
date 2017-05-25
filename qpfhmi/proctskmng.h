/******************************************************************************
 * File:    proctskmng.h
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
 *   Declaration of several dataproctskmng for QPF HMI
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
#ifndef PROCTASKMANAGER_H
#define PROCTASKMANAGER_H

#include <QtSql>

namespace QPF {

class ProcTaskManager : public QObject {

    Q_OBJECT

public:
    explicit ProcTaskManager(QObject *parent = 0);
    ~ProcTaskManager();

public:
    struct DBConnection {
        QString driverName;
        QString databaseName;
        QString userName;
        QString password;
        QString hostName;
        int port;
    };

    QSqlError addConnection(const QString &driver, const QString &connectionName,
                            const QString &dbName, const QString &host,
                            const QString &user, const QString &passwd, int port);
    void addConnection(const QString &connectionName, DBConnection & conn);

    QString getState();

signals:
    void newTaskStatusInfo();

private:
    void checkTaskStatusInfo();

private:
    QSqlDatabase       db;
    QString            activeDb;
    QMap<QString, int> connections;

};

}

#endif // PROCTASKMANAGER_H
