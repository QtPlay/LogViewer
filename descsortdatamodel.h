#ifndef DESCSORTDATAMODEL_H
#define DESCSORTDATAMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDebug>
#include "filter.h"

class DescSortDataModel : public QAbstractTableModel
{
public:
    DescSortDataModel(QObject *parent = nullptr);
    ~DescSortDataModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    void addDescFilter(DescriptionFilter descFilter);
    void addDescFilter(QList<DescriptionFilter> listDescFilter);
    void removeDescFilter(const QModelIndex& index);

    QList<DescriptionFilter> getAllData();
    DescriptionFilter getData(int row) const;
    void clear();

private:
    QList<DescriptionFilter> *m_data;
};

#endif // DESCSORTDATAMODEL_H
