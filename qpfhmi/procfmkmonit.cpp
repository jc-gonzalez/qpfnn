/******************************************************************************
 * File:    procfmkmonit.cpp
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

#include "procfmkmonit.h"

#include "frmhostinfo.h"
#include "frmagentstatus.h"

#include <QVBoxLayout>
#include <QSpacerItem>

namespace QPF {

const int GRAPH_UPDATE_PERIOD = 100;

ProcFmkMonitor::ProcFmkMonitor(QScrollArea * scrl) :
    scrollArea(scrl), mTimer(new QTimer())
{
}

void ProcFmkMonitor::setupHostsInfo(ProcessingFrameworkInfo * q)
{
    procFmkInfo = q;

    QFrame * frm = new QFrame(0);
    QVBoxLayout * vlyFrmAgents = new QVBoxLayout;
    frm->setLayout(vlyFrmAgents);
    scrollArea->setWidget(frm);

    QSpacerItem * spacerFrmAgents = new QSpacerItem(10, 10,
                                                    QSizePolicy::Minimum,
                                                    QSizePolicy::Expanding);
    //vlyFrmAgents->addSpacerItem(spacerFrmAgents);

    // Add widgets for hosts
    for (auto & kv : procFmkInfo->hostsInfo) {
        ProcessingHostInfo & ph = (*(kv.second));
        FrmHostInfo * panel = new FrmHostInfo(0);
        vlyFrmAgents->addWidget(panel);

        Graphs g;
        g.loadGraph = panel->getLoadGraph();
        g.loadGraph->setUnits("");
        g.loadGraph->setRange(0, 10);
        g.loadGraph->setReferencePoints(QList<double>() << 5);

        g.cpuGraph = panel->getCPUGraph();
        g.cpuGraph->setUnits("%");
        g.cpuGraph->setRange(0, 150);
        g.cpuGraph->setReferencePoints(QList<double>() << 100);
        g.cpuGraph->setBackgroundBrush(QBrush(QColor(10, 0, 72)));

        g.tasksGraph = panel->getTasksGraph();
        g.tasksGraph->setUnits("");
        g.tasksGraph->setRange(0, 100);
        g.tasksGraph->setReferencePoints(QList<double>() << 50);
        g.tasksGraph->setBackgroundBrush(QBrush(QColor(80, 10, 12)));

        panel->update(ph);
        graphs.append(g);

    }

    vlyFrmAgents->addSpacerItem(spacerFrmAgents);

    // Periodically update the graphs with new data points
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    mTimer->start(GRAPH_UPDATE_PERIOD);
}

void ProcFmkMonitor::timeout()
{
    /*
    mVoltage += 9.0 - double(qrand() % 1000) / 50.0 + 0.02 * (100.0 - mVoltage);
    mTemperature += 0.5 - double(qrand() % 1000) / 1000.0 + 0.02 * (20.0 - mTemperature);
    mSpeed += 4.9 - double(qrand() % 1000) / 100.0 + 0.02 * (100.0 - mSpeed + 0.3 * mVoltage);
    */
    int k = 0;
    for (auto & kv : procFmkInfo->hostsInfo) {
        ProcessingHostInfo & ph = (*(kv.second));
        const Graphs & g= graphs.at(k);

        double mLoad  = ph.hostInfo.loadAvg.load1min;
        double mCpu   = ph.hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        double mTasks = ph.numTasks;

        g.loadGraph->appendPoint(mLoad);
        g.cpuGraph->appendPoint(mCpu);
        g.tasksGraph->appendPoint(mTasks);

        ++k;
    }
}

}
