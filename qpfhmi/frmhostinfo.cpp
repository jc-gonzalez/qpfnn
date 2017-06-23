#include "frmhostinfo.h"
#include "ui_frmhostinfo.h"

#include "frmagentstatus.h"

FrmHostInfo::FrmHostInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmHostInfo)
{
    ui->setupUi(this);
}

FrmHostInfo::~FrmHostInfo()
{
    delete ui;
}

void FrmHostInfo::update(ProcessingHostInfo & ph)
{
    HostInfo & h = ph.hostInfo;
    HostInfo::LoadAvg & l = h.loadAvg;
    HostInfo::CPUInfo & c = h.cpuInfo;

    // Update info
    QString load = QString("Load Avg: %1 / %2 / %3")
            .arg(l.load1min,0,'g',2)
            .arg(l.load5min,0,'g',2).arg(l.load15min,0,'g',2);
    QString cpu  = QString("Overall CPU load: 1% over %2 s")
            .arg(c.overallCpuLoad.computedLoad,0,'g',2)
            .arg(c.overallCpuLoad.timeInterval);
    QString arch = QString("%1\n%2 (%3)\n"
                           "%4 MHz / %5 KB\n"
                           "%6 phys. sockets\n"
                           "%7 cores per socket\n"
                           "%8 threads per core")
            .arg(QString::fromStdString(c.modelName))
            .arg(QString::fromStdString(c.architecture))
            .arg(QString::fromStdString(c.vendor))
            .arg(c.cpuFreq,0,'g',2).arg(c.cacheSize)
            .arg(c.numPhysicalCpus).arg(c.numCoresPerSocket).arg(c.numThreadsPerCore);
    QString proc = QString("CPUs: %1 - Hyperthreading: %2\n"
                           "%3 running proc. of %4\n"
                           "last PID %5")
            .arg(c.numCpus)
            .arg((c.hyperthreading) ? QString("ON") : QString("off"))
            .arg(l.runProc).arg(l.totalProc).arg(l.lastPid);

    ui->lblHostName->setText(QString("%1").arg(QString::fromStdString(ph.name)));
    ui->lblTaskAgentMode->setText(QString("CONTAINER"));
    ui->lblLoadAvgs->setText(load);
    ui->lblCPULoad->setText(cpu);
    ui->lblInfo->setText(QString("%1\n%2").arg(arch).arg(proc));

    // Create agent panels
    QFrame * frm = new QFrame(0);
    QHBoxLayout * hlyFrmAgents = new QHBoxLayout;
    frm->setLayout(hlyFrmAgents);
    ui->scrollTasksInfo->setWidget(frm);

    QSpacerItem * spacerFrmAgents = new QSpacerItem(10, 10,
                                                    QSizePolicy::Expanding,
                                                    QSizePolicy::Minimum);
    // Add widgets for hosts
    for (auto & a : ph.agInfo) {
        QPF::FrmAgentStatus * panel = new QPF::FrmAgentStatus(0);
        panel->updateInfo(a);
        hlyFrmAgents->addWidget(panel);
    }
    hlyFrmAgents->addSpacerItem(spacerFrmAgents);
}

QSimpleTickerGraph * FrmHostInfo::getLoadGraph()  { return ui->wdgChartLoad; }
QSimpleTickerGraph * FrmHostInfo::getCPUGraph()   { return ui->wdgChartCPU; }
QSimpleTickerGraph * FrmHostInfo::getTasksGraph() { return ui->wdgChartTasks; }

void FrmHostInfo::setGraphs(QSimpleTickerGraph * load,
                            QSimpleTickerGraph * cpu,
                            QSimpleTickerGraph * tasks)
{
    ui->wdgChartLoad  = load;
    ui->wdgChartCPU   = cpu;
    ui->wdgChartTasks = tasks;
}

void FrmHostInfo::showLoadPanel()
{
    showPage(0, 0);
}

void FrmHostInfo::showCPUPanel()
{
    showPage(0, 1);
}

void FrmHostInfo::showTasksPanel()
{
    showPage(0, 2);
}

void FrmHostInfo::showInfoPanel()
{
    showPage(1);
}

void FrmHostInfo::showPage(int page, int subpage)
{
    ui->stckForm->setCurrentIndex(page);
    ui->stckCharts->setCurrentIndex(subpage);
}
