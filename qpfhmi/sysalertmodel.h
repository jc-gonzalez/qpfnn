/******************************************************************************
 * File:    sysalertmodel.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.sysalertmodel
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
 *   Declaration of several datasysalertmodel for QPF HMI
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
#ifndef SYSALERTMODEL_H
#define SYSALERTMODEL_H

#include "dbtblmodel.h"
#include "alert.h"

namespace QPF {

class SysAlertModel : public DBTableModel {

    Q_OBJECT

public:
    explicit SysAlertModel();

    Alert getAlertAt(QModelIndex idx);
};

}

#endif // SYSALERTMODEL_H
