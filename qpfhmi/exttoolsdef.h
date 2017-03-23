/******************************************************************************
 * File:    exttoolsdef.h
 *          Declaration of class ExtToolsDef
 *
 * Domain:  QPF.qpfgui.exttoolsdef
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
 *   Declaration of class ExtToolsDef
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

#ifndef EXTTOOLSDEF_H
#define EXTTOOLSDEF_H

#include <QDialog>
#include <QModelIndex>
#include <QTableWidgetItem>

#include "exttooledit.h"

namespace Ui {
class ExtToolsDef;
}

namespace QPF {

class ExtToolsDef : public QDialog
{
    Q_OBJECT

public:
    explicit ExtToolsDef(QWidget *parent = 0);
    ~ExtToolsDef();

    void initialize(MapOfUserDefTools & userTools, QStringList pts);
    void getTools(MapOfUserDefTools & userTools);

private slots:
    void addNewTool();
    void editTool(QModelIndex idx);
    void editTool();
    void editTool(int row);
    void removeTool();
    void cancelDlg();
    void changeToolWithItem(QTableWidgetItem * item);

private:
    Ui::ExtToolsDef *ui;
    MapOfUserDefTools userDefTools;
    MapOfUserDefTools origDefTools;
    QStringList       prodTypes;
};

}

#endif // EXTTOOLSDEF_H
