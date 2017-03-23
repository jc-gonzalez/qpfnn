/******************************************************************************
 * File:    verbleveldlg.h
 *          Declaration of class VerbLevelDlg
 *
 * Domain:  QPF.qpfgui.verbleveldlg
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
 *   Declaration of class VerbLevelDlg
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

#ifndef VERBLEVELDLG_H
#define VERBLEVELDLG_H

#include <QDialog>

namespace Ui {
class VerbLevelDlg;
}

namespace QPF {

class VerbLevelDlg : public QDialog
{
    Q_OBJECT

public:
    explicit VerbLevelDlg(QWidget *parent = 0);
    ~VerbLevelDlg();
    QString getVerbosityLevelName();
    int getVerbosityLevelIdx();

public slots:
    void setVerbosityLevel(int lvl);

private:
    Ui::VerbLevelDlg *ui;
    QStringList verbLevels;
    QString currentLevel;
    int currentLevelIdx;
};

}

#endif // VERBLEVELDLG_H
