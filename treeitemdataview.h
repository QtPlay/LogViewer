#ifndef TREEITEMDATAVIEW_H
#define TREEITEMDATAVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLabel>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QHeaderView>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlRecord>
#include "databasemanager.h"
#include "colordelegate.h"
#include "relationshipdelegate.h"
#include "formattextdelegate.h"
#include "datamodel.h"

class TreeItemDataView : public QWidget
{
    Q_OBJECT
public:
    explicit TreeItemDataView(QWidget *parent = 0);

public slots:
    void changeSortFilter(const QString& project, const QString& filter);
    void changeHighlightFilter(const QString& project, const QString& filter);
    void changeProject(const QString& project);

    void changeVisibleProjectView(bool visible);
    void changeVisisbleSortFilterView(bool visible);
    void changeVisibleHighlightFilterView(bool visible);

protected:
    void hideColumn(QTableView *tableView, const QList<int> &hiddenColumn);
    void setHeaderName(QSqlQueryModel *dataModel, const QStringList &headerName);

private:
    QStringList m_columnNameSortView;
    QStringList m_columnNameHighlightView;

    QList<int> m_listHiddenColumnSortView;
    QList<int> m_listHiddenColumnHighlightView;

    QString m_currentProject;
    QString m_currentSortFilter;
    QString m_currentSortFilterProject;
    QString m_currentHighlightFilter;
    QString m_currentHighlightFilterProject;

    DataModel *m_sortDataModel;
    DataModel *m_highlightDataModel;

    QTableView *m_dataSortView;
    QTableView *m_dataHighlightView;
    QPlainTextEdit *m_dataDbView;

    QLabel *m_labelDataSortView;
    QLabel *m_labelDataHighlightView;
    QLabel *m_labelDataDbView;

    QWidget *m_containerDataSortView;
    QWidget *m_containerDataHighlightView;
    QWidget *m_containerDataDbView;
};

#endif // TREEITEMDATAVIEW_H
