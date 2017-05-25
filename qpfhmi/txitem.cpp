/******************************************************************************
 * File:    txitem.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.txitem
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement TxItem class
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

#include "txitem.h"

#include <QPainter>

namespace QPF {

TxItem::TxItem()
{
}

void TxItem::setMode(Mode m)
{
    mode = m;
}

void TxItem::setOrientation(Orientation o)
{
    orientation = o;
}

void TxItem::setMsgName(QString n)
{
    name = n;
}

void TxItem::setContent(QJsonObject o)
{
    content = o;
}

QSize TxItem::sizeHint() const
{
    return QSize(100, 28); //PaintingScaleFactor * QSize(myMaxStarCount, 1);
}

void TxItem::paint(QPainter *painter, const QRect &rect,
                       const QPalette &palette) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    //painter->setPen(Qt::NoPen);
    int x1 = rect.left();
    int x2 = rect.left() + rect.width();
    int x  = rect.left() + rect.width() / 2;
    int y  = rect.top() + rect.height() / 2;
    QRect rectDraw(rect.left() + 10,rect.top() + 5,
                   rect.width() - 20, rect.height() - 10);
    QRect rectText(rect.left() + 12,rect.top() + 6,
                   rect.width() - 24, rect.height() - 12);
    //QString msgText = QString("%1").arg("MESSAGE");
    QFont myFont("Helvetica", 7);
    QPolygonF arrow;

    switch (mode) {
    case Line:
        painter->setPen(Qt::blue);
        painter->drawLine(x1, y, x2, y);
        break;
    case Source:
        painter->setPen(Qt::blue);
        if (orientation == FromLeftToRight) {
            painter->drawLine(x, y, x2, y);
        } else {
            painter->drawLine(x1, y, x, y);
        }
        painter->setBrush(Qt::red);
        painter->drawEllipse(x - 5, y - 5, 10, 10);
        break;
    case Target:
        // Compute message name rectangle
        painter->setFont(myFont);
        painter->setPen(Qt::blue);
        painter->setBrush(Qt::yellow);
        painter->drawRect(rectDraw);
        painter->setPen(Qt::black);
        painter->drawText(rectText, Qt::AlignHCenter | Qt::AlignVCenter, name);
        // Show arrow
        if (orientation == FromLeftToRight) {
            arrow << QPointF(x1, y) << QPointF(x1 + 10, y)
                  << QPointF(x1 + 4, y - 2) << QPointF(x1 + 6, y) << QPointF(x1 + 4, y + 2)
                  << QPointF(x1 + 10, y) << QPointF(x1, y);
        } else {
            arrow << QPointF(x2, y) << QPointF(x2 - 10, y)
                  << QPointF(x2 - 4, y - 2) << QPointF(x2 - 6, y) << QPointF(x2 - 4, y + 2)
                  << QPointF(x2 - 10, y) << QPointF(x2, y);
        }
        painter->drawPolygon(arrow);
        break;
    case Empty:
    default:
        break;
    }

    painter->restore();
}

}
