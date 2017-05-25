#ifndef FRM_FILTERSET_H
#define FRM_FILTERSET_H

#include <QWidget>
#include <QVector>
#include <QHash>

#include "filters.h"
#include "frm_filter.h"

namespace Ui {
class Frm_FilterSet;
}

class Frm_FilterSet : public QWidget
{
    Q_OBJECT

public:
    explicit Frm_FilterSet(QWidget *parent = 0);
    ~Frm_FilterSet();

    void setTableName(QString t);
    void setFields(FieldSet & flds, QStringList fldOrder, QStringList hdrs);
    void setConnections(Frm_Filter *w);

public slots:
    void addNewFilter();
    void removeCurrentFilter();

    void filterJoinIsAND(bool b);
    void filterJoinIsOR(bool b);
    void defineFilter(bool b);
    void defineManualFilter();
    void resetFilter(bool b);

signals:
    void filterIsDefined(QString, QStringList);
    void filterReset();

protected:
    bool eventFilter(QObject *target, QEvent *event);

private:
    void init();

private:
    Ui::Frm_FilterSet *ui;

    QString          tableName;
    FieldSet         fields;
    QStringList      fieldsOrder;
    QStringList      headers;
    QHash<QString,QString> hdr2fld;

    QVector<Frm_Filter*> wdgFilters;

    FilterSet        filterSet;
};

#endif // FRM_FILTERSET_H
