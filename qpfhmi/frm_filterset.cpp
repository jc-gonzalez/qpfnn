#include "frm_filterset.h"
#include "ui_frm_filterset.h"

#include <iostream>

#include <QDebug>
#include <QScrollArea>

Frm_FilterSet::Frm_FilterSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frm_FilterSet)
{
    ui->setupUi(this);

    init();
}

Frm_FilterSet::~Frm_FilterSet()
{
    delete ui;
}

void Frm_FilterSet::init()
{
    Frm_Filter * frm = new Frm_Filter;
    setConnections(frm);
    wdgFilters.append(frm);
    ui->vlayFilterContainer->insertWidget(0, frm);
    filterJoinIsAND(true);

    ui->edSQL->installEventFilter(this);

    connect(ui->rbtnAll, SIGNAL(clicked(bool)),
            this, SLOT(filterJoinIsAND(bool)));
    connect(ui->rbtnAny, SIGNAL(clicked(bool)),
            this, SLOT(filterJoinIsOR(bool)));
    connect(ui->btnApply, SIGNAL(clicked(bool)),
            this, SLOT(defineFilter(bool)));
    connect(ui->btnReset, SIGNAL(clicked(bool)),
            this, SLOT(resetFilter(bool)));
}

void Frm_FilterSet::setTableName(QString t)
{
    tableName = t;
}

void Frm_FilterSet::setFields(FieldSet & flds, QStringList fldOrder, QStringList hdrs)
{
    fields      = flds;
    fieldsOrder = fldOrder;
    headers     = hdrs;
    wdgFilters.at(0)->setFields(fields, fieldsOrder, hdrs);
    int i = 0;
    for (auto & f : hdrs) {
        hdr2fld[f] = fieldsOrder.at(i++);
        ui->lstFields->addItem(f);
        ui->lstGroup->addItem(f);
        ui->lstSort->addItem(f);
    }
}

void Frm_FilterSet::setConnections(Frm_Filter * w)
{
    connect(w, SIGNAL(addNewFilter()),
            this, SLOT(addNewFilter()));
    connect(w, SIGNAL(removeThisFilter()),
            this, SLOT(removeCurrentFilter()));
}

void Frm_FilterSet::addNewFilter()
{
    // Find index of emitter
    QWidget * w = qobject_cast<QWidget*>(sender());
    int i = 0;
    for (; i < wdgFilters.count(); ++i) {
        if (wdgFilters.at(i) == w) { break; }
    }
    qDebug() << "Item is number " << i;
    if (i >= wdgFilters.count()) { return; }

    // Add new filter widget *after* emitter
    Frm_Filter * frm = new Frm_Filter;
    frm->setFields(fields, fieldsOrder, headers);
    setConnections(frm);
    wdgFilters.insert(i + 1, frm);
    ui->vlayFilterContainer->insertWidget(i + 1, frm);
}

void Frm_FilterSet::removeCurrentFilter()
{
    // If only one is left, do not remove it
    if (wdgFilters.count() == 1) { return; }

    // Find index of emitter
    QWidget * w = qobject_cast<QWidget*>(sender());
    int i = 0;
    for (; i < wdgFilters.count(); ++i) {
        if (wdgFilters.at(i) == w) { break; }
    }
    if (i >= wdgFilters.count()) { return; }

    // Remove current filter widget and delete it
    delete wdgFilters.takeAt(i);
}

void Frm_FilterSet::filterJoinIsAND(bool b)
{
    if (b) filterSet.join = AND;
}

void Frm_FilterSet::filterJoinIsOR(bool b)
{
    if (b) filterSet.join = OR;
}

bool Frm_FilterSet::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->edSQL) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if ((keyEvent->key() == Qt::Key_Return ) &&
                (keyEvent->modifiers().testFlag(Qt::ControlModifier))) {
                defineManualFilter();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}

void Frm_FilterSet::defineFilter(bool b)
{
    filterSet.selectedFields.clear();
    filterSet.headers.clear();
    foreach (QListWidgetItem * i, ui->lstFields->selectedItems()) {
        filterSet.selectedFields << hdr2fld[i->text()];
        filterSet.headers        << i->text();
    }
    filterSet.filters.clear();
    for (int i = 0; i < wdgFilters.count(); ++i) {
        if (wdgFilters.at(i)->isDefined()) {
            filterSet.filters.append(wdgFilters.at(i)->getFilter());
        }
    }
    filterSet.grouping.clear();
    foreach (QListWidgetItem * i, ui->lstGroup->selectedItems()) {
        filterSet.grouping << hdr2fld[i->text()];
    }
    filterSet.sorting.clear();
    foreach (QListWidgetItem * i, ui->lstSort->selectedItems()) {
        filterSet.sorting << hdr2fld[i->text()];
    }

    // Build SQL expression
    QString sqlExpr = "SELECT ";

    if (filterSet.selectedFields.count() < 1) {
        for (int i = 0; i < ui->lstFields->count(); ++i) {
            filterSet.selectedFields << hdr2fld[ui->lstFields->item(i)->text()];
            filterSet.headers        << ui->lstFields->item(i)->text();
        }        
        //sqlExpr += "*";
    }
    sqlExpr += filterSet.selectedFields.join(",");

    sqlExpr += " FROM " + tableName;

    if (filterSet.filters.count() > 0) {
        sqlExpr += " WHERE ";
        QStringList s;
        for (auto & flt : filterSet.filters) { s << " (" + flt.expr + ") "; }
        sqlExpr += s.join(filterSet.join == AND ? "AND" : "OR");
    }

    if (filterSet.grouping.size() > 0) {
        sqlExpr += " GROUP BY " + filterSet.grouping.join(",");
    }

    if (filterSet.sorting.size() > 0) {
        sqlExpr += " ORDER BY " + filterSet.sorting.join(",");
    }

    sqlExpr += ";";

    ui->edSQL->setPlainText(sqlExpr);
    emit filterIsDefined(sqlExpr, filterSet.headers);
}

void Frm_FilterSet::defineManualFilter()
{
    std::cerr << "Filter is " << ui->edSQL->toPlainText().toStdString() << "\n";
    emit filterIsDefined(ui->edSQL->toPlainText(), QStringList());
}

void Frm_FilterSet::resetFilter(bool b)
{
    ui->lstFields->clearSelection();
    for (int i = wdgFilters.count() - 1; i > 0; --i) {
        delete wdgFilters.takeAt(i);
    }
    wdgFilters.at(0)->reset();
    ui->lstGroup->clearSelection();
    ui->lstSort->clearSelection();
    emit filterReset();
}

