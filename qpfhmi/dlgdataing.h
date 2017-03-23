/******************************************************************************
 * File:    dlgdataing.h
 *          Declaration of class DlgDataIngestion
 *
 * Domain:  QPF.qpfgui.dlgdataing
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
 *   Declaration of class DlgDataIngestion
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

#ifndef DLGDATAING_H
#define DLGDATAING_H

#include <QDialog>

namespace Ui {
class DlgDataIngestion;
}

class DlgDataIngestion : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDataIngestion(QWidget *parent = 0);
    ~DlgDataIngestion();

private:
    Ui::DlgDataIngestion *ui;
};

#endif // DLGDATAING_H
