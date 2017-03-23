/******************************************************************************
 * File:    txtblmodel.h
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
 *   Declaration of several datatxtblmodel for QPF HMI
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
#ifndef TXTABLEMODEL_H
#define TXTABLEMODEL_H

#include "dbtblmodel.h"

namespace QPF {

class TxTableModel : public DBTableModel {

    Q_OBJECT

public:
    explicit TxTableModel(QStringList nodes);

    virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;

private:
    QMap<QString, int> nodeOrder;
};

}

#endif // TXTABLEMODEL_H
