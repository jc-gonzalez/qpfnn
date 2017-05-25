/******************************************************************************
 * File:    exttoolsdef.cpp
 *          Implementation of class ExtToolsDef
 *
 * Domain:  QPF.qpfgui.exttoolsdef
 *
 * Version:  1.2
 *
 * Date:    2016-11-03
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implementation of class ExtToolsDef
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

#include "exttoolsdef.h"
#include "ui_exttoolsdef.h"

#include <QMessageBox>
#include <QDebug>

namespace QPF {

ExtToolsDef::ExtToolsDef(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtToolsDef)
{
    ui->setupUi(this);
}

ExtToolsDef::~ExtToolsDef()
{
    delete ui;
}

void ExtToolsDef::initialize(MapOfUserDefTools & userTools, QStringList pts)
{
    ui->tblwdgUserDefTools->clear();
    ui->tblwdgUserDefTools->setRowCount(userTools.count());
    ui->tblwdgUserDefTools->setColumnCount(5);
    ui->tblwdgUserDefTools->setHorizontalHeaderLabels(QStringList()
                                                      << "Name"
                                                      << "Description"
                                                      << "Executable"
                                                      << "Arguments"
                                                      << "Product types");
    int row = 0;
    foreach (QString key, userTools.keys()) {
        const QUserDefTool & udt = userTools.value(key);
        ui->tblwdgUserDefTools->setItem(row, 0, new QTableWidgetItem(udt.name));
        ui->tblwdgUserDefTools->setItem(row, 1, new QTableWidgetItem(udt.desc));
        ui->tblwdgUserDefTools->setItem(row, 2, new QTableWidgetItem(udt.exe));
        ui->tblwdgUserDefTools->setItem(row, 3, new QTableWidgetItem(udt.args));
        ui->tblwdgUserDefTools->setItem(row, 4, new QTableWidgetItem(udt.prod_types.join(QString("|"))));
        ++row;
    }
    userDefTools = userTools;
    origDefTools = userTools;
    prodTypes = pts;
    connect(ui->tblwdgUserDefTools, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(changeToolWithItem(QTableWidgetItem*)));
}

void ExtToolsDef::addNewTool()
{
    ExtToolEdit dlg;
    dlg.setProdTypes(prodTypes);
    if (dlg.exec()) {
        // Create new tool and append to list in table
        QUserDefTool udt;
        dlg.getToolInfo(udt);
        int row = ui->tblwdgUserDefTools->rowCount();
        ui->tblwdgUserDefTools->insertRow(row);

        ui->tblwdgUserDefTools->setItem(row, 0, new QTableWidgetItem(udt.name));
        ui->tblwdgUserDefTools->setItem(row, 2, new QTableWidgetItem(udt.desc));
        ui->tblwdgUserDefTools->setItem(row, 1, new QTableWidgetItem(udt.exe));
        ui->tblwdgUserDefTools->setItem(row, 3, new QTableWidgetItem(udt.args));
        ui->tblwdgUserDefTools->setItem(row, 4, new QTableWidgetItem(udt.prod_types.join(QString("|"))));

        userDefTools[udt.name] = udt;
    }
}

void ExtToolsDef::editTool(QModelIndex idx)
{
    int row = idx.row();
    editTool(row);
}

void ExtToolsDef::editTool()
{
    QList<QTableWidgetItem*> items = ui->tblwdgUserDefTools->selectedItems();
    int row = items.first()->row();
    editTool(row);
}

void ExtToolsDef::editTool(int row)
{
    QString name = ui->tblwdgUserDefTools->item(row, 0)->data(0).toString();
    QUserDefTool udt = userDefTools[name];
    ExtToolEdit dlg;
    dlg.setProdTypes(prodTypes);
    dlg.editTool(udt);
    if (dlg.exec()) {
        // Create new tool and append to list in table
        dlg.getToolInfo(udt);

        ui->tblwdgUserDefTools->item(row, 0)->setData(0, udt.name);
        ui->tblwdgUserDefTools->item(row, 1)->setData(0, udt.desc);
        ui->tblwdgUserDefTools->item(row, 2)->setData(0, udt.exe);
        ui->tblwdgUserDefTools->item(row, 3)->setData(0, udt.args);
        ui->tblwdgUserDefTools->item(row, 4)->setData(0, udt.prod_types.join(QString("|")));

        userDefTools[udt.name] = udt;
    }
}

void ExtToolsDef::changeToolWithItem(QTableWidgetItem * item)
{
    QString content = item->data(0).toString();
    QString name = ui->tblwdgUserDefTools->item(item->row(), 0)->data(0).toString();
    QUserDefTool & udt = const_cast<QUserDefTool&>(userDefTools[name]);
    switch (item->column()) {
    case 0: udt.name       = content; break;
    case 1: udt.desc       = content; break;
    case 2: udt.exe        = content; break;
    case 3: udt.args       = content; break;
    case 4: udt.prod_types = content.split("|"); break;
    default: break;
    }
}

void ExtToolsDef::removeTool()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Remove tool from list");
    msgBox.setText("You requested to remove selected user defined tool from the list.");
    msgBox.setInformativeText("Do you really want to remove this tool?");
    QPushButton *removeButton = msgBox.addButton(tr("Remove tool"), QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(QMessageBox::Abort);

    msgBox.exec();
    if (msgBox.clickedButton() == removeButton) {
        QList<QTableWidgetItem*> items = ui->tblwdgUserDefTools->selectedItems();
        int row = items.first()->row();
        QString name = ui->tblwdgUserDefTools->item(row, 0)->data(0).toString();
        ui->tblwdgUserDefTools->removeRow(row);
        userDefTools.remove(name);
    } else if (msgBox.clickedButton() == cancelButton) {
        return;
    }
}

void ExtToolsDef::cancelDlg()
{
    userDefTools = origDefTools;
}

void ExtToolsDef::getTools(MapOfUserDefTools & userTools)
{
    userTools = userDefTools;
}

}
