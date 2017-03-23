/******************************************************************************
 * File:    dlgshowtaskinfo.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.QPF.dlgshowtaskinfo
 *
 * Version:  1.1
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   
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
#ifndef DLGSHOWTASKINFO_H
#define DLGSHOWTASKINFO_H

#include <QDialog>
#include <QTreeWidget>

namespace Ui {
class DlgShowTaskInfo;
}

class DlgShowTaskInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgShowTaskInfo(QWidget *parent = 0);
    ~DlgShowTaskInfo();

    QTreeWidget * getTreeTaskInfo();
    void setTaskInfo(QString info);

private:
    Ui::DlgShowTaskInfo *ui;
};

#endif // DLGSHOWTASKINFO_H
