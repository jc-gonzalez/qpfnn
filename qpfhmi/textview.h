/******************************************************************************
 * File:    textview.h
 *          Declaration of class TextView
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
 *   Declaration of class TextView
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

#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QtWidgets>
#include "logframe.h"

//class TextView : public QPlainTextEdit
class TextView : public LogFrame
{
    Q_OBJECT

public:
    TextView();

    bool saveAs();
    bool saveFile(const QString &fileName);
    void setLogName(QString n);
    QString logName();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    QString name;
};

#endif // TEXTVIEW_H

