/******************************************************************************
 * File:    txitem.h
 *          Declaration of class TxItem
 *
 * Domain:  QPF.qpfgui.txitem
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
 *   Declaration of class TxItem
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

#ifndef TXITEM_H
#define TXITEM_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class TxItem;
}

namespace QPF {

class TxItem
{
public:
    TxItem();

    enum Mode { Empty, Line, Source, Target };
    enum Orientation { FromLeftToRight, FromRightToLeft };

    void setMode(Mode m);
    void setOrientation(Orientation o);
    void setMsgName(QString n);
    void setContent(QJsonObject o);

    QSize sizeHint() const;
    void paint(QPainter *painter, const QRect &rect,
               const QPalette &palette) const;

private:
    Ui::TxItem *ui;
    Mode mode;
    Orientation orientation;
    QString name;
    QJsonObject content;
};

}

#endif // TXITEM_H
