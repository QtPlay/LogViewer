#ifndef DESCHIGHLIGHTDATAMODEL_H
#define DESCHIGHLIGHTDATAMODEL_H

#include <QObject>
#include <QObject>
#include <QAbstractTableModel>
#include "filter.h"

class DescHighlightDataModel : public QAbstractTableModel
{
public:
    DescHighlightDataModel(QObject *parent = nullptr);
    ~DescHighlightDataModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addDescFilter(DescriptionHighlighFilter descFilter);
    void addDescFilter(QList<DescriptionHighlighFilter> listDescFilter);
    void removeDescFilter(const QModelIndex& index);

    DescriptionHighlighFilter getData(int row) const;
    QList<DescriptionHighlighFilter> getAllData();
    void clear();

private:
    QList<DescriptionHighlighFilter> *m_data;

};

#endif // DESCHIGHLIGHTDATAMODEL_H
