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
    scrollArea(scrl)
{
}

void ProcFmkMonitor::setupHostsInfo(ProcessingFrameworkInfo & q)
{
    int k = 0;

    /*
    int numag[] = {10,8,12};

    q.numContTasks = 0;
    HostInfo hi;
    hi.update();

    for (auto & h : {"host1.name", "host2.name", "host3.name"}) {
        hi.update();
        ProcessingHostInfo ph;
        ph.name = h;
        ph.numAgents = numag[k];
        ph.hostInfo = hi;
        ph.numTasks = 0;

        for (int i = 0; i < ph.numAgents; ++i) {
            AgentInfo agInfo;
            agInfo.name = QString("TskAge_%1_%2")
                    .arg(k + 1, 2, 10, QLatin1Char('0'))
                    .arg(i + 1, 2, 10, QLatin1Char('0')).toStdString();
            agInfo.taskStatus = TaskStatusSpectra({rand() % 10, rand() % 10, rand() % 4,
                                                   rand() % 3, rand() % 3, rand() % 10});
            agInfo.load = (rand() % 1000) * 0.01;
            ph.agInfo.push_back(agInfo);
            ph.numTasks += (agInfo.taskStatus.running +
                            agInfo.taskStatus.scheduled +
                            agInfo.taskStatus.paused +
                            agInfo.taskStatus.stopped +
                            agInfo.taskStatus.failed +
                            agInfo.taskStatus.finished);
        }

        q.hostsInfo.push_back(ph);
        q.numContTasks += ph.numTasks;
        k++;
    }
    */

    QFrame * frm = new QFrame(0);
    QVBoxLayout * vlyFrmAgents = new QVBoxLayout;
    frm->setLayout(vlyFrmAgents);
    scrollArea->setWidget(frm);

    QSpacerItem * spacerFrmAgents = new QSpacerItem(10, 10,
                                                    QSizePolicy::Minimum,
                                                    QSizePolicy::Expanding);
    //vlyFrmAgents->addSpacerItem(spacerFrmAgents);

    // Add widgets for hosts
    for (auto & ph : q.hostsInfo) {
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
    mVoltage += 9.0 - double(qrand() % 1000) / 50.0 + 0.02 * (100.0 - mVoltage);
    mTemperature += 0.5 - double(qrand() % 1000) / 1000.0 + 0.02 * (20.0 - mTemperature);
    mSpeed += 4.9 - double(qrand() % 1000) / 100.0 + 0.02 * (100.0 - mSpeed + 0.3 * mVoltage);

    for (auto & g : graphs) {
        g.loadGraph->appendPoint((mVoltage + 100.) / 25.);
        g.cpuGraph->appendPoint(mSpeed * .75);
        g.tasksGraph->appendPoint(int(mTemperature));
    }
}

}
