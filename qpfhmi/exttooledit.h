/******************************************************************************
 * File:    exttooledit.h
 *          Declaration of class ExtToolEdit
 *
 * Domain:  QPF.qpfgui.exttooledit
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
 *   Declaration of class ExtToolEdit
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

#ifndef EXTTOOLEDIT_H
#define EXTTOOLEDIT_H

#include <QDialog>

#include "hmitypes.h"

namespace Ui {
class ExtToolEdit;
}

namespace QPF {

class ExtToolEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ExtToolEdit(QWidget *parent = 0);
    ~ExtToolEdit();

public:
    void setProdTypes(QStringList pts);
    void editTool(QUserDefTool & udt);
    void getToolInfo(QUserDefTool & udt);

private slots:
    void showHelp();
    void selectApp();
    void selectProdTypes();

private:
    Ui::ExtToolEdit *ui;
    QStringList prodTypes;
};

}

#endif // EXTTOOLEDIT_H
