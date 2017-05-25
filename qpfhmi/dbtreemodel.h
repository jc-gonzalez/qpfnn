/******************************************************************************
 * File:    dbtreemodel.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbtreemodel
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
 *   Declaration of several datadbtreemodel for QPF HMI
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
#ifndef DBTREEMODEL_H
#define DBTREEMODEL_H

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QSqlQuery>
#include <QHash>
#include <QVector>

#include <functional>

namespace QPF {

class DBTreeBoldHeaderDelegate : public QStyledItemDelegate {

    Q_OBJECT

public:
    DBTreeBoldHeaderDelegate(QObject *parent = Q_NULLPTR);
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setCustomFilter(bool b);
protected:
    bool        isCustomFilter;
};

class DBTreeModel : public QStandardItemModel {

    Q_OBJECT

public:
    explicit DBTreeModel(QString q = QString(""),
                         QStringList hdr = QStringList());

    virtual void refresh();

    virtual void defineHeaders(QStringList hdr);
    virtual void defineQuery(QString q);
    virtual void skipColumns(int n = 0);
    virtual void setCustomFilter(bool b);
    virtual void setBoldHeader(bool b = false);
    virtual void restart();

protected:
    virtual void setHeaders(QStringList & hdr);

    //virtual QList<QStandardItem *> prepareRow(QStringList & l) = 0;
    virtual void execQuery(QString & qry, QSqlDatabase & db);
    
    QString     queryString;
    QStringList headerLabels;
    int         rowsFromQuery;
    int         skippedColumns;
    bool        boldHeader;
    QString     initialQuery;
    QStringList initialHeaders;
    int         initialSkippedColumns;

    std::function<QString(QSqlQuery&)> getGroupId;
    bool        isCustomFilter;
};

}

#endif // DBTREEMODEL_H
