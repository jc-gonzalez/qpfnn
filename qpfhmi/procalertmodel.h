/******************************************************************************
 * File:    procalertmodel.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.procalertmodel
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
 *   Declaration of several dataprocalertmodel for QPF HMI
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
#ifndef PROCALERTMODEL_H
#define PROCALERTMODEL_H

#include "dbtblmodel.h"

namespace QPF {

class ProcAlertModel : public DBTableModel {

    Q_OBJECT

public:
    explicit ProcAlertModel();

    virtual Alert getAlertAt(QModelIndex idx);
};

}

#endif // PROCALERTMODEL_H
