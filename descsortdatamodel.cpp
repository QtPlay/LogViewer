#include "descsortdatamodel.h"

DescSortDataModel::DescSortDataModel(QObject *parent):QAbstractTableModel(parent)
{
    m_data = new QList<DescriptionFilter>();
}

DescSortDataModel::~DescSortDataModel()
{
    m_data->clear();
    delete m_data;
}

int DescSortDataModel::rowCount(const QModelIndex &parent) const
{
    return m_data->size();
}

int DescSortDataModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant DescSortDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    switch (section) {
    case 0:
        return QVariant("Column");
    case 1:
        return QVariant("SignRegExp");
    case 2:
        return QVariant("Relationship");
    case 3:
        return QVariant("Value");
    default:
        return QVariant();
    }
}

QVariant DescSortDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (role == Qt::CheckStateRole && index.column() == 1) {
        return m_data->at(index.row()).signRegExp;
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return QVariant(m_data->at(index.row()).columnName);
        case 1:
            return QVariant(m_data->at(index.row()).signRegExp);
        case 2:
            return QVariant(RelationSign::convert2TextValue(m_data->at(index.row()).relationSign));
        case 3:
            return QVariant(m_data->at(index.row()).filterValue);
        }
    }
    return QVariant();
}

void DescSortDataModel::addDescFilter(DescriptionFilter descFilter)
{
    beginInsertRows(QModelIndex(), m_data->size(), m_data->size());
    m_data->push_back(descFilter);
    endInsertRows();
}

void DescSortDataModel::addDescFilter(QList<DescriptionFilter> listDescFilter)
{
    for (auto it = listDescFilter.cbegin(); it != listDescFilter.cend(); ++it) {
        addDescFilter((*it));
    }
}

void DescSortDataModel::removeDescFilter(const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_data->removeAt(index.row());
    endRemoveRows();
}

QList<DescriptionFilter> DescSortDataModel::getAllData()
{
    return *m_data;
}

DescriptionFilter DescSortDataModel::getData(int row) const
{
    return m_data->at(row);
}

void DescSortDataModel::clear()
{
    m_data->clear();
}
