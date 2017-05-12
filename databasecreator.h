#ifndef DATABASECREATOR_H
#define DATABASECREATOR_H

#include <QObject>
#include <QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QDir>
#include "typeconversion.h"
#include "filter.h"

class DatabaseCreator : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseCreator(QObject *parent = 0);
    ~DatabaseCreator();
    bool create(const QString& nameDatabaseFile, QList<QVariant::Type> listTypeVariable);
    bool destroy();
    bool openDatabase(const QString& fileName);
    QSqlQuery executeQuery(const QString& textQuery);
    void close();

    bool addRecord(QStringList values);
    bool addSortFilter(SortFilter newSortFilter);
    bool addHighlightFilter(HighlighFilter newHighlighFilter);

    bool updateSortFilter(SortFilter sortFilter);
    bool updateHighlighFilter(HighlighFilter highlighFilter);

    bool deleteFilter(const QString& filterName);

    QList<SortFilter> getSortFilter();
    QList<HighlighFilter> getHighlighFilter();

    bool getStatusDatabase() const;

protected:
    bool checkExistTable(const QString& tableName);
    std::tuple<bool, int> checkExistsFilter(const QString& filterName);
    std::tuple<bool, QSqlRecord> checkExistsDescriptionFilter(Filter filter, DescriptionFilter descFilter, bool isSortFilter);

private:
    QSqlDatabase m_db;
    QString m_nameDatabaseFile;
    QList <QVariant::Type> m_listTypeVariable;
};

#endif // DATABASECREATOR_H
