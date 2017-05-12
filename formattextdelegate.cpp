#include "formattextdelegate.h"

FormatTextDelegate::FormatTextDelegate(QSqlQueryModel *dataModel):QItemDelegate()
  , m_dataModel(dataModel)
{

}

void FormatTextDelegate::setDataModel(QSqlQueryModel *dataModel)
{
    m_dataModel = dataModel;
}

void FormatTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_dataModel == nullptr) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    QModelIndex indexTypeField = m_dataModel->index(index.row(), m_dataModel->columnCount() - 1);
    if (indexTypeField.isValid()) {
        QVariant::Type type = QVariant::Type(indexTypeField.data().toInt());
        QString displayText;
        switch (type) {
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
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}
