/******************************************************************************
 * File:    configtool.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.qpfgui.configtool
 *
 * Version:  1.1
 *
 * Date:    2015/09/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement ConfigTool UI class
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

#include "configtool.h"
#include "ui_configtool.h"

#include "tools.h"

#include "exttooledit.h"
#include "config.h"

#include <QHostInfo>
#include <QFileInfo>
#include <QCheckBox>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QtCore/qnamespace.h>

using Configuration::cfg;

#define C(x) (x).c_str()
#define QS(x) QString::fromStdString(x)

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace QPF {

//----------------------------------------------------------------------
// Method: flags
// Overwrites QAbstractItemModel::flags
//----------------------------------------------------------------------
Qt::ItemFlags StandardItemModel::flags(const QModelIndex&index) const
{
    Qt::ItemFlags flags;
    if (index.isValid()) {
        flags =  Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    } else {
        flags =  Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled  | Qt::ItemIsEnabled;
    }
    return flags;
}

//----------------------------------------------------------------------
// Method: flags
// Overwrites QAbstractItemModel::flags
//----------------------------------------------------------------------
Qt::ItemFlags StringListModel::flags(const QModelIndex&index) const
{
    Qt::ItemFlags flags;
    if (index.isValid()) {
        flags =  Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    } else {
        flags =  Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled  | Qt::ItemIsEnabled;
    }
    return flags;
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ModelView::ModelView(QVector<QStringList> & vdlist,
                     QAbstractItemView  * v,
                     QAbstractItemModel * m)
    : view(v), model(m), type(Table)
{
    if (view  == 0) { view = new QTableView(0); }
    if (model == 0) { model = new StandardItemModel(vdlist.count(),
                                                     vdlist.at(0).count()); }
    setData(vdlist);
    view->setModel(model);
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ModelView::ModelView(QStringList & dlist,
                     QAbstractItemView  * v,
                     QAbstractItemModel * m)
    : view(v), model(m), type(List)
{
    if (view  == 0) { view  = new QListView(0); }
    if (model == 0) { model = new StringListModel(); }
    dynamic_cast<StringListModel*>(model)->setStringList(dlist);
    view->setModel(model);
}

//----------------------------------------------------------------------
// Method: setHeader
// Sets header in section 0
//----------------------------------------------------------------------
void ModelView::setHeader(QString h)
{
    model->setHeaderData(0, Qt::Horizontal, h);
}

//----------------------------------------------------------------------
// Method: setHeaders
// Sets column headers for the model
//----------------------------------------------------------------------
void ModelView::setHeaders(QStringList & hlist)
{
    for (int i = 0; i < hlist.count(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, hlist.at(i));
    }
}

//----------------------------------------------------------------------
// Method: setData
// Sets the data for a list view
//----------------------------------------------------------------------
void ModelView::setData(QStringList & dlist)
{
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

    for (int row = 0; row < dlist.count(); ++row) {
        model->insertRows(row, 1, QModelIndex());
        model->setData(model->index(row, 0, QModelIndex()), dlist.at(row));
    }
}

//----------------------------------------------------------------------
// Method: setData
// Sets the data for a table view
//----------------------------------------------------------------------
void ModelView::setData(QVector<QStringList> & vdlist)
{
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

    for (int row = 0; row < vdlist.count(); ++row) {
        model->insertRows(row, 1, QModelIndex());
        const QStringList & rowdata = vdlist.at(row);
        for (int col = 0; col < rowdata.count(); ++col) {
            model->setData(model->index(row, col, QModelIndex()), rowdata.at(col));
        }
    }
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ConfigTool::ConfigTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigTool)
{
    ui->setupUi(this);
tramp
    monitMsgFlags.append(FlagSt({ "START",       ui->chkMsgsSTARTToDisk     , ui->chkMsgsSTARTToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "INDATA"     , ui->chkMsgsINDATAToDisk    , ui->chkMsgsINDATAToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "DATA_RQST"  , ui->chkMsgsDATARQSTToDisk  , ui->chkMsgsDATARQSTToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "DATA_INFO"  , ui->chkMsgsDATAINFOToDisk  , ui->chkMsgsDATAINFOToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "MONIT_RQST" , ui->chkMsgsMONITRQSTToDisk , ui->chkMsgsMONITRQSTToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "MONIT_INFO" , ui->chkMsgsMONITINFOToDisk , ui->chkMsgsMONITINFOToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "TASK_PROC"  , ui->chkMsgsTASKPROCToDisk  , ui->chkMsgsTASKPROCToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "TASK_RES"   , ui->chkMsgsTASKRESToDisk   , ui->chkMsgsTASKRESToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "CMD"        , ui->chkMsgsCMDToDisk       , ui->chkMsgsCMDToDisk_2 }));
    monitMsgFlags.append(FlagSt({ "STOP"       , ui->chkMsgsSTOPToDisk      , ui->chkMsgsSTOPToDisk_2 }));


    // Hide non-yet-implemented widgets
    ui->gpboxInput->hide();
    ui->gpboxOutput->hide();

    ui->btngrpSection->setId(ui->tbtnGeneral       , PageGeneral);
    ui->btngrpSection->setId(ui->tbtnMachines      , PageMachines);
    ui->btngrpSection->setId(ui->tbtnProdProc      , PageProdProc);
    ui->btngrpSection->setId(ui->tbtnNetwork       , PageNetwork);
    ui->btngrpSection->setId(ui->tbtnOrchestration , PageOrchestration);
    ui->btngrpSection->setId(ui->tbtnExtTools      , PageExtTools);
    ui->btngrpSection->setId(ui->tbtnStorage       , PageStorage);
    ui->btngrpSection->setId(ui->tbtnFlags         , PageFlags);
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
ConfigTool::~ConfigTool()
{
    delete ui;
}

//----------------------------------------------------------------------
// Method: readConfig
// Reads config data from internals and places them into dialog
//----------------------------------------------------------------------
void ConfigTool::prepare(MapOfUserDefTools & userTools, QStringList pts)
{
    // Save current config data string
    cfgDataBackup = cfg.str();
    std::cerr << cfgDataBackup << std::endl;

    // Place config data in dialog
    transferCfgToGUI();
    initExtTools(userTools, pts);
}

//----------------------------------------------------------------------
// Method: initExtTools
// Init table of user defined / external tools
//----------------------------------------------------------------------
void ConfigTool::initExtTools(MapOfUserDefTools & userTools, QStringList pts)
{
    ui->tblwdgUserDefTools->clear();
    ui->tblwdgUserDefTools->setRowCount(userTools.count());
    ui->tblwdgUserDefTools->setColumnCount(5);
    ui->tblwdgUserDefTools->setHorizontalHeaderLabels(QStringList()
                                                      << "Name"
                                                      << "Description"
                                                      << "Executable"
                                                      << "Arguments"
                                                      << "Product types");
    int row = 0;
    foreach (QString key, userTools.keys()) {
        const QUserDefTool & udt = userTools.value(key);
        ui->tblwdgUserDefTools->setItem(row, 0, new QTableWidgetItem(udt.name));
        ui->tblwdgUserDefTools->setItem(row, 1, new QTableWidgetItem(udt.desc));
        ui->tblwdgUserDefTools->setItem(row, 2, new QTableWidgetItem(udt.exe));
        ui->tblwdgUserDefTools->setItem(row, 3, new QTableWidgetItem(udt.args));
        ui->tblwdgUserDefTools->setItem(row, 4, new QTableWidgetItem(udt.prod_types.join(QString("|"))));
        ++row;
    }
    userDefTools = userTools;
    origDefTools = userTools;
    prodTypes = pts;
    connect(ui->tblwdgUserDefTools, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(changeToolWithItem(QTableWidgetItem*)));
}

//----------------------------------------------------------------------
// Method: getExtTools
// Allows external access to user defined tools
//----------------------------------------------------------------------
void ConfigTool::getExtTools(MapOfUserDefTools & userTools)
{
    userTools = userDefTools;
}

//----------------------------------------------------------------------
// Slot: save
// Saves the configuration data into the original file loaded
//----------------------------------------------------------------------
void ConfigTool::save()
{
    if (!cfg.isActualFile) {
        QString fileName(QString::fromStdString(cfg.cfgFileName));
        int ret = QMessageBox::question(this, tr("Save"),
                    tr("Do you want to overwrite the configuration file \"") +
                    fileName + QString("\"?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            saveAsFilename(fileName);
        }
    } else {
        saveAs();
    }
}

//----------------------------------------------------------------------
// Slot: saveAs
// Saves the configuration data into a new config file
//----------------------------------------------------------------------
void ConfigTool::saveAs()
{


    QFileInfo fs(QString::fromStdString(cfg.cfgFileName));
    QString filter("*.json");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
            fs.absolutePath(), filter);
    if (! fileName.isEmpty()) {
        saveAsFilename(fileName);
    }
}

//----------------------------------------------------------------------
// Slot: saveAsFilename
// Saves the configuration data into a new config file
//----------------------------------------------------------------------
void ConfigTool::saveAsFilename(QString & fName)
{
    if (! fName.isEmpty()) {
        transferGUIToCfg();
        QString cfgJsonContent(cfg.str().c_str());
        QFile file( fName );
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << cfgJsonContent << endl;
        }
    }
}

//----------------------------------------------------------------------
// Slot: apply
// Moves the new config. info into the internals for immediate use
//----------------------------------------------------------------------
void ConfigTool::apply()
{
    transferGUIToCfg();
    accept();
}

//----------------------------------------------------------------------
// Slot: closeDoNothing
// Closes the dialog, without modifying the configuration
//----------------------------------------------------------------------
void ConfigTool::closeDoNothing()
{
    reject();
}

//----------------------------------------------------------------------
// Slot: selectBasePath
// Select QPF base path
//----------------------------------------------------------------------
void ConfigTool::selectBasePath()
{


    QString pathName(QString::fromStdString(cfg.storage.base));
    pathName = QFileDialog::getExistingDirectory(this,
                                                 tr("Select QPF base path"),
                                                 pathName);
    QFileInfo fs(pathName);
    if (fs.exists()) {
        ui->edBasePath->setText(pathName);
    }

}

//----------------------------------------------------------------------
// Slot: setWorkingPaths
// Sets working paths for a given base path
//----------------------------------------------------------------------
void ConfigTool::setWorkingPaths(QString newPath)
{
    ui->nedLocalArchiveFolder->setText(newPath + "/data/archive");
    ui->nedInbox->setText(newPath + "/data/inbox");
    ui->nedOutbox->setText(newPath + "/data/outbox");
    ui->nedUserReprocArea->setText(newPath + "/data/user");
}

//----------------------------------------------------------------------
// Slot: addHost
// Adds a new row to the hosts table to allow the addition of a new host
//----------------------------------------------------------------------
void ConfigTool::addHost()
{
    QTableView * vw = ui->tblviewHosts;
    vw->model()->insertRow(vw->model()->rowCount());
}

//----------------------------------------------------------------------
// Slot: removeHost
// Removes the selected row with information of a configured host
//----------------------------------------------------------------------
void ConfigTool::removeHost()
{
    removeFromTable(ui->tblviewHosts, "host");
}

//----------------------------------------------------------------------
// Slot: addProduct
// Adds a new row to the product types list to allow the addition of a new one
//----------------------------------------------------------------------
void ConfigTool::addProduct()
{
    QListView * vw = ui->listProductTypes;
    vw->model()->insertRow(vw->model()->rowCount());
}

//----------------------------------------------------------------------
// Slot: removeProduct
// Removes the selected row with information of a configured product type
//----------------------------------------------------------------------
void ConfigTool::removeProduct()
{
    removeFromTable(ui->listProductTypes, "product type");
}

//----------------------------------------------------------------------
// Slot: addProcessor
// Adds a new row to the processors list to allow the addition of a new one
//----------------------------------------------------------------------
void ConfigTool::addProcessor()
{
    QListView * vw = ui->listProcs;
    vw->model()->insertRow(vw->model()->rowCount());
}

//----------------------------------------------------------------------
// Slot: removeProcessor
// Removes the selected row with information of a configured processor
//----------------------------------------------------------------------
void ConfigTool::removeProcessor()
{
    removeFromTable(ui->listProcs, "processor");
}

//----------------------------------------------------------------------
// Slot: addRule
// Adds a new row to the rules table to allow the addition of a new rule
//----------------------------------------------------------------------
void ConfigTool::addRule()
{
    QTableView * vw = ui->tblviewRules;
    vw->model()->insertRow(vw->model()->rowCount());
}

//----------------------------------------------------------------------
// Slot: removeRule
// Removes the selected row with information of a configured rule
//----------------------------------------------------------------------
void ConfigTool::removeRule()
{
    removeFromTable(ui->tblviewRules, "rule");
}

//----------------------------------------------------------------------
// Slot: removeFromTable
// Removes the selected row from a table/list view, if confirmed by user
//----------------------------------------------------------------------
void ConfigTool::removeFromTable(QAbstractItemView * vw, QString item)
{
    QMessageBox msgBox;
    msgBox.setText(QString("Yoy requested to remove a host from the list.").arg(item));
    msgBox.setInformativeText(QString("Do you really want to delete the selected %1? "
                                      " Its information will be lost.").arg(item));
    QPushButton *removeButton = msgBox.addButton(QString("Remove %1").arg(item), QMessageBox::ActionRole);
    QPushButton *discardtButton = msgBox.addButton(QMessageBox::Discard);
    msgBox.setDefaultButton(discardtButton);
    msgBox.exec();
    if (msgBox.clickedButton() == removeButton) {
        vw->model()->removeRow(vw->currentIndex().row());
    }
}

//----------------------------------------------------------------------
// Slot: addNewTool
// Opens a dialog to add a new used defined tool
//----------------------------------------------------------------------
void ConfigTool::addNewTool()
{
    ExtToolEdit dlg;
    dlg.setProdTypes(prodTypes);
    if (dlg.exec()) {
        // Create new tool and append to list in table
        QUserDefTool udt;
        dlg.getToolInfo(udt);
        int row = ui->tblwdgUserDefTools->rowCount();
        ui->tblwdgUserDefTools->insertRow(row);

        ui->tblwdgUserDefTools->setItem(row, 0, new QTableWidgetItem(udt.name));
        ui->tblwdgUserDefTools->setItem(row, 2, new QTableWidgetItem(udt.desc));
        ui->tblwdgUserDefTools->setItem(row, 1, new QTableWidgetItem(udt.exe));
        ui->tblwdgUserDefTools->setItem(row, 3, new QTableWidgetItem(udt.args));
        ui->tblwdgUserDefTools->setItem(row, 4, new QTableWidgetItem(udt.prod_types.join(QString("|"))));

        userDefTools[udt.name] = udt;
    }
}

//----------------------------------------------------------------------
// Slot: editTool
// Edits the tool in the table with a given model index
//----------------------------------------------------------------------
void ConfigTool::editTool(QModelIndex idx)
{
    int row = idx.row();
    editTool(row);
}

//----------------------------------------------------------------------
// Slot: editTool
// Edits the tool selected in the table
//----------------------------------------------------------------------
void ConfigTool::editTool()
{
    QList<QTableWidgetItem*> items = ui->tblwdgUserDefTools->selectedItems();
    int row = items.first()->row();
    editTool(row);
}

//----------------------------------------------------------------------
// Slot: editTool
// Edits the row-th tool in the table
//----------------------------------------------------------------------
void ConfigTool::editTool(int row)
{
    QString name = ui->tblwdgUserDefTools->item(row, 0)->data(0).toString();
    QUserDefTool udt = userDefTools[name];
    ExtToolEdit dlg;
    dlg.setProdTypes(prodTypes);
    dlg.editTool(udt);
    if (dlg.exec()) {
        // Create new tool and append to list in table
        dlg.getToolInfo(udt);

        ui->tblwdgUserDefTools->item(row, 0)->setData(0, udt.name);
        ui->tblwdgUserDefTools->item(row, 1)->setData(0, udt.desc);
        ui->tblwdgUserDefTools->item(row, 2)->setData(0, udt.exe);
        ui->tblwdgUserDefTools->item(row, 3)->setData(0, udt.args);
        ui->tblwdgUserDefTools->item(row, 4)->setData(0, udt.prod_types.join(QString("|")));

        userDefTools[udt.name] = udt;
    }
}

//----------------------------------------------------------------------
// Slot: changeToolWithItem
// Puts tool info into table item
//----------------------------------------------------------------------
void ConfigTool::changeToolWithItem(QTableWidgetItem * item)
{
    QString content = item->data(0).toString();
    QString name = ui->tblwdgUserDefTools->item(item->row(), 0)->data(0).toString();
    QUserDefTool & udt = const_cast<QUserDefTool&>(userDefTools[name]);
    switch (item->column()) {
    case 0: udt.name       = content; break;
    case 1: udt.desc       = content; break;
    case 2: udt.exe        = content; break;
    case 3: udt.args       = content; break;
    case 4: udt.prod_types = content.split("|"); break;
    default: break;
    }
}

//----------------------------------------------------------------------
// Slot: removeTool
// Removes the selected tool from table
//----------------------------------------------------------------------
void ConfigTool::removeTool()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Remove tool from list");
    msgBox.setText("You requested to remove selected user defined tool from the list.");
    msgBox.setInformativeText("Do you really want to remove this tool?");
    QPushButton * removeButton = msgBox.addButton(tr("Remove tool"), QMessageBox::ActionRole);
    QPushButton * cancelButton = msgBox.addButton(QMessageBox::Abort);

    msgBox.exec();
    if (msgBox.clickedButton() == removeButton) {
        QList<QTableWidgetItem*> items = ui->tblwdgUserDefTools->selectedItems();
        int row = items.first()->row();
        QString name = ui->tblwdgUserDefTools->item(row, 0)->data(0).toString();
        ui->tblwdgUserDefTools->removeRow(row);
        userDefTools.remove(name);
    } else if (msgBox.clickedButton() == cancelButton) {
        return;
    }
}

//----------------------------------------------------------------------
// Slot: cancelDlg
// Cancel dialog (all edited info is forgotten)
//----------------------------------------------------------------------
void ConfigTool::cancelDlg()
{
    userDefTools = origDefTools;
}

//----------------------------------------------------------------------
// Method: transferCfgToGUI
// Transfer internal configuration info to GUI widgets
//----------------------------------------------------------------------
void ConfigTool::transferCfgToGUI()
{


    // Generate values for Config. display
    QVector<QStringList> netTable;
    QVector<QStringList> hostsTable;
    QVector<QStringList> rulesTable;
    QStringList hostsList;
    QStringList data;
    QStringList hdr;
    int masterIndex = 0;

    QMap<QString, QString> ipToMachine;
    QMap<QString, int> machineAgents;

    int i = 0;
    for (std::string & m : cfg.machines) {
        std::string ip("0.0.0.0");
        if (cfg.machineNodes.find(m) != cfg.machineNodes.end()) {
            std::vector<std::string> & mnodes = cfg.machineNodes[m];
            std::string pname = mnodes.at(0);
            Peer * peer = cfg.peersCfgByName[pname];
            ip = peer->clientAddr.substr(6);
            ip = ip.substr(0, ip.find(':'));
        }
        if (m == cfg.masterMachine) { masterIndex = i; }
        QString machine = QS(m);
        QString iP = QS(ip);
        QStringList column;
        column << machine << iP
               << ((masterIndex == i) ? tr("Master Host") : tr("Processing Host"));
        hostsTable.append(column);
        hostsList << machine;
        ipToMachine[iP] = machine;
        ++i;
    }

    int numNodes = cfg.peersCfg.size();
    for (i = 0; i < numNodes; ++i) {
        Peer & peer = cfg.peersCfg.at(i);
        QString s = QS(peer.clientAddr);
        s = s.mid(s.lastIndexOf("/") + 1);
        s = s.left(s.indexOf(":"));
        if (peer.type == "taskagent") {
            QString m = ipToMachine[s];
            if (machineAgents.find(m) == machineAgents.end()) {
                machineAgents[m] = 1;
            } else {
                machineAgents[m] = machineAgents[m] + 1;
            }
        }

        data.clear();
        data << ipToMachine[s] + " (" + s + ")"
             << QS(peer.name) << QS(peer.type)
             << QS(peer.clientAddr) << QS(peer.serverAddr);

        netTable.append(data);
    }

    for (i = 0; i < hostsTable.count(); ++i) {
        QStringList & column = hostsTable[i];
        column << QString("%1").arg(machineAgents[column.at(0)]);
    }

    Peer * peer = cfg.peersCfgByName[std::string("QPFHMI")];
    QString qpfhmiServerAddr = QS(peer->serverAddr);
    int basePort = qpfhmiServerAddr.mid(qpfhmiServerAddr.lastIndexOf(":") + 1).toInt();

    int numRules = cfg.orcParams.rules.size();
    for (i = 0; i < numRules; ++i) {
        Rule * const rule = cfg.orcParams.rules.at(i);
        data.clear();
        data << QS(rule->name)
             << QS(join(rule->inputs, ", "))
             << QS(rule->condition)
             << QS(rule->processingElement)
             << QS(join(rule->outputs, ", "));
        rulesTable.append(data);
    }

    // Now, put data in the GUI

    ui->lblConfigName->setText(C("Source: " + cfg.cfgFileName));

    // 1. GENERAL
    // 1.1 General
    ui->edAppName->setText(C(cfg.appName));
    ui->edAppVersion->setText(C(cfg.appVersion));
    ui->edLastAccess->setText(C(cfg.lastAccess));
    ui->edWorkArea->setText(C(cfg.storage.base));

    // 1.2 Environment
    ui->edUser->setText(qgetenv("USER"));
    ui->edHost->setText(QHostInfo::localHostName());

    ui->edExec->setText(QCoreApplication::applicationFilePath());
    ui->edPATH->setText(qgetenv("PATH"));
    ui->edLDPATH->setText(qgetenv("LD_LIBRARY_PATH"));

    // 2. MACHINES
    hdr.clear();
    hdr << "Host name" << "IP" << "Host Type" << "Num.Agents";
    ModelView * mvHosts = createTableModelView(ui->tblviewHosts, hostsTable, hdr);
    (void)(mvHosts);

    ui->cboxMasterHost->addItems(hostsList);
    ui->cboxMasterHost->setCurrentIndex(masterIndex);

    ui->spboxBasePort->setValue(basePort);

    // 3. PRODUCTS & PROCESSORS
    // 3.1 Products
    data.clear();
    for (auto & s : cfg.orcParams.productTypes) data << QS(s);
    ModelView * mvPT = createListModelView(ui->listProductTypes, data, "Product Type");
    (void)(mvPT);

    // 3.2 Processors
    data.clear();
    for (auto & kv : cfg.orcParams.processors) data << QS(kv.first);
    ModelView * mvProcs = createListModelView(ui->listProcs, data, "Processors");
    (void)(mvProcs);

    // 4. NETWORK
    hdr.clear();
    hdr << "Host"
        << "Node name" << "Node Type"
        << "Client Addr" << "Server Addr";
    ModelView * mvNet = createTableModelView(ui->tblviewNet, netTable, hdr);
    (void)(mvNet);

    // 4. RULES
    hdr.clear();
    hdr << "Rule name" << "Inputs" << "Condition" << "Processor" << "Outputs";
    ModelView * mvRules = createTableModelView(ui->tblviewRules, rulesTable, hdr);
    (void)(mvRules);

    // 5. USER DEFINED TOOLS

    // Already set

    // 6. STORAGE
    ui->edBasePath->setText(C(cfg.storage.base));
    ui->nedLocalArchiveFolder->setText(C(cfg.storage.local_archive.path));
    ui->nedInbox->setText(C(cfg.storage.inbox.path));
    ui->nedOutbox->setText(C(cfg.storage.outbox.path));

    // 7. FLAGS
    transferFlagsFromCfgToGUI();

}

//----------------------------------------------------------------------
// Method: transferGUIToCfg
// Transfer config settings in GUI to internal configuration info
//----------------------------------------------------------------------
void ConfigTool::transferGUIToCfg()
{

    // 7. FLAGS
    transferFlagsFromGUIToCfg();
}

//----------------------------------------------------------------------
// Method: createListModelView
// Uses ModelView class to create a model for a list view
//----------------------------------------------------------------------
ModelView * ConfigTool::createListModelView(QAbstractItemView * v,
                                            QStringList & dlist,
                                            QString hdr)
{
    ModelView * mv = new ModelView(dlist, v);
    mv->setHeader(hdr);

    return mv;
}


//----------------------------------------------------------------------
// Method: createListModelView
// Uses ModelView class to create a model for a table view
//----------------------------------------------------------------------
ModelView * ConfigTool::createTableModelView(QAbstractItemView * v,
                                             QVector<QStringList> & vdlist,
                                             QStringList & hdr)
{
    ModelView * mv = new ModelView(vdlist, v);
    mv->setHeaders(hdr);

    return mv;
}

//----------------------------------------------------------------------
// Method: transferFlagsFromCfgToGUI
// Transfer flags settings from internal cfg structure to dialog
//----------------------------------------------------------------------
void ConfigTool::transferFlagsFromCfgToGUI()
{


    std::string msgName;

    std::map<std::string, bool> & fmapDsk = cfg.flags.monit.msgsToDisk;
    std::map<std::string, bool> & fmapDB  = cfg.flags.monit.msgsToDB;

    for (int i = (int)(MSG_START_IDX); i < (int)(MSG_UNKNOWN_IDX); ++i) {
        msgName = monitMsgFlags.at(i).msgName;
        monitMsgFlags.at(i).chkDisk->setChecked(!(fmapDsk.find(msgName) == fmapDsk.end()));
        monitMsgFlags.at(i).chkDB->setChecked(!(fmapDB.find(msgName) == fmapDB.end()));
    }

    ui->chkMsgsIncommingInLog->setChecked(cfg.flags.monit.notifyMsgArrival);
    ui->chkGroupTskAgentLogs->setChecked(cfg.flags.monit.groupTaskAgentLogs);

    ui->chkAllowReproc->setChecked(cfg.flags.proc.allowReprocessing);
    ui->chkGenerateIntermedProd->setChecked(cfg.flags.proc.intermedProducts);

    ui->chkSendOutputsToArchive->setChecked(cfg.flags.arch.sendOutputsToMainArchive);
}

//----------------------------------------------------------------------
// Method: transferFlagsFromGUIToCfg
// Transfer flags settings from dialog to internal cfg structure
//----------------------------------------------------------------------
void ConfigTool::transferFlagsFromGUIToCfg()
{


    std::string msgName;

    std::map<std::string, bool> & fmapDsk = cfg.flags.monit.msgsToDisk;
    std::map<std::string, bool> & fmapDB = cfg.flags.monit.msgsToDB;
    fmapDsk.clear();
    fmapDB.clear();

    for (int i = (int)(MSG_START_IDX); i < (int)(MSG_UNKNOWN_IDX); ++i) {
        msgName = monitMsgFlags.at(i).msgName;
        if (monitMsgFlags.at(i).chkDisk->isChecked()) { fmapDsk[msgName] = true; }
        if (monitMsgFlags.at(i).chkDB->isChecked()) { fmapDB[msgName] = true; }
    }

    cfg.flags.monit.notifyMsgArrival   = ui->chkMsgsIncommingInLog->isChecked();
    cfg.flags.monit.groupTaskAgentLogs = ui->chkGroupTskAgentLogs->isChecked();

    cfg.flags.proc.allowReprocessing = ui->chkAllowReproc->isChecked();
    cfg.flags.proc.intermedProducts  = ui->chkGenerateIntermedProd->isChecked();

    cfg.flags.arch.sendOutputsToMainArchive = ui->chkSendOutputsToArchive->isChecked();
}

// Auxiliary vector with information about config. flags
QVector<ConfigTool::FlagSt> ConfigTool::monitMsgFlags;

}
