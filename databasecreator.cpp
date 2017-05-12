#include "databasecreator.h"

DatabaseCreator::DatabaseCreator(QObject *parent) : QObject(parent)
{

}

DatabaseCreator::~DatabaseCreator()
{
    close();
}

bool DatabaseCreator::create(const QString &nameDatabaseFile, QList<QVariant::Type> listTypeVariable)
{
    QFileInfo fileInfo(nameDatabaseFile);
    QString fileName = QString("%1%2%3%4").arg(QDir::currentPath()).arg("/").arg(fileInfo.baseName()).arg(".db3");
    qDebug() << fileName;
    m_nameDatabaseFile = fileName;
    m_db = QSqlDatabase::addDatabase("QSQLITE", QLatin1String(nameDatabaseFile.toLatin1()));
    m_db.setDatabaseName(fileName);
    if (m_db.open()) {
        QString queryRequest;
        queryRequest.append("create table log_data (id integer primary key autoincrement");
        int fieldIndex = 0;
        foreach (QVariant::Type type, listTypeVariable) {
            queryRequest.append(QString(", field_%1 %2").arg(fieldIndex).arg(TypeConversionQVariantToSqlite::convert(type)));
            fieldIndex++;
        }
        queryRequest.append(")");
        QSqlQuery createQuery(m_db);

        if (!createQuery.exec(queryRequest)) {
            qDebug() << Q_FUNC_INFO << "Error create log table." << createQuery.lastError().text();
            return false;
        }

        queryRequest.clear();
        queryRequest.append("create table filter (id integer primary key autoincrement, name text unique not null)");
        if (!createQuery.exec(queryRequest)) {
            qDebug() << Q_FUNC_INFO << "Error create filter table." << createQuery.lastError().text();
            return false;
        }

        queryRequest.clear();
        queryRequest.append("create table filter_description ("
                            "filter_id integer primary key not null, "
                            "column_name text primary key not null, "
                            "sign_regexp integer not null, "
                            "filter_value text not null, "
                            "FOREIGN KEY (filter_id) REFERENCES filter(id))");
        if (!createQuery.exec(queryRequest)) {
            qDebug() << Q_FUNC_INFO << "Error create filter_description table." << createQuery.lastError().text();
            return false;
        }

        queryRequest.clear();
        queryRequest.append("create table filter_highlight_description ("
                            "filter_id integer primary key not null, "
                            "column_name text primary key not null, "
                            "sign_regexp integer not null,"
                            "color text not null, "
                            "filter_value text not null, "
                            "FOREIGN KEY (filter_id) REFERENCES filter(id))");
        if (!createQuery.exec(queryRequest)) {
            qDebug() << Q_FUNC_INFO << "Error create filter_highlight_description table." << createQuery.lastError().text();
            return false;
        }
    }
    return true;
}

bool DatabaseCreator::destroy()
{
    if (getStatusDatabase()) {
        m_db.close();
    }
    QFile file(m_nameDatabaseFile);
    return file.remove();
}

bool DatabaseCreator::openDatabase(const QString &fileName)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", QLatin1String(fileName.toLatin1()));
    if (m_db.open()) {


    }
    return false;
}

QSqlQuery DatabaseCreator::executeQuery(const QString &textQuery)
{
    QSqlQuery query(m_db);
    if (!getStatusDatabase()) return query;
    query.exec(textQuery);
    return query;
}

void DatabaseCreator::close()
{
    if (getStatusDatabase()) m_db.close();
}

bool DatabaseCreator::addRecord(QStringList values)
{
    if (!getStatusDatabase() || values.size() != m_listTypeVariable.size()) return false;

    QSqlQuery query(m_db);
    QString startPartRequest = QString("insert into log_data");
    QString descField;
    QString descValueField;

    int fieldIndex = 0;
    int countVariable = m_listTypeVariable.size();

    foreach (QVariant::Type type, m_listTypeVariable) {
        descField.append(QString("field_%1").arg(fieldIndex));
        switch (type) {
        case QVariant::String:
            descValueField.append(QString("'%1'").arg(values.at(fieldIndex)));
            break;
        default:
            descValueField.append(values.at(fieldIndex));
            break;
        }
        if (fieldIndex != countVariable -1) {
            descField.append(",");
            descValueField.append(",");
        }
        fieldIndex++;
    }
    query.prepare(QString("%1%2%3%4%5%6%7").arg(startPartRequest)
                  .arg(" (")
                  .arg(descField).arg(")")
                  .arg(" VALUES (")
                  .arg(descValueField)
                  .arg(")"));

    if (!query.exec()) {
        qDebug() << Q_FUNC_INFO << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseCreator::addSortFilter(SortFilter newSortFilter)
{
    if (!getStatusDatabase()) {
        qDebug() << Q_FUNC_INFO << "database is closed";
        return false;
    }

    bool signExistsFilter = false;
    int filterId = 0;
    std::tie(signExistsFilter, filterId) = checkExistsFilter(newSortFilter.name);

    if (!signExistsFilter) {
        QString textInsertQuery = QString("insert into filter (name) values ('%1')").arg(newSortFilter.name);
        QSqlQuery insertQuery(m_db);
        if (!insertQuery.exec(textInsertQuery)) {
            qDebug() << Q_FUNC_INFO << insertQuery.lastError().text();
            std::tie(signExistsFilter, filterId) = checkExistsFilter(newSortFilter.name);
            if (!signExistsFilter) {
                qDebug() << "Error insert new filter";
                exit(1);
            }

            QSqlQuery insertDescFilter(m_db);
            for (QList<DescriptionFilter>::const_iterator it = newSortFilter.listDescriptionFilter.cbegin();
                 it != newSortFilter.listDescriptionFilter.cend(); ++it) {
                QString textInsertDescFilterQuery = QString("insert into filter_description (%1, %2, %3, %4) values (%5, %6, %7, %8")
                        .arg("filter_id").arg("column_name").arg("sign_regexp").arg("filter_value")
                        .arg(filterId).arg((*it).columnName).arg((*it).signRegExp).arg((*it).filterValue);
                if (!insertDescFilter.exec(textInsertDescFilterQuery)) {
                    qDebug() << Q_FUNC_INFO << insertDescFilter.lastError().text();
                }
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Filter with name " + newSortFilter.name + " exists";
        return false;
    }
}

bool DatabaseCreator::updateSortFilter(SortFilter sortFilter)
{
    if (deleteFilter(sortFilter.name)) {
        return addSortFilter(sortFilter);
    } else {
        return false;
    }
}

bool DatabaseCreator::updateHighlighFilter(HighlighFilter highlighFilter)
{
    if (deleteFilter(highlighFilter.name)) {
        return true;//addHighlightFilter(highlighFilter);
    } else {
        return false;
    }
}

bool DatabaseCreator::deleteFilter(const QString &filterName)
{
    bool signExistFilter = false;
    int filterId = 0;
    std::tie(signExistFilter, filterId) = checkExistsFilter(filterName);
    if (signExistFilter) {
        QString textDeleteQuery = QString("DELETE FROM %1 WHERE %2 = %3");
        executeQuery(textDeleteQuery.arg("filter_description").arg("fiter_id").arg(filterId));
        executeQuery(textDeleteQuery.arg("filter_highlight_description").arg("fiter_id").arg(filterId));
        QSqlQuery query = executeQuery(textDeleteQuery.arg("filter").arg("id").arg(filterId));
        return query.isActive() && query.lastError().type() == QSqlError::NoError;
    } else {
        return true;
    }
}

QList<SortFilter> DatabaseCreator::getSortFilter()
{
    QString  textSelectFilterQuery = QString("SELECT * FROM %1");
    QString textSelectDescFilterQuery = QString("SELECT * FROM %1 WHERE %2 = %3");

    QList<SortFilter> listSortFilters;

    QSqlQuery selectFilterQuery = executeQuery(textSelectFilterQuery.arg("filter"));
    if (selectFilterQuery.isActive()) {
        while(selectFilterQuery.next()) {
            SortFilter sortFilter;
            sortFilter.id = selectFilterQuery.value(0).toInt();
            sortFilter.name = selectFilterQuery.value(1).toString();
            QSqlQuery selectDescFilterQuery = executeQuery(textSelectDescFilterQuery.arg("filter_description").arg("filter_id").arg(sortFilter.id));
            if (selectDescFilterQuery.isActive() && selectDescFilterQuery.lastError().type() == QSqlError::NoError) {
                while (selectDescFilterQuery.next()) {
                    DescriptionFilter descFilter;
                    descFilter.columnName = selectDescFilterQuery.value(1).toString();
                    descFilter.signRegExp = selectDescFilterQuery.value(2).toBool();
                    descFilter.filterValue = selectDescFilterQuery.value(3).toString();
                    sortFilter.listDescriptionFilter.append(descFilter);
                }
            }
            listSortFilters.append(sortFilter);
        }
    }
    return listSortFilters;
}

QList<HighlighFilter> DatabaseCreator::getHighlighFilter()
{
    QString  textSelectFilterQuery = QString("SELECT * FROM %1");
    QString textSelectDescFilterQuery = QString("SELECT * FROM %1 WHERE %2 = %3");

    QList<HighlighFilter> listHighlighFilters;

    QSqlQuery selectFilterQuery = executeQuery(textSelectFilterQuery.arg("filter"));
    if (selectFilterQuery.isActive()) {
        while(selectFilterQuery.next()) {
            HighlighFilter highlighFilter;
            highlighFilter.id = selectFilterQuery.value(0).toInt();
            highlighFilter.name = selectFilterQuery.value(1).toString();
            QSqlQuery selectDescFilterQuery = executeQuery(textSelectDescFilterQuery.arg("filter_highlight_description").arg("filter_id").arg(highlighFilter.id));
            if (selectDescFilterQuery.isActive() && selectDescFilterQuery.lastError().type() != QSqlError::NoError) {
                while (selectDescFilterQuery.next()) {
                    DescriptionHighlighFilter descFilter;
                    descFilter.columnName = selectDescFilterQuery.value(1).toString();
                    descFilter.signRegExp = selectDescFilterQuery.value(2).toBool();
                    descFilter.filterValue = selectDescFilterQuery.value(3).toString();
                    highlighFilter.listDescriptionFilter.append(descFilter);
                }
            }
            listHighlighFilters.append(highlighFilter);
        }
    }
    return listHighlighFilters;
}

std::tuple<bool, int> DatabaseCreator::checkExistsFilter(const QString &filterName)
{
    if (m_db.isOpen()) {
        QString textQuery = QString("select id from filter where name='%1'").arg(filterName);
        QSqlQuery query(m_db);
        if (query.exec(textQuery)) {
            if (query.next()) {
                return std::make_tuple(true, query.record().value(0).toInt());
            } else {
                return std::make_tuple(0, false);
            }
        } else {
            qDebug() << Q_FUNC_INFO << query.lastError().text();
        }
    } else {
        qDebug() << Q_FUNC_INFO << "database is closed";
    }
    return std::make_tuple(0, false);
}

std::tuple<bool, QSqlRecord> DatabaseCreator::checkExistsDescriptionFilter(Filter filter, DescriptionFilter descFilter, bool isSortFilter)
{
    if (!getStatusDatabase()) {
        qDebug() << Q_FUNC_INFO << "database is closed";
        return std::make_tuple(false, QSqlRecord());
    }

    QString descFilterTable = isSortFilter ? QString("filter_description") : QString("filter_highlight_description");
    QString textSelectQuery = QString("select * from %1 where %2=%3 and %4='%5'")
            .arg(descFilterTable)
            .arg("filter_id")
            .arg(filter.id)
            .arg("column_name")
            .arg(descFilter.columnName);
    QSqlQuery query(m_db);
    if (query.exec(textSelectQuery)) {
        qDebug() << Q_FUNC_INFO << query.lastError().text();
        return std::make_tuple(false, QSqlRecord());
        if (query.next()) {
            return std::make_tuple(true, query.record());
        }
        return std::make_tuple(false, QSqlRecord());
    }
}

bool DatabaseCreator::getStatusDatabase() const
{
    return m_db.isOpen();
}

bool DatabaseCreator::checkExistTable(const QString &tableName)
{
    QString checkExistTableQuery = QString("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = '%2'");
    QSqlQuery query = executeQuery(checkExistTableQuery.arg(tableName));
    if (query.isActive() && query.lastError().type() == QSqlError::NoError && query.next()) {
        if (query.value(0).toInt() == 1) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}
