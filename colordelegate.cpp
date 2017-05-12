#include "colordelegate.h"

ColorDelegate::ColorDelegate():QItemDelegate()
{

}

void ColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->fillRect(option.rect, QBrush(QColor(index.data().toString())));
    painter->restore();
    drawDisplay(painter, option, option.rect, tr(""));
}
