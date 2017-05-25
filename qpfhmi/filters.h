#ifndef FILTERS_H
#define FILTERS_H

#include <QVariant>
#include <QDateTime>
#include <QHash>
#include <QList>
#include <QString>

enum FieldType {
    CHOICE,
    STRING,
    NUMBER,
    DATETIME
};
enum StringOperator {
    STR_CONTAINS,
    STR_BEGINS_WITH,
    STR_ENDS_WITH,
    STR_IS,
    STR_IS_NOT
};
enum NumberOperator {
    NUM_EQUALS,
    NUM_LT,
    NUM_GT,
    NUM_IS_NOT
};
enum DateTimeCriteria {
    DT_WITHIN_LAST,
    DT_EXACTLY,
    DT_BEFORE,
    DT_AFTER,
    DT_TODAY,
    DT_YESTERDAY,
    DT_THIS_WEEK,
    DT_THIS_MONTH,
    DT_THIS_YEAR
};
enum DateTimeUnits {
    HOURS,
    DAYS,
    WEEKS,
    MONTHS,
    YEARS
};

enum FiltersJoin {
    AND,
    OR
};

struct ChoiceFilter {
    ChoiceFilter() = default;
    ~ChoiceFilter() = default;
    QString          fieldValue;
    QString          choice;
    bool evaluate(QString v);
};
struct StringFilter {
    StringFilter() = default;
    ~StringFilter() = default;
    QString          fieldValue;
    StringOperator   op;
    QString          value;
    bool evaluate(QString v);
};
struct NumberFilter {
    NumberFilter() = default;
    ~NumberFilter() = default;
    int              fieldValue;
    NumberOperator   op;
    int              value;
    bool evaluate(int v);
};
struct DateTimeFilter {
    DateTimeFilter() = default;
    ~DateTimeFilter() = default;
    QDateTime        fieldValue;
    DateTimeCriteria test;
    int              howMany;
    DateTimeUnits    units;
    QDateTime        value;
    bool evaluate(QDateTime dt);
};

struct Field {
    Field () = default;
    Field(QString n, FieldType t) : name(n), type(t) {}
    void setChoices(QStringList l) { choices = l; }
    ~Field () = default;
    QString          name;
    QVariant         value;
    FieldType        type;
    QStringList      choices;
};

typedef QHash<QString, Field>  FieldSet;

struct Filter {
    Filter() = default;
    Filter(const Filter&) = default;
    ~Filter() = default;
    Field            field;
    ChoiceFilter     filtChoice;
    StringFilter     filtString;
    NumberFilter     filtNumber;
    DateTimeFilter   filtDateTime;
    bool evaluate(FieldSet fld) {
        bool result = false;
        QString fieldName = field.name;
        QVariant value = fld[fieldName].value;
        switch (field.type) {
        case CHOICE:
            result = filtChoice.evaluate(value.toString());
            break;
        case STRING:
            result = filtString.evaluate(value.toString());
            break;
        case NUMBER:
            result = filtNumber.evaluate(value.toInt());
            break;
        case DATETIME:
            result = filtDateTime.evaluate(value.toDateTime());
            break;
        default:
            break;
        }
        return result;
    }
    QString expr;
};

struct FilterSet {
    QStringList      selectedFields;
    QList<Filter>    filters;
    QStringList      grouping;
    QStringList      sorting;
    QStringList      headers;
    FiltersJoin      join;
    bool evaluate(FieldSet fld) {
        if (join == AND) {
            QList<Filter>::iterator i;
            for (i = filters.begin(); i != filters.end(); ++i) {
                if (!(*i).evaluate(fld)) { return false; }
            }
            return true;
        } else {
            QList<Filter>::iterator i;
            for (i = filters.begin(); i != filters.end(); ++i) {
                if (!(*i).evaluate(fld)) { return true; }
            }
            return false;
        }
    }
};

#endif // FILTERS_H
