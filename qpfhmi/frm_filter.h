#ifndef FRM_FILTER_H
#define FRM_FILTER_H

#include <QWidget>
#include <QHash>

#include "filters.h"

namespace Ui {
class Frm_Filter;
}

class Frm_Filter : public QWidget
{
    Q_OBJECT

public:
    explicit Frm_Filter(QWidget *parent = 0);
    ~Frm_Filter();

    void setFields(FieldSet & flds, QStringList fldOrder, QStringList hdrs);

    Filter getFilter();
    void reset();
    bool isDefined();

public slots:
    void addFilter(bool);
    void removeFilter(bool);
    void currentFieldChanged(int i);
    void currentOpChanged(int i);

signals:
    void addNewFilter();
    void removeThisFilter();

private:
    void init();

private:
    Ui::Frm_Filter * ui;

    FieldSet         fields;
    QStringList      headers;
    QHash<QString,QString> hdr2fld;
};

#endif // FRM_FILTER_H
