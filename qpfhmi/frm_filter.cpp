#include "frm_filter.h"
#include "ui_frm_filter.h"

Frm_Filter::Frm_Filter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frm_Filter)
{
    ui->setupUi(this);

    init();
}

Frm_Filter::~Frm_Filter()
{
    delete ui;
}

void Frm_Filter::init()
{
    connect(ui->tbtnAdd, SIGNAL(clicked(bool)),
            this, SLOT(addFilter(bool)));
    connect(ui->tbtnRemove, SIGNAL(clicked(bool)),
            this, SLOT(removeFilter(bool)));
}

void Frm_Filter::setFields(FieldSet & flds, QStringList fldOrder, QStringList hdrs)
{
    fields = flds;
    headers = hdrs;
    int i = 0;
    for (auto & f : hdrs) {
        QString actualFld = fldOrder.at(i++);
        hdr2fld[f] = actualFld;
        Field & fld = fields[actualFld];
        ui->cboxField->addItem(f, QVariant(int(fld.type)));
    }

    ui->cboxField->setCurrentIndex(-1);
    ui->stckFilterPage->setCurrentIndex(5);

    // Connections
    connect(ui->cboxField, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentFieldChanged(int)));
    connect(ui->cboxDateCriteria, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentOpChanged(int)));
}

void Frm_Filter::addFilter(bool b)
{
    emit addNewFilter();
}

void Frm_Filter::removeFilter(bool b)
{
    emit removeThisFilter();
}

void Frm_Filter::currentFieldChanged(int i)
{
    ui->stckFilterPage->setCurrentIndex(ui->cboxField->itemData(i).toInt());
    if (FieldType(ui->cboxField->itemData(i).toInt()) == CHOICE) {
        Field & fld = fields[hdr2fld[ui->cboxField->currentText()]];
        ui->cboxChoices->clear();
        ui->cboxChoices->addItems(fld.choices);
    }
}

void Frm_Filter::currentOpChanged(int i)
{
    // This only for datetime fields
    switch (DateTimeCriteria(i)) {
    case DT_WITHIN_LAST:
        ui->stckDateTimeFilterValue->setCurrentIndex(0);
        break;
    case DT_EXACTLY:
        ui->stckDateTimeFilterValue->setCurrentIndex(1);
        break;
    case DT_BEFORE:
        ui->stckDateTimeFilterValue->setCurrentIndex(1);
        break;
    case DT_AFTER:
        ui->stckDateTimeFilterValue->setCurrentIndex(1);
        break;
    case DT_TODAY:
        ui->stckDateTimeFilterValue->setCurrentIndex(2);
        break;
    case DT_YESTERDAY:
        ui->stckDateTimeFilterValue->setCurrentIndex(2);
        break;
    case DT_THIS_WEEK:
        ui->stckDateTimeFilterValue->setCurrentIndex(2);
        break;
    case DT_THIS_MONTH:
        ui->stckDateTimeFilterValue->setCurrentIndex(2);
        break;
    case DT_THIS_YEAR:
        ui->stckDateTimeFilterValue->setCurrentIndex(2);
        break;
    }
}

Filter Frm_Filter::getFilter()
{
    QString fieldName = hdr2fld[ui->cboxField->currentText()];
    Filter filter;
    filter.field = fields[fieldName];
    int op;
    QString value;
    QDateTime now = QDateTime::currentDateTime();
    static const QString IntervalUnit[] = {"hour", "day", "week", "month", "year"};

    switch (filter.field.type) {
    case CHOICE:
        filter.filtChoice.choice = ui->cboxChoices->currentText();
        filter.expr = fieldName + "='" + filter.filtChoice.choice + "'";
        break;
    case STRING:
        op = ui->cboxStringOper->currentIndex();
        filter.filtString.op = StringOperator(op);
        filter.filtString.value = ui->edString->text();
        switch (filter.filtString.op) {
        case STR_CONTAINS:
            filter.expr = fieldName + " LIKE '%" + filter.filtString.value + "%'";
            break;
        case STR_BEGINS_WITH:
            filter.expr = fieldName + " LIKE '" + filter.filtString.value + "%'";
            break;
        case STR_ENDS_WITH:
            filter.expr = fieldName + " LIKE '%" + filter.filtString.value + "'";
            break;
        case STR_IS:
            filter.expr = fieldName + "='" + filter.filtString.value + "'";
            break;
        case STR_IS_NOT:
            filter.expr = fieldName + "!='" + filter.filtString.value + "'";
            break;
        }
        break;
    case NUMBER:
        op = ui->cboxNumberOper->currentIndex();
        filter.filtNumber.op = NumberOperator(op);
        value = ui->edNumber->text();
        filter.filtNumber.value = value.toInt();
        switch (filter.filtNumber.op) {
        case NUM_EQUALS:
            filter.expr = fieldName + "=" + value;
            break;
        case NUM_LT:
            filter.expr = fieldName + "<" + value;
            break;
        case NUM_GT:
            filter.expr = fieldName + ">" + value;
            break;
        case NUM_IS_NOT:
            filter.expr = fieldName + "!=" + value;
            break;
        }
        break;
    case DATETIME:
        op = ui->cboxDateCriteria->currentIndex();
        filter.filtDateTime.test = DateTimeCriteria(op);
        filter.filtDateTime.value = ui->dtDateTime->dateTime();
        filter.filtDateTime.howMany = ui->spboxTimeUnits->value();
        filter.filtDateTime.units = DateTimeUnits(ui->cboxTimeUnits->currentIndex());
        fieldName = "DATE(" + fieldName + ")";
        switch (filter.filtDateTime.test) {
        case DT_WITHIN_LAST:
            filter.expr = fieldName + ">(NOW() - " +
                          QString("%1 * INTERVAL '1 %2'")
                          .arg(filter.filtDateTime.howMany)
                          .arg(IntervalUnit[filter.filtDateTime.units]) + ")";
            break;
        case DT_EXACTLY:
            filter.expr = fieldName + "=DATE('" + filter.filtDateTime.value.toString() + "')";
            break;
        case DT_BEFORE:
            filter.expr = fieldName + "=DATE('" + filter.filtDateTime.value.toString() + "')";
            break;
        case DT_AFTER:
            filter.expr = fieldName + "=DATE('" + filter.filtDateTime.value.toString() + "')";
            break;
        case DT_TODAY:
            filter.expr = fieldName + "=DATE(NOW())";
            break;
        case DT_YESTERDAY:
            filter.expr = fieldName + "=(DATE(NOW()) - INTERVAL '1 day')";
            break;
        case DT_THIS_WEEK:
            filter.expr = fieldName + ">(DATE(NOW()) - INTERVAL '1 week')";
            break;
        case DT_THIS_MONTH:
            filter.expr = fieldName + ">(DATE(NOW()) - INTERVAL '1 month')";
            break;
        case DT_THIS_YEAR:
            filter.expr = fieldName + ">(DATE(NOW()) - INTERVAL '1 year')";
            break;
        }
        break;
    default:
        break;
    }
    return filter;
}

void Frm_Filter::reset()
{
    ui->cboxField->setCurrentIndex(-1);
}

bool Frm_Filter::isDefined()
{
    return (ui->cboxField->currentIndex() != -1);
}
