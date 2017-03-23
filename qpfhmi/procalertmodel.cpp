/******************************************************************************
 * File:    procalertmodel.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.procalertmodel
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

#include "procalertmodel.h"

namespace QPF {

ProcAlertModel::ProcAlertModel()
{
    defineQuery("SELECT "
                "a.alert_id AS ID, "
                "a.creation AS created, "
                "a.sev AS severity, "
                "a.typ AS type, "
                "a.origin AS origin, "
                "a.msgs AS addinfo, "
                "a.var AS variable "
                "FROM alerts a "
                "WHERE a.grp = 'Diagnostics' "
                "ORDER BY a.alert_id;");
    defineHeaders({"ID", "Created", "Severity",
                "Type", "Origin", "Add.Info",
                "Variable"});

    refresh();
}

}
