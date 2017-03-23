/******************************************************************************
 * File:    dbtreemodel.cpp
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

#include "dbtreemodel.h"

#include "dbmng.h"
#include <QDebug>

namespace QPF {

DBTreeBoldHeaderDelegate::DBTreeBoldHeaderDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
{
}

void DBTreeBoldHeaderDelegate::paint(QPainter* painter,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    bool shouldBeBold = (! index.parent().isValid());
    opt.font.setBold(shouldBeBold);
    QStyledItemDelegate::paint(painter, opt, index);
}

DBTreeModel::DBTreeModel(QString q, QStringList hdr) :
    queryString(q),
    headerLabels(hdr),
    rowsFromQuery(-1),
    skippedColumns(0),
    boldHeader(false)
{
    refresh();
}

void DBTreeModel::setHeaders(QStringList & hdr)
{
    int i = 0;
    for (QString & s: hdr) {
        setHeaderData(i, Qt::Horizontal, s);
        ++i;
    }
}

void DBTreeModel::defineHeaders(QStringList hdr)
{
    headerLabels = hdr;
}

void DBTreeModel::defineQuery(QString q)
{
    queryString = q;
}

void DBTreeModel::skipColumns(int n)
{
    skippedColumns = n;
}

void DBTreeModel::refresh()
{
    if (! queryString.isEmpty()) {
        execQuery(queryString, DBManager::getDB());
    }
}

void DBTreeModel::execQuery(QString & qry, QSqlDatabase & db)
{
    // Perform query
    QSqlQuery q(qry, db);
    QSqlRecord rec = q.record();
    int fldCount = rec.count();

    if (q.size() == rowsFromQuery) { return; }

    clear();

    // Create root item
    QStandardItem * root = invisibleRootItem();
    QStandardItem * parent;
    QList<QStandardItem *> row;
    QString prevGrp("");

    int children = 0;
    rowsFromQuery = 0;
    while (q.next()) {
#if GROUP_ROW_EMPTY
        QString grp = q.value(0).toString();
        if (prevGrp != grp) {
            parent = new QStandardItem(q.value(skippedColumns).toString());
            row.clear();
            row << parent;
            for (int i = 1; i < fldCount - skippedColumns; ++i) { row << 0; }
            root->appendRow(row);
            children = 0;
            prevGrp = grp;
        }
        row.clear();
        for (int i = skippedColumns; i < fldCount; ++i) {
            row << new QStandardItem(q.value(i).toString());
        }
        parent->appendRow(row);
#else
        QString grp = q.value(0).toString();
        row.clear();
        if (prevGrp != grp) {
            parent = new QStandardItem(q.value(skippedColumns).toString());
            row << parent;
            for (int i = skippedColumns + 1; i < fldCount; ++i) {
                row << new QStandardItem(q.value(i).toString());
            }
            root->appendRow(row);
            children = 0;
            prevGrp = grp;
        } else {
            for (int i = skippedColumns; i < fldCount; ++i) {
                row << new QStandardItem(q.value(i).toString());
            }
            parent->appendRow(row);
        }
#endif
        ++rowsFromQuery;
        ++children;
    }

    if (headerLabels.count() > 0) {
        setHeaders(headerLabels);
    }
}

}
