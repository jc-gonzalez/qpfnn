/******************************************************************************
 * File:    textview.cpp
 *          Implementation of class TextView
 *
 * Domain:  QPF.qpfgui.textview
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
 *   Implementation of class TextView
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

#include <QtWidgets>

#include "textview.h"

TextView::TextView()
{
    setAttribute(Qt::WA_DeleteOnClose);
}

bool TextView::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool TextView::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QPF"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << getTextEditor()->toPlainText();
    QApplication::restoreOverrideCursor();

    return true;
}

void TextView::setLogName(QString n)
{
    name = n;
    setWindowTitle(n);

}

QString TextView::logName()
{
    return name;
}

void TextView::closeEvent(QCloseEvent *event)
{
    event->accept();
}

