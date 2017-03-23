/******************************************************************************
 * File:    dbtblmodel.h
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
 *   Declaration of several datadbtblmodel for QPF HMI
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
#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include <QSqlQueryModel>
#include <QColor>

namespace QPF {

class DBTableModel : public QSqlQueryModel {

    Q_OBJECT

public:

    struct FgBgColors {
        FgBgColors(QColor fg = QColor(), QColor bg = QColor()) : fgColor(fg), bgColor(bg) {}
        QColor fgColor;
        QColor bgColor;
    };

    typedef QMap<QString, FgBgColors> ColumnPalette;
    typedef QMap<int, ColumnPalette>  TablePalette;

    explicit DBTableModel(QString q = QString(""),
                          QStringList hdr = QStringList(),
                          TablePalette pal = TablePalette());

    void refresh();

    void defineHeaders(QStringList hdr);
    void defineQuery(QString q);
    void defineTablePalette(TablePalette pal);
    void setFullUpdate(bool b);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
    void setHeaders(QStringList & hdr);

    QString       queryString;
    QStringList   headerLabels;
    TablePalette  tblPalette;
    int           rowsFromQuery;
    bool          headerIsSet;
    bool          fullUpdate;
};

}

#endif // DBTABLEMODEL_H
