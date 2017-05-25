/******************************************************************************
 * File:    dbmng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbmng
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
 *   Declaration of several datadbmng for QPF HMI
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
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>

namespace QPF {

class DBManager : public QObject {

    Q_OBJECT

public:
    //explicit DBManager(QObject *parent = 0);
    ~DBManager();

public:
    struct DBConnection {
        QString driverName;
        QString databaseName;
        QString userName;
        QString password;
        QString hostName;
        int port;
    };

    static QSqlError addConnection(const QString &driver, const QString &connectionName,
                                  const QString &dbName, const QString &host,
                                  const QString &user, const QString &passwd, int port);
    static void addConnection(const QString &connectionName, DBConnection & conn);

    inline static QSqlDatabase & getDB() { return db; }

    static QString getState();
    static void setState(QString newState);
    static QMap<QString,QString> getCurrentStates(QString session);

    static int numOfRowsInDbTable(QString tableName);

    static void addICommand(QString cmd);
    static bool getICommand(QString cmd, bool removeCmd = false);
    static void removeICommands(QString cmd);

    static void close();

    //bool getTasksInfo(QMap<QString, QJsonObject> & newTasks, int offset = 0);

signals:
    void newTaskStatusInfo();

private:
    static void checkTaskStatusInfo();
    DBManager();

private:
    static QSqlDatabase       db;
    static QString            activeDb;
    static QMap<QString, int> connections;
};

}

#endif // DBMANAGER_H
