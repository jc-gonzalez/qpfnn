/******************************************************************************
 * File:    dlgalert.h
 *          Declaration of class DlgAlert
 *
 * Domain:  QPF.qpfgui.dlgalert
 *
 * Version:  1.1
 *
 * Date:    2016-11-03
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declaration of class DlgAlert
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
 *   See <ChangeLog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef DLGALERT_H
#define DLGALERT_H

#include <QDialog>
#include <QDateTime>

#include "alert.h"

namespace Ui {
class DlgAlert;
}

namespace QPF {

class DlgAlert : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAlert(QWidget *parent = 0);
    ~DlgAlert();

    void setAlert(Alert & alert);

private:
    Ui::DlgAlert *ui;
};
}

#endif // DLGALERT_H
