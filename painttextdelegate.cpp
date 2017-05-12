#include "painttextdelegate.h"

PaintTextDelegate::PaintTextDelegate(QObject *parent): QItemDelegate(parent)
{

}

void PaintTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_filters.isEmpty()) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    QSet<QString> acceptedColor = checkValue(index);
    if (!acceptedColor.isEmpty()) {
        painter->save();
        int width = option.rect.width() / acceptedColor.size();
        QRect rectangle = option.rect;
        rectangle.setWidth(width);
        int offsetFactor = 0;
        for (QSet<QString>::const_iterator it = acceptedColor.cbegin(); it != acceptedColor.cend(); ++it) {
            rectangle.setTopLeft(QPoint(rectangle.topLeft().x() + width * offsetFactor, rectangle.topLeft().y()));
            painter->fillRect(rectangle, QBrush(QColor((*it))));
            rectangle = option.rect;
            offsetFactor++;
        }
        painter->restore();
    }
    QVariant::Type displayType = m_mapFieldTypes.value(index.column(), QVariant::Invalid);
    if (displayType == QVariant::Invalid) {
        drawDisplay(painter, option, option.rect, index.data().toString());
    } else {
        QString displayText;
        switch (displayType) {
        case QVariant::Time:
            displayText = QTime::fromMSecsSinceStartOfDay(index.data().toLongLong()).toString("hh:mm:ss");
            break;
        case QVariant::Date:
            displayText = QDateTime::fromSecsSinceEpoch(index.data().toLongLong()).date().toString("dd/MM/yyyy");
            break;
        case QVariant::DateTime:
            displayText = QDateTime::fromSecsSinceEpoch(index.data().toLongLong()).toString("dd/MM/yyyy hh:mm:ss");
            break;
        default:
            displayText = index.data().toString();
            break;
        }
        drawDisplay(painter, option, option.rect, displayText);
    }
}

void PaintTextDelegate::setFilter(HighlighFilter filter)
{
    m_filters = filter.listDescriptionFilter;
}

void PaintTextDelegate::setFieldTypes(QMap<int, QVariant::Type> mapFieldTypes)
{
    m_mapFieldTypes = mapFieldTypes;
}

void PaintTextDelegate::resetFilter()
{
    m_filters.clear();
}

QSet<QString> PaintTextDelegate::checkValue(const QModelIndex &index) const
{
    QSet <QString> acceptedColors;
    QSet <QString> deniedColors;

    QVariant::Type editorType = index.data(Qt::EditRole).type();
    for (QList<DescriptionHighlighFilter>::const_iterator it = m_filters.cbegin(); it != m_filters.cend(); ++it) {
        bool compareResult = false;
        switch (editorType) {
        case QVariant::String:
            if ((*it).signRegExp) {
                compareResult = QRegExp((*it).filterValue).exactMatch(index.data().toString());
            } else {
                compareResult = index.data().toString() == (*it).filterValue;
            }
            break;
        case QVariant::Double:
            if ((*it).relationSign != RelationSign::NOT_VALID) {
                compareResult = CompareValue<double>::compare(index.data().toDouble(), (*it).filterValue.toDouble(), (*it).relationSign);
            }
            break;
        case QVariant::LongLong: case QVariant::Int: case QVariant::UInt:
            if ((*it).relationSign != RelationSign::NOT_VALID) {
                compareResult = CompareValue<qlonglong>::compare(index.data().toLongLong(), (*it).filterValue.toLongLong(), (*it).relationSign);
            }
            break;
        default:
            break;
        }
        QString colorName = (*it).color.name();
        if (!compareResult) {
            deniedColors.insert(colorName);
            if (acceptedColors.contains(colorName)) {
                acceptedColors.remove(colorName);
            }
        }
        if (compareResult && !deniedColors.contains(colorName)) {
            acceptedColors.insert(colorName);
        }
    }
    return acceptedColors;
}
