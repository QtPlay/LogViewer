#ifndef DescDbTreeView_H
#define DescDbTreeView_H

#include <QWidget>
#include <QTableView>
#include <QTreeView>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSignalMapper>
#include <QtSql/QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <databasemanager.h>
#include "descdbtreeitem.h"

class DescDbTreeView : public QWidget
{
    Q_OBJECT
public:
    explicit DescDbTreeView(QWidget *parent = 0);

    QString getActiveProject();
    QString getActiveSortFilter();
    QString getActiveHighlightFilter();

signals:
    void s_clickedSortFilter(const QString& nameDatabase, const QString& filter);
    void s_clickedHighlightFilter(const QString& nameDatabase, const QString& filter);
    void s_clickedProject(const QString& nameDatabase);

    void s_showLogData(const QString& nameDatabase);
    void s_closeProject(const QString& nameDatabase);
    void s_removeProject(const QString& nameDatabase);
    void s_addFilter(const QString& nameDatabase, Filter::Type);
    void s_removeFilter(const QString& nameDatabase, const QString& filter);
    void s_changeFilter(const QString& nameDatabase, const QString& filter, Filter::Type);
    void s_applyFilter(const QString& nameDatabase, const QString& filter);

    void s_activeProjectChanged(const QString& project);
    void s_activeSortFilterChanged(const QString &project, const QString& filter);
    void s_activeHighlightFilterChanged(const QString &project, const QString& filter);

public slots:
    void updateTree();
    void newProject(const QString& nameDatabase);
    void updateFilter(const QString& nameDatabase);

private slots:
    void showContextMenu(const QPoint& pos);
    void showLogData();
    void closeProject();
    void removeProject();
    void addFilter();
    void removeFilter();
    void applyFilter();
    void changeFilter();
    void itemClicked(QTreeWidgetItem*, int);

protected:
    DescDbTreeItem *addItem(const QString& displayText, const QString& editorText,
                            DescDbTreeItem::TypeItemTree type, QTreeWidgetItem *parent = nullptr);
    DescDbTreeItem *addProject(const QString& nameDatabase);
    void updateFilter(const QString &name, DescDbTreeItem::TypeItemTree type,
                      const QString& nameDatabase, DescDbTreeItem *parent);
    void deleteChild(DescDbTreeItem *parent);

    void changeActiveProject(DescDbTreeItem *projectItem);
    void changeActiveSortFilter(DescDbTreeItem *filterItem);
    void changeActiveHighlightFilter(DescDbTreeItem *filterItem);

private:
    QTreeWidget *m_descDbTreeView;
    QMenu *m_projectMenu;
    QMenu *m_filterMenu;
    QMenu *m_filterFolderMenu;
    QMap <QString, DescDbTreeItem*> m_mapDbItem;

    DescDbTreeItem *m_activeProjectItem;
    DescDbTreeItem *m_activeSortFilterItem;
    DescDbTreeItem *m_activeHightlightFilterItem;

    QString m_activeProject;
    QString m_activeSortFilter;
    QString m_activeHightlightFilter;
};

#endif // DescDbTreeView_H
