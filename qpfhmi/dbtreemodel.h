/******************************************************************************
 * File:    dbtreemodel.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbtreemodel
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

namespace QPF {

class DBTreeBoldHeaderDelegate : public QStyledItemDelegate {

    Q_OBJECT

public:
    DBTreeBoldHeaderDelegate(QObject *parent = Q_NULLPTR);
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const Q_DECL_OVERRIDE;
};

class DBTreeModel : public QStandardItemModel {

    Q_OBJECT

public:
    explicit DBTreeModel(QString q = QString(""),
                         QStringList hdr = QStringList());

    void refresh();

    void defineHeaders(QStringList hdr);
    void defineQuery(QString q);
    void skipColumns(int n = 0);
    void setBoldHeader(bool b = false);

protected:
    void setHeaders(QStringList & hdr);

    //virtual QList<QStandardItem *> prepareRow(QStringList & l) = 0;
    virtual void execQuery(QString & qry, QSqlDatabase & db);

    QString     queryString;
    QStringList headerLabels;
    int         rowsFromQuery;
    int         skippedColumns;
    bool        boldHeader;
};

}

#endif // DBTREEMODEL_H
