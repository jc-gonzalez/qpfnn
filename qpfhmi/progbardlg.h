/******************************************************************************
 * File:    progbardlg.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.progbardlg
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
 *   Declare ProgressBarDelegate class
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

#ifndef PROGBARDLG_H
#define PROGBARDLG_H

#include <QItemDelegate>
#include <QProgressBar>

namespace QPF {

class ProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ProgressBarDelegate(QObject * parent = 0, int col = 0);
    ~ProgressBarDelegate();
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

private:
    int progressColumn;
};

}

#endif
