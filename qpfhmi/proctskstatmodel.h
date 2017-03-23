/******************************************************************************
 * File:    proctskstatmodel.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.proctskstatmodel
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
 *   Declaration of several dataproctskstatmodel for QPF HMI
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
#ifndef PROCTASKSTATUSMODEL_H
#define PROCTASKSTATUSMODEL_H

#include "dbtblmodel.h"

namespace QPF {

class ProcTaskStatusModel : public DBTableModel {

    Q_OBJECT

public:
    explicit ProcTaskStatusModel();
};

}

#endif // PROCTASKSTATUSMODEL_H
