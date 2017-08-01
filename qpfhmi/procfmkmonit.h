/******************************************************************************
 * File:    procfmkmonit.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.qpfgui.ProcFmkMonitor
 *
 * Version:  1.2
 *
 * Date:    2017/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare ProcFmkMonitor class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
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

#ifndef PROCFMKMONIT_H
#define PROCFMKMONIT_H

#include <QScrollArea>
#include <QTimer>
#include <QVector>

#include "qsimpletickergraph.h"
#include "procinfo.h"
#include "frmhostinfo.h"
//#include "datatypes.h"

namespace QPF {

struct Graphs {
    QSimpleTickerGraph * loadGraph;
    QSimpleTickerGraph * cpuGraph;
    QSimpleTickerGraph * tasksGraph;
    double               lastLoad;
    double               lastCPU;
    double               lastTasks;
};

class ProcFmkMonitor : public QObject
{
    Q_OBJECT

public:
    explicit ProcFmkMonitor(QScrollArea * scrl);

public:
    void setupHostsInfo(ProcessingFrameworkInfo * q);

protected slots:
    void timeout();

 private:
    void appendPoint(QSimpleTickerGraph * grph, double value);

private:
    QScrollArea * scrollArea;
    QTimer *      mTimer;

    ProcessingFrameworkInfo * procFmkInfo;

    double mVoltage;
    double mTemperature;
    double mSpeed;

    QVector<Graphs> graphs;
    QVector<FrmHostInfo *> panels;
};

}

#endif // PROCFMKMONIT_H
