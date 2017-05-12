#include "datamodel.h"

DataModel::DataModel(QObject *parent): QSqlQueryModel(parent)
{
    m_offsetData = 0;
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::TextColorRole && orientation == Qt::Vertical) {
        if (index(section, 0).data().toLongLong() > 7) {
            return QColor(Qt::red);
        }
    }
    if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return section + m_offsetData;
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}

void DataModel::setOffsetHeaderNumber(int offsetData)
{
    m_offsetData = offsetData;
}
