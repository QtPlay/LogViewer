#ifndef LOGDATAMODEL_H
#define LOGDATAMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QColor>
#include <QDebug>

class DataModel : public QSqlQueryModel
{
public:
    DataModel(QObject *parent = nullptr);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setOffsetHeaderNumber(int offsetData);
private:
    int m_offsetData;
};

#endif // LOGDATAMODEL_H
