#include "databasemanager.h"


DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{

}

DatabaseManager::~DatabaseManager()
{
    QList<QSqlDatabase> databases = m_mapDatabase.values();
    for (auto it = databases.begin(); it != databases.end(); ++it) {
        (*it).close();
    }
}

DatabaseManager &DatabaseManager::getInstance()
{
    static DatabaseManager m_dbManager;
    return m_dbManager;
}

bool DatabaseManager::openDatabase(const QString &nameDatabase)
{
    if (m_mapDatabase.contains(nameDatabase)) {
        QSqlDatabase db = m_mapDatabase.value(nameDatabase);
        if (!db.isOpen()) {
            if (!db.open()) {
                emit s_openDatabaseFinished(RESULT_CODE::OPEN_FAILED);
                return false;
            }
        }
        emit s_openDatabaseFinished(RESULT_CODE::OPEN_OK);
        emit s_activeDatabaseChanged();
    } else {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", nameDatabase);
        db.setDatabaseName(nameDatabase);
        if (!db.open()) {
            emit s_openDatabaseFinished(RESULT_CODE::OPEN_FAILED);
            return false;
        }
        if (!checkStructureDatabase(db)) {
            emit s_openDatabaseFinished(RESULT_CODE::STRUCTURE_FAILED);
            return false;
        }
        m_mapDatabase.insert(nameDatabase, db);
        QFileInfo fileInfo(nameDatabase);
        m_mapShortName.insert(nameDatabase, fileInfo.fileName());
        emit s_addDatabase(nameDatabase);
    }
    return true;
}

bool DatabaseManager::checkExistFilter(const QString &nameDatabase, const QString &filter)
{
    QString textQuery = QString("select count(id) from filter where filter.name = '%1'").arg(filter);
    QSqlQuery *query = getQuery(textQuery, nameDatabase);
    if (query == nullptr) return true;
    if (query->next()) {
        return query->value(0).toInt();
    }
}

bool DatabaseManager::addFilter(const QString& nameDatabase, const QString &filter, QList<DescriptionFilter> descSort, QList<DescriptionHighlighFilter> descHighlight)
{
    QString textInsertQuery = QString("insert into filter (name) values ('%1')").arg(filter);
    QSqlQuery *query = getQuery(textInsertQuery, nameDatabase);
    if (query == nullptr) return false;
    int lastId = query->lastInsertId().toInt();
    query->finish();
    delete query;
    query = nullptr;

    QMap <QString, QVariant::Type> mapFieldTypes = getFieldTypes(nameDatabase);
    if (mapFieldTypes.isEmpty()) return false;

    if (!descSort.isEmpty()) {
        QString textInsertDescSortFilter = QString("insert into sort_description (filter_id, column_name, sign_regexp, relationship, value) values %1");
        QString formatValues = QString("(%1, '%2', %3, %4, %5)");
        QString values;
        for (auto it = descSort.cbegin(); it != descSort.cend(); ++it) {
            QVariant::Type columnType = mapFieldTypes.value((*it).columnName, QVariant::Invalid);
            QString filterValue;
            switch (columnType) {
            case QVariant::String:
                filterValue = QString("'%1'").arg((*it).filterValue);
                break;
            case QVariant::Time:
                filterValue = QString("%1").arg(QTime::fromString((*it).filterValue, "hh:mm:ss").msecsSinceStartOfDay());
                break;
            case QVariant::Date: {
                QDate date = QDate::fromString((*it).filterValue, "dd/MM/yyyy");
                filterValue = QString("%1").arg(QDateTime(date).toSecsSinceEpoch());
                break;
            }
            case QVariant::DateTime:
                filterValue = QString("%1").arg(QDateTime::fromString((*it).filterValue, "dd/MM/yyyy hh:mm:ss").toSecsSinceEpoch());
                break;
            default:
                filterValue = QString("%1").arg((*it).filterValue);
                break;
            }
            values += formatValues.arg(lastId).arg((*it).columnName).arg((*it).signRegExp).arg((int)(*it).relationSign).arg(filterValue);
            if (it + 1 != descSort.cend()) {
                values += QString(", ");
            }
        }
        textInsertDescSortFilter = textInsertDescSortFilter.arg(values);
        query = getQuery(textInsertDescSortFilter, nameDatabase);
        if (query != nullptr) {
            query->finish();
            delete query;
            query = nullptr;
        }
    }

    if (!descHighlight.isEmpty()) {
        QString textInsertDescHighlightFilter = QString("insert into highlight_description (filter_id, column_name, sign_regexp, relationship, value, color) values %1");
        QString formatValues = QString("(%1, '%2', %3, %4, %5, %6)");
        QString values;
        for (auto it = descHighlight.cbegin(); it != descHighlight.cend(); ++it) {
            QVariant::Type columnType = mapFieldTypes.value((*it).columnName, QVariant::Invalid);
            QString filterValue;
            switch (columnType) {
            case QVariant::String:
                filterValue = QString("'%1'").arg((*it).filterValue);
                break;
            case QVariant::Time:
                filterValue = QString("%1").arg(QTime::fromString((*it).filterValue, "hh:mm:ss").msecsSinceStartOfDay());
                break;
            case QVariant::Date: {
                QDate date = QDate::fromString((*it).filterValue, "dd/MM/yyyy");
                filterValue = QString("%1").arg(QDateTime(date).toSecsSinceEpoch());
                break;
            }
            case QVariant::DateTime:
                filterValue = QString("%1").arg(QDateTime::fromString((*it).filterValue, "dd/MM/yyyy hh:mm:ss").toSecsSinceEpoch());
                break;
            default:
                filterValue = QString("%1").arg((*it).filterValue);
                break;
            }
            values += formatValues.arg(lastId).arg((*it).columnName).arg((*it).signRegExp).arg((int)(*it).relationSign).arg(filterValue).arg(QString("'%1'").arg((*it).color.name()));
            if (it + 1 != descHighlight.cend()) {
                values += QString(", ");
            }
        }
        textInsertDescHighlightFilter = textInsertDescHighlightFilter.arg(values);
        query = getQuery(textInsertDescHighlightFilter, nameDatabase);
        if (query != nullptr) {
            query->finish();
            delete query;
            query = nullptr;
        }
    }
}

bool DatabaseManager::changeFilter(const QString &nameDatabase, const QString &filter, QList<DescriptionFilter> descSort, QList<DescriptionHighlighFilter> descHighlight)
{
    deleteFilter(nameDatabase, filter);
    addFilter(nameDatabase, filter, descSort, descHighlight);
}

QString DatabaseManager::getNameFilterTable() const
{
    return tr("filter");
}

QString DatabaseManager::getNameDescSortFilter() const
{
    return tr("sort_description");
}

QString DatabaseManager::getNameDescHighlightFilter() const
{
    return tr("highlight_description");
}

QString DatabaseManager::getNameLogDataTable() const
{
    return tr("data");
}

QString DatabaseManager::getNameInformationTable() const
{
    return tr("information");
}

QString DatabaseManager::getShortNameDatabase(const QString &fullNameDatabase)
{
    return m_mapShortName.value(fullNameDatabase, fullNameDatabase);
}

QList<QString> DatabaseManager::getListNameDatabase() const
{
    return m_mapDatabase.keys();
}

QList<QString> DatabaseManager::getListNameFilters(const QString &textQuery, const QString &nameDatabase)
{
    if (textQuery.isEmpty() || nameDatabase.isEmpty() || !m_mapDatabase.contains(nameDatabase)) {
        return QList<QString>();
    }

    QList <QString> listNameFilters;
    QSqlQuery *query = getQuery(textQuery, nameDatabase);
    if (query == nullptr) {
        return QList<QString>();
    }

    while (query->next()) {
        listNameFilters << query->value(0).toString();
    }
    query->finish();
    delete query;
    return listNameFilters;
}

QList<QString> DatabaseManager::getListNameAllFilter(const QString &nameDatabase)
{
    if (nameDatabase.isEmpty() || !m_mapDatabase.contains(nameDatabase)) {
        return QList<QString>();
    }
    QString textQuery = QString("SELECT %1 FROM %2").arg(tr("name")).arg(tr("filter"));
    QList <QString> listNameFilters = getListNameFilters(textQuery, nameDatabase);
    return listNameFilters;
}

QList<QString> DatabaseManager::getListNameSortFilter(const QString &nameDatabase)
{
    if (nameDatabase.isEmpty() || !m_mapDatabase.contains(nameDatabase)) {
        return QList<QString>();
    }
    QString textQuery = QString("SELECT %1 FROM %2 JOIN %3 WHERE %2.%4 = %3.%5 group by %1")
            .arg(tr("name")).arg("filter").arg("sort_description")
            .arg(tr("id")).arg(tr("filter_id"));
    QList <QString> listNameFilters = getListNameFilters(textQuery, nameDatabase);
    return listNameFilters;
}

QList<QString> DatabaseManager::getListNameHighlightFilter(const QString &nameDatabase)
{
    if (nameDatabase.isEmpty() || !m_mapDatabase.contains(nameDatabase)) {
        return QList<QString>();
    }
    QString textQuery = QString("SELECT %1 FROM %2 JOIN %3 WHERE %2.%4 = %3.%5 group by %1")
            .arg(tr("name")).arg("filter").arg("highlight_description")
            .arg(tr("id")).arg(tr("filter_id"));
    QList <QString> listNameFilters = getListNameFilters(textQuery, nameDatabase);
    return listNameFilters;
}

QMap<QString, QVariant::Type> DatabaseManager::getFieldTypes(const QString &nameDatabase)
{
    QMap <QString, QVariant::Type> mapfieldTypes;
    if (nameDatabase.isEmpty() || !m_mapDatabase.contains(nameDatabase)) {
        return mapfieldTypes;
    }

    QString textSelectQuery = QString("SELECT %1 FROM %2").arg(tr("*")).arg(tr("field_description"));
    QSqlQuery *query = getQuery(textSelectQuery, nameDatabase);
    if (query != nullptr) {
        while (query->next()) {
            QString fieldName = query->value(0).toString();
            QVariant::Type type = QVariant::Type(query->value(1).toInt());
            mapfieldTypes.insert(fieldName, type);
        }
    }
    return mapfieldTypes;
}

DatabaseCreator *DatabaseManager::getDatabaseCreator(const QString& fileName)
{
    return new DatabaseCreator();
}

SortFilter DatabaseManager::getSortFilter(const QString &nameDatabase, const QString &filter)
{
    if (nameDatabase.isEmpty() || filter.isEmpty()) return SortFilter();

    QString textQuery = QString("select filter.id, filter.name, sort_description.column_name, "
                                "sort_description.sign_regexp, sort_description.relationship, "
                                "sort_description.value from filter join sort_description on "
                                "sort_description.filter_id = filter.id where filter.name = '%1'").arg(filter);
    QSqlQuery *query = getQuery(textQuery, nameDatabase);
    if (query == nullptr) return SortFilter();
    SortFilter sortFilter;
    if (query->next()) {
        sortFilter.id = query->value(0).toInt();
        sortFilter.name = query->value(1).toString();
        do {
            DescriptionFilter descFilter;
            descFilter.columnName = query->value(2).toString();
            descFilter.signRegExp = query->value(3).toBool();
            descFilter.relationSign = RelationSign::fromNumber(query->value(4).toInt());
            descFilter.filterValue = query->value(5).toString();
            sortFilter.listDescriptionFilter.append(descFilter);
        } while (query->next());
    }
    return sortFilter;
}

HighlighFilter DatabaseManager::getHighlightFilter(const QString &nameDatabase, const QString &filter)
{
    if (nameDatabase.isEmpty() || filter.isEmpty()) return HighlighFilter();

    QString textQuery = QString("select filter.id, filter.name, highlight_description.column_name, "
                                "highlight_description.sign_regexp, highlight_description.relationship, "
                                "highlight_description.value, highlight_description.color from filter "
                                "join highlight_description on highlight_description.filter_id = filter.id "
                                "where filter.name = '%1'").arg(filter);
    QSqlQuery *query = getQuery(textQuery, nameDatabase);
    if (query == nullptr) return HighlighFilter();
    HighlighFilter highlightFilter;
    if (query->next()) {
        highlightFilter.id = query->value(0).toInt();
        highlightFilter.name = query->value(1).toString();
        do {
            DescriptionHighlighFilter descFilter;
            descFilter.columnName = query->value(2).toString();
            descFilter.signRegExp = query->value(3).toBool();
            descFilter.relationSign = RelationSign::fromNumber(query->value(4).toInt());
            descFilter.filterValue = query->value(5).toString();
            descFilter.color = QColor(query->value(6).toString());
            highlightFilter.listDescriptionFilter.append(descFilter);
        } while (query->next());
    }
    return highlightFilter;
}

QSqlQuery *DatabaseManager::getQuery(const QString &textQuery, const QString &nameDatabase)
{
    QSqlQuery *query = new QSqlQuery(textQuery, m_mapDatabase.value(nameDatabase));
    if (query->lastError().type() != QSqlError::NoError) {
        qWarning() << Q_FUNC_INFO << query->lastError().text() << query->lastQuery();
        query->finish();
        delete query;
        return nullptr;
    }
    return query;
}

bool DatabaseManager::deleteFilter(const QString& nameDatabase, const QString &filter)
{
    QString textSelectQuery = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
            .arg(tr("id")).arg(tr("filter")).arg(tr("name")).arg(filter);
    QSqlQuery *query = getQuery(textSelectQuery, nameDatabase);
    if (query == nullptr) return false;
    qlonglong filterId = -1;
    if (query->next()) {
        filterId = query->value(0).toLongLong();
    }
    query->finish();
    delete query;
    query = nullptr;

    QString textDeleteQuery = QString("DELETE FROM %1 WHERE %2 = %3");
    query = getQuery(textDeleteQuery.arg(tr("sort_description")).arg(tr("filter_id")).arg(filterId), nameDatabase);
    if (query != nullptr) {
        query->finish();
        delete query;
        query = nullptr;
    }

    query = getQuery(textDeleteQuery.arg(tr("highlight_description")).arg(tr("filter_id")).arg(filterId), nameDatabase);
    if (query != nullptr) {
        query->finish();
        delete query;
        query = nullptr;
    }

    query = getQuery(textDeleteQuery.arg(tr("filter")).arg(tr("id")).arg(filterId), nameDatabase);
    if (query != nullptr) {
        query->finish();
        delete query;
        query = nullptr;
        return false;
    }
    return true;
}

void DatabaseManager::closeDatabase(const QString &nameDatabase)
{
    if (m_mapDatabase.contains(nameDatabase)) {
        QSqlDatabase db = m_mapDatabase.value(nameDatabase);
        if (db.isOpen()) {
            db.close();
        }
        m_mapDatabase.remove(nameDatabase);
        emit s_closeDatabase(nameDatabase);
    }
}

bool DatabaseManager::checkStructureDatabase(QSqlDatabase db)
{
    return true;
}
