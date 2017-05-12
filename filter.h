#ifndef FILTER_H
#define FILTER_H

#include <QString>
#include <QColor>
#include <QList>
#include "relationsign.h"
struct DescriptionFilter
{
    bool signRegExp;
    QString columnName;
    QString filterValue;
    RelationSign::Sign relationSign;
};

struct DescriptionHighlighFilter: public DescriptionFilter
{
    QColor color;
};

struct Filter
{
    enum Type {
        SORT,
        HIGHLIGHT
    };

    int id;
    QString name;
};

struct SortFilter: public Filter
{
    QList <DescriptionFilter> listDescriptionFilter;
};

struct HighlighFilter: public Filter
{
    QList <DescriptionHighlighFilter> listDescriptionFilter;
};

#endif // FILTER_H
