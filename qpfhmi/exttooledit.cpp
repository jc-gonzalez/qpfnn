/******************************************************************************
 * File:    exttooledit.cpp
 *          Implementation of class ExtToolEdit
 *
 * Domain:  QPF.qpfgui.exttooledit
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
 *   Implementation of class ExtToolEdit
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

#include "exttooledit.h"
#include "ui_exttooledit.h"

#include <QListWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

namespace QPF {

ExtToolEdit::ExtToolEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtToolEdit)
{
    ui->setupUi(this);
}

ExtToolEdit::~ExtToolEdit()
{
    delete ui;
}

void ExtToolEdit::setProdTypes(QStringList pts)
{
    prodTypes = pts;
}

void ExtToolEdit::editTool(QUserDefTool & udt)
{
    ui->edName->setText(udt.name);
    ui->edDesc->setText(udt.desc);
    ui->edExe->setText(udt.exe);
    ui->edArgs->setText(udt.args);
    ui->edProdTypes->setText(udt.prod_types.join("|"));
}

void ExtToolEdit::getToolInfo(QUserDefTool & udt)
{
    udt.name       = ui->edName->text();
    udt.desc       = ui->edDesc->text();
    udt.exe        = ui->edExe->text();
    udt.args       = ui->edArgs->text();
    udt.prod_types = ui->edProdTypes->text().split("|");
}

void ExtToolEdit::showHelp()
{
    QString helpMsg =
        tr("You can specify a user defined tool following these simple steps:\n"
           "1. Enter the name and the description of the tool in the"
           "corresponding fields.\n"
           "2. Then, click on the button [...] next to the ""'Executable' field, "
           "in order to select the tool executable file.\n"
           "3. Enter an argument template (follow instructions below).\n"
           "4. Finally, click on the button [...] next to the 'Products' field "
           "the products associated to this tool.\n\n"
           "You can use the following template placeholders "
           "to specify the arguments for the user defined tool:\n"
           "  %f\tProduct file name (without path)\n"
           "  %F\tProduct complete file name (with path)\n"
           "  %p\tPath where the product file is located\n"
           "  %i\tProduct Id\n"
           "  %o\tObservation Id\n"
           "  %s\tStart time stamp\n"
           "  %e\tEnd time stamp\n"
           "  %t\tProduct type\n"
           "  %x\tProduct file extension\n"
           "  %1 - %9\tUser-requested inputs");

    (void)QMessageBox::information(this, tr("How to specify a user defined tool"),
                                   helpMsg, QMessageBox::Ok);
}

void ExtToolEdit::selectApp()
{
    QString fileName =
            QFileDialog::getOpenFileName(this,
                                         tr("Select user defined tool executable"),
                                         "/");
    if (! fileName.isEmpty()) {
        ui->edExe->setText(fileName);
    }
}

void ExtToolEdit::selectProdTypes()
{
    QLabel *lbl = new QLabel("Select product types:");

    QListWidget * lstProds = new QListWidget();
    lstProds->setSelectionMode(QAbstractItemView::MultiSelection);
    QStringList associatedProds = ui->edProdTypes->text().split("|");
    foreach (QString p, prodTypes) {
        QListWidgetItem * item = new QListWidgetItem(p, lstProds);
        item->setSelected(associatedProds.contains(p));
    }
    lstProds->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QPushButton *btnOk = new QPushButton("&Ok");
    QPushButton *btnCancel = new QPushButton("&Cancel");
    QHBoxLayout *lyBtn = new QHBoxLayout;
    lyBtn->addWidget(btnOk);
    lyBtn->addWidget(btnCancel);
    QVBoxLayout *lyDlg = new QVBoxLayout;
    lyDlg->addWidget(lbl);
    lyDlg->addWidget(lstProds);
    lyDlg->addLayout(lyBtn);

    QDialog dlg;
    dlg.setLayout(lyDlg);
    dlg.setWindowTitle(tr("Select product types"));

    connect(btnOk, SIGNAL(clicked()), &dlg, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), &dlg, SLOT(reject()));
    //dlg.resize(200, 300);

    if (dlg.exec()) {
        QList<QListWidgetItem *> items = lstProds->selectedItems();
        QStringList pts;
        foreach (QListWidgetItem * it, items) {
            pts << it->data(0).toString();
        }
        ui->edProdTypes->setText(pts.join("|"));
    }
}

}
