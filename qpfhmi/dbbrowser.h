/******************************************************************************
 * File:    dbbrowser.h
 *          Declaration of class DBBrowser
 *
 * Domain:  QPF.qpfgui.dbbrowser
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
 *   Declaration of class DBBrowser
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

#ifndef DBBROWSER_H
#define DBBROWSER_H

#include <QDialog>

namespace Ui {
class DBBrowser;
}

class DBBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit DBBrowser(QWidget *parent = 0);
    ~DBBrowser();

private:
    Ui::DBBrowser *ui;
};

#endif // DBBROWSER_H
