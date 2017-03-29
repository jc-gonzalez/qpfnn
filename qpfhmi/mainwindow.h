/******************************************************************************
 * File:    mainwindow.h
 *          Declaration of class MainWindow
 *
 * Domain:  QPF.qpfgui.mainwindow
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
 *   Declaration of class MainWindow
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QFuture>
#include <QFutureWatcher>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QMdiSubWindow>
#include <QSignalMapper>

#include "datatypes.h"
#include "hmitypes.h"
#include "config.h"
#include "frmagentstatus.h"
#include "textview.h"
#include "sm.h"

#include "hmipxy.h"

#include "dbmng.h"

#include "proctskstatmodel.h"
#include "sysalertmodel.h"
#include "procalertmodel.h"
#include "productsmodel.h"
#include "txtblmodel.h"

namespace Ui {
    class MainWindow;
}

namespace QPF {

class LogWatcher;

class MainWindow : public QMainWindow, StateMachine
{
    Q_OBJECT

    // Valid Manager states
    static const int ERROR;
    static const int OFF;
    static const int INITIALISED;
    static const int RUNNING;
    static const int OPERATIONAL;

    // Valid Manager states
    static const std::string ERROR_StateName;
    static const std::string OFF_StateName;
    static const std::string INITIALISED_StateName;
    static const std::string RUNNING_StateName;
    static const std::string OPERATIONAL_StateName;

public:
    explicit MainWindow(QString url = QString("db://eucops:e314clid@localhost:5432/qpfdb"),
                        QString sessionName = QString(""),
                        QString masterAddr = QString("127.0.0.1"),
                        int port = DEFAULT_INITIAL_PORT,
                        QWidget *parent = 0);
    ~MainWindow();

protected:
    void manualSetupUI();
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

public slots:
    void setAppInfo(QString name, QString rev, QString bld);
    void updateMenus();
    void setActiveSubWindow(QWidget *window);
    void updateSystemView();
    void localarchViewUpdate();
    void updateLocalArchModel();
    void setAutomaticUpdateLocalArchModel(bool b);

private slots:
    void processPath();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void about();
    void quitApp();
    void quitAllQPF();

    void showSelectedInNav(QListWidgetItem* item);
    void selectRowInNav(int row);
    void removeRowInNav(int row);

    void updateWindowMenu();

    void reprocessProduct();

    void processPendingEvents();

    void transitToOperational();

    void showConfigTool();
    void showDBBrowser();
    void showExtToolsDef();
    void showVerbLevel();

    void openWithDefault();
    void openWith();
    void showArchiveTableContextMenu(const QPoint & p);
    void openLocalArchiveElement(QModelIndex idx);

    void showAlertsContextMenu(const QPoint & p);
    void showAlertInfo(QTableView * tblvw);
    void showSysAlertInfo();
    void showProcAlertInfo();

    void sortTaskViewByColumn(int c);
    void showTaskMonitContextMenu(const QPoint & p);
    void showWorkDir();
    void displayTaskInfo();
    void restartTask();
    void stopTask();
    bool runDockerCmd(QModelIndex idx, QString cmd);
    void dumpTaskInfoToTree(QString taskName, const Json::Value & v, QTreeWidget * t);
    void dumpToTree(const Json::Value & v, QTreeWidgetItem * t);

    void sortTxViewByColumn(int c);
    void showTxContextMenu(const QPoint & p);
    void displayTxInfo();

    void showJsonContextMenu(const QPoint & p);

    void showTabsContextMenu(const QPoint & p);
    void showTabsListMenu();
    void selectTabFromList();
    void closeTab(int n);

    void jsontreeExpand();
    void jsontreeExpandSubtree();
    void jsontreeExpandAll();
    void jsontreeCollapse();
    void jsontreeCollapseSubtree();
    void jsontreeCollapseAll();

    void closeTabAction();
    void closeAllTabAction();
    void closeOtherTabAction();

private:
    void readConfig(QString dbUrl);
    TextView *createTextView();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    QVariant getFromSettings(QString name);
    void putToSettings(QString name, QVariant value);
    void getUserToolsFromSettings();
    void putUserToolsToSettings();
    TextView *activeTextView();
    void switchLayoutDirection();

    void processProductsInPath(QString folder);
    void getProductsInFolder(QString & path, QStringList & files, bool recursive = true);

    void init();
    void setLogWatch();
    void handleFinishedHMI();
    QString getState();
    void showState();

    virtual void defineValidTransitions();

    void attachJsonPopUpMenu(QWidget * w);

    void initLocalArchiveView();
    void setUToolTasks();
    void initTasksMonitView();
    void initAlertsTables();
    void initTxView();

    void updateAgentsMonitPanel();

    void showJSONdata(QString title, QString & dataString);
    void binaryGetFITSHeader(QString fileName, QString & tr);

    void getAllChildren(QModelIndex index, QModelIndexList &indices);

    void storeQUTools2Cfg(MapOfUserDefTools qutmap);

    //void convertQUTools2UTools(MapOfUserDefTools qutmap,
    //                           std::map<std::string, UserDefTool> & utmap);

private:
    Ui::MainWindow *ui;

    QString dbUrl;
    QString sessionId;
    QString masterAddress;
    int     startingPort;

    QSignalMapper *windowMapper;

    bool updateProductsModel;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *toolsMenu;
    QMenu *sessionInfoMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QAction *processPathAct;
    QAction *saveAsAct;

    QAction *stopTaskAct;
    QAction *restartTaskAct;

    QAction *restartAct;
    QAction *quitAct;
    QAction *quitAllAct;
#ifndef QT_NO_CLIPBOARD
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
#endif

    QAction *configToolAct;
    QAction *browseDBAct;
    QAction *extToolsAct;
    QAction *verbosityAct;
    QAction *execTestRunAct;

    QAction *acDefault;
    QAction *acReprocess;

    QAction *navigAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    bool isMenuForTabWidget;
    QPoint menuPt;
    QAction * tabCloseAct;
    QAction * tabCloseAllAct;
    QAction * tabCloseOtherAct;

    QVector<LogWatcher*> nodeLogs;
    QStringList nodeNames;
    QVector<QString> activeNodes;
    bool isThereActiveCores;

    HMIProxy * hmiNode;

    QString  fileInDataParams;
    QTimer * taskMonitTimer;

    QString inboxDirName;

    QAction * acWorkDir;
    QAction * acShowTaskInfo;
    QAction * acRestartTask;
    QAction * acStopTask;

    QAction * acShowMsgInfo;

    QAction * acShowAlert;
    QAction * acAckAlert;

    QAction * acArchiveShow;
    QMenu *   acArchiveOpenExt;
    QList<QAction *> acArchiveOpenExtTools;
    QMap<QString, QAction *> acUserTools;

    QVBoxLayout * vlyFrmAgents;
    QSpacerItem * spacerFrmAgents;

    std::map<std::string, TaskAgentInfo*> taskAgentsInfo;
    std::map<std::string, FrmAgentStatus*> taskAgentsInfoPanel;

    // User Defined Tools
    MapOfUserDefTools userDefTools;
    QStringList       userDefProdTypes;

    ProcTaskStatusModel * procTaskStatusModel;
    SysAlertModel *       sysAlertModel;
    ProcAlertModel *      procAlertModel;
    ProductsModel *       productsModel;
    TxTableModel *        txModel;

    QMap<QString,QString> nodeStates;

    QPoint pointOfAction;
    //DBManager *       dbMng;
};

}

#endif // MAINWINDOW_H
