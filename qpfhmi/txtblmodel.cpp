/******************************************************************************
 * File:    txtblmodel.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.txtblmodel
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

#include "txtblmodel.h"

#include <QDebug>

namespace QPF {

TxTableModel::TxTableModel(QStringList nodes)
{
    // Build query
    QStringList qry {"SELECT id, msg_date, msg_from, msg_to, msg_type, msg_content"};
    int i = 6;
    foreach (QString node, nodes) {
        qry << ", CASE WHEN msg_from = '" << node << "' THEN 1 "
            << "       WHEN msg_to   = '" << node << "' THEN 2 "
            << "       ELSE 0 END AS " << node;
        nodeOrder[node] = i;
        ++i;
    }
    qry << " FROM transmissions;";
    defineQuery(qry.join(""));

    defineHeaders(QStringList() << "ID" << "TimeStamp" << "Source" << "Target" << "Type" << "Content"
                  << nodes
                  );

    refresh();
}

QVariant TxTableModel::data(const QModelIndex &idx, int role) const
{
    int col = idx.column();

    if (col < 6) {
        return QSqlQueryModel::data(idx, role);
    }

    int row = idx.row();
    QModelIndex idxFrom = this->index(row, 2);
    QModelIndex idxTo   = this->index(row, 3);
    QString from = QSqlQueryModel::data(idxFrom, Qt::DisplayRole).toString().trimmed();
    QString to   = QSqlQueryModel::data(idxTo,   Qt::DisplayRole).toString().trimmed();
    int orderFrom = nodeOrder[from];
    int orderTo   = nodeOrder[to];
    int leftMost = qMin(orderFrom, orderTo);
    int rightMost = qMax(orderFrom, orderTo);

    int value = QSqlQueryModel::data(idx, Qt::DisplayRole).toInt();
    int finalValue = value;

    switch (role) {
    case Qt::DisplayRole:
        if (value == 0) {
            finalValue = ((col < leftMost) || (col > rightMost)) ? -1 : 0;
        } else if (value == 1) {
            finalValue = (orderFrom < orderTo) ? 101 : 102;
        } else if (value == 2) {
            finalValue = (orderFrom < orderTo) ? 201 : 202;
        } else {
            finalValue = 0;
        }
        return QVariant(finalValue);
    default:
        break;
    }
    return QSqlQueryModel::data(idx, role);
}

}
