#include "deschighlightdatamodel.h"

DescHighlightDataModel::DescHighlightDataModel(QObject *parent): QAbstractTableModel(parent)
{
    m_data = new QList<DescriptionHighlighFilter>();
}

DescHighlightDataModel::~DescHighlightDataModel()
{
    m_data->clear();
    delete m_data;
}

int DescHighlightDataModel::rowCount(const QModelIndex &parent) const
{
    return m_data->size();
}

int DescHighlightDataModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant DescHighlightDataModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    case 4:
        return QVariant("Color");
    default:
        return QVariant();
    }
}

QVariant DescHighlightDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (role == Qt::CheckStateRole && index.column() == 1) {
        return QVariant(m_data->at(index.row()).signRegExp);
    }

    if (role == Qt::BackgroundColorRole && index.column() == 4) {
        return QVariant(m_data->at(index.row()).color);
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
        case 4:
            return QVariant(m_data->at(index.row()).color.name());
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void DescHighlightDataModel::addDescFilter(DescriptionHighlighFilter descFilter)
{
    beginInsertRows(QModelIndex(), m_data->size(), m_data->size());
    m_data->push_back(descFilter);
    endInsertRows();
}

void DescHighlightDataModel::addDescFilter(QList<DescriptionHighlighFilter> listDescFilter)
{
    for (auto it = listDescFilter.cbegin(); it != listDescFilter.cend(); ++it) {
        addDescFilter((*it));
    }
}

void DescHighlightDataModel::removeDescFilter(const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_data->removeAt(index.row());
    endRemoveRows();
}

DescriptionHighlighFilter DescHighlightDataModel::getData(int row) const
{
    return m_data->at(row);
}

QList<DescriptionHighlighFilter> DescHighlightDataModel::getAllData()
{
    return *m_data;
}

void DescHighlightDataModel::clear()
{
    m_data->clear();
}
