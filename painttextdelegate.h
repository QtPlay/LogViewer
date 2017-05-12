#ifndef PAINTTEXTDELEGATE_H
#define PAINTTEXTDELEGATE_H

#include <QItemDelegate>
#include <QPainter>
#include <QStyleOption>
#include <QStack>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QSet>
#include "filter.h"

class PaintTextDelegate : public QItemDelegate
{
public:
    PaintTextDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setFilter(HighlighFilter filter);
    void setFieldTypes(QMap <int, QVariant::Type> mapFieldTypes);
    void resetFilter();

protected:
    QSet<QString> checkValue(const QModelIndex& index) const;

private:
    QMap <int, QVariant::Type> m_mapFieldTypes;
    QList<DescriptionHighlighFilter> m_filters;
};

#endif // PAINTTEXTDELEGATE_H
