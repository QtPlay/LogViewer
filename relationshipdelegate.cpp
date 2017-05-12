#include "relationshipdelegate.h"

RelationshipDelegate::RelationshipDelegate():QItemDelegate()
{

}

void RelationshipDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString signTextValue = RelationSign::convert2TextValue(RelationSign::Sign(index.data().toInt()));
    drawDisplay(painter, option, option.rect, signTextValue);
}
