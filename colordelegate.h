#ifndef COLORDELEGATE_H
#define COLORDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QItemDelegate>

class ColorDelegate : public QItemDelegate
{
public:
    ColorDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // COLORDELEGATE_H
