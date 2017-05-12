#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include "databasecreator.h"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    enum RESULT_CODE {
        OPEN_OK,
        OPEN_FAILED,
        STRUCTURE_FAILED
    };

private:
    explicit DatabaseManager(QObject *parent = 0);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager& operator =(const DatabaseManager&) = delete;

public:
    static DatabaseManager &getInstance();
    QString getNameFilterTable() const;
    QString getNameDescSortFilter() const;
    QString getNameDescHighlightFilter() const;
    QString getNameLogDataTable() const;
    QString getNameInformationTable() const;
    QString getShortNameDatabase(const QString& fullNameDatabase);
    QList<QString> getListNameDatabase() const;
    QList<QString> getListNameAllFilter(const QString& nameDatabase);
    QList<QString> getListNameSortFilter(const QString& nameDatabase);
    QList<QString> getListNameHighlightFilter(const QString& nameDatabase);
    QMap <QString, QVariant::Type> getFieldTypes(const QString& nameDatabase);
    DatabaseCreator *getDatabaseCreator(const QString& fileName);
    SortFilter getSortFilter(const QString& nameDatabase, const QString& filter);
    HighlighFilter getHighlightFilter(const QString& nameDatabase, const QString& filter);
    QSqlQuery *getQuery(const QString& textQuery, const QString& nameDatabase);
    bool checkExistFilter(const QString &nameDatabase, const QString &filter);

signals:
    void s_openDatabaseFinished(int resultCode);
    void s_activeDatabaseChanged();
    void s_addDatabase(const QString& nameDatabase);
    void s_closeDatabase(const QString& nameDatabase);

public slots:
    bool openDatabase(const QString& nameDatabase);
    bool addFilter(const QString &nameDatabase, const QString& filter, QList<DescriptionFilter> descSort, QList<DescriptionHighlighFilter> descHighlight);
    bool changeFilter(const QString &nameDatabase, const QString& filter, QList<DescriptionFilter> descSort, QList<DescriptionHighlighFilter> descHighlight);
    bool deleteFilter(const QString &nameDatabase, const QString& filter);
    void closeDatabase(const QString& nameDatabase);

protected:
    QList<QString> getListNameFilters(const QString& textQuery, const QString& nameDatabase);
    bool checkStructureDatabase(QSqlDatabase db);

private:
    QMap <QString, QSqlDatabase> m_mapDatabase;
    QMap <QString, QString> m_mapShortName;
};

#endif // DATABASEMANAGER_H
