/******************************************************************************
 * File:    dbtblmodel.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbtblmodel
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

#include "dbtblmodel.h"

#include "dbmng.h"

namespace QPF {

DBTableModel::DBTableModel(QString q, QStringList hdr, DBTableModel::TablePalette pal) :
    queryString(q),
    headerLabels(hdr),
    tblPalette(pal),
    rowsFromQuery(-1),
    headerIsSet(false),
    fullUpdate(false)
{
    refresh();
}

void DBTableModel::setHeaders(QStringList & hdr)
{
    int i = 0;
    for (QString & s: hdr) {
        setHeaderData(i, Qt::Horizontal, s);
        ++i;
    }
}

void DBTableModel::defineHeaders(QStringList hdr)
{
    headerLabels = hdr;
}

void DBTableModel::defineQuery(QString q)
{
    queryString = q;
}

void DBTableModel::defineTablePalette(DBTableModel::TablePalette pal)
{
    tblPalette = pal;
}

QVariant DBTableModel::data(const QModelIndex &index, int role) const
{
    QVariant content = QSqlQueryModel::data(index, role);
    switch(role) {
    case Qt::BackgroundRole:
    case Qt::ForegroundRole:
        for (int col: tblPalette.keys()) {
            if (index.column() == col) {
                ColumnPalette colPalette = tblPalette[col];
                for (QString key: colPalette.keys()) {
                    if (QSqlQueryModel::data(index, Qt::DisplayRole).toString() == key) {
                        FgBgColors fb = colPalette[key];
                        if (role == Qt::BackgroundRole) {
                            return QVariant(fb.bgColor);
                        } else {
                            return QVariant(fb.fgColor);
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    return content;
}


void DBTableModel::setFullUpdate(bool b)
{
    fullUpdate = b;
}

void DBTableModel::refresh()
{
    if (! queryString.isEmpty()) {
        QSqlQuery qry(queryString, DBManager::getDB());
        if (fullUpdate || (rowsFromQuery != qry.size())) {
            rowsFromQuery = qry.size();
            setQuery(qry);
            headerIsSet = false;
        }
    }

    if (! headerIsSet) {
        if (headerLabels.count() > 0) {
            setHeaders(headerLabels);
            headerIsSet = true;
        }
    }
}

}
