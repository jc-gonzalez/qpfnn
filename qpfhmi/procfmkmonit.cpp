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
#include <iostream>
#include "frmagentstatus.h"

#include <QVBoxLayout>
#include <QSpacerItem>

namespace QPF {

const int GRAPH_UPDATE_PERIOD = 1000;

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

    // Add widgets for container hosts/agents
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
        g.cpuGraph->setRange(0, 100);
        //g.cpuGraph->setReferencePoints(QList<double>() << 100);
        g.cpuGraph->setBackgroundBrush(QBrush(QColor(10, 0, 72)));

        g.tasksGraph = panel->getTasksGraph();
        g.tasksGraph->setUnits("");
        g.tasksGraph->setRange(0, 100);
        g.tasksGraph->setReferencePoints(QList<double>() << 50);
        g.tasksGraph->setBackgroundBrush(QBrush(QColor(80, 10, 12)));

        panel->update(ph);
        panels.append(panel);
        graphs.append(g);
    }

    // Add widgets for service swarm managers
    for (auto & kv : procFmkInfo->swarmInfo) {
        SwarmInfo & sw = (*(kv.second));
        FrmHostInfo * panel = new FrmHostInfo(0);
        vlyFrmAgents->addWidget(panel);

        Graphs g;
        g.loadGraph = panel->getLoadGraph();
        g.loadGraph->setUnits("");
        g.loadGraph->setRange(0, 10);
        g.loadGraph->setReferencePoints(QList<double>() << 5);

        g.cpuGraph = panel->getCPUGraph();
        g.cpuGraph->setUnits("%");
        g.cpuGraph->setRange(0, 100);
        //g.cpuGraph->setReferencePoints(QList<double>() << 100);
        g.cpuGraph->setBackgroundBrush(QBrush(QColor(10, 0, 72)));

        g.tasksGraph = panel->getTasksGraph();
        g.tasksGraph->setUnits("");
        g.tasksGraph->setRange(0, 100);
        g.tasksGraph->setReferencePoints(QList<double>() << 50);
        g.tasksGraph->setBackgroundBrush(QBrush(QColor(80, 10, 12)));

        panel->update(sw);
        panels.append(panel);
        graphs.append(g);
    }

    vlyFrmAgents->addSpacerItem(spacerFrmAgents);

    // Periodically update the graphs with new data points
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    mTimer->start(GRAPH_UPDATE_PERIOD);
}

void ProcFmkMonitor::timeout()
{
    int k = 0;
    // Update panels related to container-based agents
    for (auto const & kv : procFmkInfo->hostsInfo) {
        //std::cerr << "UPDATING DATA FOR " << kv.first << "\n";
        ProcessingHostInfo * ph = kv.second;

        // Put new data on graphs
        const Graphs & g = graphs.at(k);
        double mLoad  = ph->hostInfo.loadAvg.load1min;
        double mCpu   = ph->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        double mTasks = ph->hostInfo.loadAvg.runProc;
        if (mLoad > 0.01)  appendPoint(g.loadGraph,  mLoad);
        if (mCpu > 0.01)   appendPoint(g.cpuGraph,   mCpu);
        if (mTasks > 0.01) appendPoint(g.tasksGraph, mTasks);

        // Update Host Agents statistics
        panels.at(k)->update(*ph);
        ++k;
    }

    // Update panels related to service-based agents (swarms)
    for (auto const & kv : procFmkInfo->swarmInfo) {
        //std::cerr << "UPDATING DATA FOR " << kv.first << "\n";
        SwarmInfo * sw = kv.second;

        // Put new data on graphs
        const Graphs & g = graphs.at(k);
        double mLoad  = sw->hostInfo.loadAvg.load1min;
        double mCpu   = sw->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        double mTasks = sw->hostInfo.loadAvg.runProc;
        if (mLoad > 0.01)  appendPoint(g.loadGraph,  mLoad);
        if (mCpu > 0.01)   appendPoint(g.cpuGraph,   mCpu);
        if (mTasks > 0.01) appendPoint(g.tasksGraph, mTasks);

        // Update Host Agents statistics
        panels.at(k)->update(*sw);
        ++k;
    }
}

void ProcFmkMonitor::appendPoint(QSimpleTickerGraph * grph, double value)
{
    QPair<double, double> range = grph->range();
    double low  = range.first;
    double high = range.second;
    bool change  = false;
        
    if (value > high) {
        double lg  = log10(value);
        double nm  = pow(10., lg + 0.1);
        high       = (int)(nm / pow(10., int(lg)) + 0.5) * pow(10., int(lg));
        change     = true;
    }
    if (value < low) {
        double lg  = log10(value);
        double nm  = pow(10., lg - 0.1);
        low        = (int)(nm / pow(10., int(lg)) - 0.5) * pow(10., int(lg));
        change     = true;
    }
    
    if (change) { grph->setRange(low, high); }
    
    grph->appendPoint(value);
}
    
}
