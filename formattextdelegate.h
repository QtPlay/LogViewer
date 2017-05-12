#ifndef FORMATTEXTDELEGATE_H
#define FORMATTEXTDELEGATE_H

#include <QItemDelegate>
#include <QVariant>
#include <QDate>
#include <QTime>
#include <QDateEdit>
#include <QDebug>
#include <QtSql/QSqlQueryModel>

class FormatTextDelegate : public QItemDelegate
{
public:
    FormatTextDelegate(QSqlQueryModel *dataModel = nullptr);
    void setDataModel(QSqlQueryModel *dataModel);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QSqlQueryModel *m_dataModel;
};

#endif // FORMATTEXTDELEGATE_H
