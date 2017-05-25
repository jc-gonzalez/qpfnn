/******************************************************************************
 * File:    procalertmodel.cpp
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
                "a.msgs AS addinfo "
                "FROM alerts a "
                "WHERE a.grp = 'Diagnostics' "
                "ORDER BY a.alert_id;");
    defineHeaders({"ID", "Created", "Severity",
                "Type", "Origin", "Add.Info"});

    refresh();
}

Alert ProcAlertModel::getAlertAt(QModelIndex idx)
{
    QStringList listOfMsgs = index(idx.row(), 5).data().toString().split('\n');
    Alert::Messages msgs;
    foreach (QString m, listOfMsgs) { msgs.push_back(m.toStdString()); }
    Alert a(Alert::now(),
            Alert::System,
            Alert::SeverityIdx[index(idx.row(), 2).data().toString().toStdString()],
            Alert::TypeIdx[index(idx.row(), 3).data().toString().toStdString()],
            index(idx.row(), 4).data().toString().toStdString(),
            msgs);
    return a;
}

}
