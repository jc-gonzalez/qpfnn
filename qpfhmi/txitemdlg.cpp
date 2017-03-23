/******************************************************************************
 * File:    txitemdlg.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.txitemdlg
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
 *   Implement TxItemDelegate class
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

#include "txitemdlg.h"
#include "txitem.h"

#include <QDebug>

namespace QPF {

TxItemDelegate::TxItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TxItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if (index.column() > 5) {
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        TxItem txItem;
        int value = index.data(Qt::DisplayRole).toInt();
        if (value > 200) {
            QString msgName = index.model()->index(index.row(), 4).data().toString().trimmed();
            QJsonObject msgContent = index.model()->index(index.row(), 5).data().toJsonObject();
            txItem.setMsgName(msgName);
            txItem.setContent(msgContent);
        }
        switch (value) {
        case -1:
            txItem.setMode(TxItem::Empty);
            break;
        case 0:
            txItem.setMode(TxItem::Line);
            break;
        case 101:
            txItem.setOrientation(TxItem::FromLeftToRight);
            txItem.setMode(TxItem::Source);
            break;
        case 102:
            txItem.setOrientation(TxItem::FromRightToLeft);
            txItem.setMode(TxItem::Source);
            break;
        case 201:
            txItem.setOrientation(TxItem::FromLeftToRight);
            txItem.setMode(TxItem::Target);
            break;
        case 202:
            txItem.setOrientation(TxItem::FromRightToLeft);
            txItem.setMode(TxItem::Target);
            break;
        default:
            break;
        }
        txItem.paint(painter, option.rect, option.palette);
   } else {
       QStyledItemDelegate::paint(painter, option, index);
   }
}

QSize TxItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (index.column() > 5) {
        TxItem txItem;
        return txItem.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

QWidget *TxItemDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    return Q_NULLPTR;
/*
    if (index.data().canConvert<StarRating>()) {
        StarEditor *editor = new StarEditor(parent);
        connect(editor, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
*/
}

void TxItemDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    // if (index.data().canConvert<StarRating>()) {
    //     StarRating starRating = qvariant_cast<StarRating>(index.data());
    //     StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
    //     starEditor->setStarRating(starRating);
    // } else {
        QStyledItemDelegate::setEditorData(editor, index);
    // }
}

void TxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    // if (index.data().canConvert<StarRating>()) {
    //     StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
    //     model->setData(index, QVariant::fromValue(starEditor->starRating()));
    // } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    // }
}

// void TxItemDelegate::commitAndCloseEditor()
// {
//     StarEditor *editor = qobject_cast<StarEditor *>(sender());
//     emit commitData(editor);
//     emit closeEditor(editor);
// }

}
