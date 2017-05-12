#ifndef RELATIONSHIPDELEGATE_H
#define RELATIONSHIPDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include "relationsign.h"

class RelationshipDelegate : public QItemDelegate
{
public:
    RelationshipDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // RELATIONSHIPDELEGATE_H
