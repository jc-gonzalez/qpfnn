#ifndef FRMHOSTINFO_H
#define FRMHOSTINFO_H

#include <QWidget>
#include <QVector>
#include "frmagentstatus.h"

#include "procinfo.h"
#include "qsimpletickergraph.h"

namespace Ui {
class FrmHostInfo;
}

class FrmHostInfo : public QWidget
{
    Q_OBJECT

public:
    explicit FrmHostInfo(QWidget *parent = 0);
    ~FrmHostInfo();

    void update(ProcessingHostInfo & ph);
    void update(SwarmInfo & sw);
    void setGraphs(QSimpleTickerGraph * load,
                   QSimpleTickerGraph * cpu,
                   QSimpleTickerGraph * tasks);
    QSimpleTickerGraph * getLoadGraph();
    QSimpleTickerGraph * getCPUGraph();
    QSimpleTickerGraph * getTasksGraph();

protected slots:
    void showLoadPanel();
    void showCPUPanel();
    void showTasksPanel();
    void showInfoPanel();
    void showPage(int page = 0, int subpage = 0);

private:
    Ui::FrmHostInfo *ui;
    QVector<QPF::FrmAgentStatus*> panels;
    bool initialization;
};

#endif // FRMHOSTINFO_H
