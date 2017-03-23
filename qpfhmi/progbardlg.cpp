/******************************************************************************
 * File:    progbardlg.cpp
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
 *   Implement ProgressBarDelegate class
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

#include "progbardlg.h"

#include <QDebug>
#include <QApplication>

namespace QPF {

ProgressBarDelegate::ProgressBarDelegate(QObject * parent, int col)
    : QItemDelegate(parent), progressColumn(col)
{
}

ProgressBarDelegate::~ProgressBarDelegate()
{
}

QSize ProgressBarDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QSize defaultSize(QItemDelegate::sizeHint(option, index));
    if (index.column() == progressColumn) { defaultSize.setWidth(100); }
    return defaultSize;
}

void ProgressBarDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    //QProgressBar progressbar;

    if (index.column() != progressColumn) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionProgressBarV2 progressBarOption;

    QRect rect = option.rect;
    rect.setWidth(rect.width());

    progressBarOption.state = QStyle::State_Enabled;
    progressBarOption.direction = QApplication::layoutDirection();
    progressBarOption.rect = rect;
    progressBarOption.fontMetrics = QApplication::fontMetrics();
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.textVisible = true;

    //QModelIndex ind = index.parent();
    int progress = index.data(Qt::DisplayRole).toInt();
    // Set the progress and text values of the style option.
    progressBarOption.progress = progress < 0 ? 0 : progress;
    progressBarOption.text = QString().sprintf("%d%%", progressBarOption.progress);

    // Draw the progress bar onto the view.
    QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                       &progressBarOption, painter);
}

}
