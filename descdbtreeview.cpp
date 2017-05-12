#include "descdbtreeview.h"

DescDbTreeView::DescDbTreeView(QWidget *parent) : QWidget(parent)
{
    m_descDbTreeView = new QTreeWidget();
    m_descDbTreeView->setColumnCount(1);
    m_descDbTreeView->setHeaderLabel("Открытые файлы");
    m_descDbTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_descDbTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_descDbTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_descDbTreeView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(m_descDbTreeView);
    setLayout(mainLayout);

    m_projectMenu = new QMenu(this);
    m_filterMenu = new QMenu(this);
    m_filterFolderMenu = new QMenu(this);

    QAction *addFilter = m_filterFolderMenu->addAction(QIcon(":/image/add.png"), tr("Добавить"));
    connect(addFilter, SIGNAL(triggered(bool)), this, SLOT(addFilter()));
    QAction *applyFilter = m_filterMenu->addAction(QIcon(":/image/ok-mark.png"), tr("Применить"));
    connect(applyFilter, SIGNAL(triggered(bool)), this, SLOT(applyFilter()));
    QAction *changeFilter = m_filterMenu->addAction(QIcon(":/image/exchange.png"), tr("Изменить"));
    connect(changeFilter, SIGNAL(triggered(bool)), this, SLOT(changeFilter()));
    QAction *removeFilter = m_filterMenu->addAction(QIcon(":/image/cancel.png"), tr("Удалить"));
    connect(removeFilter, SIGNAL(triggered(bool)), this, SLOT(removeFilter()));
    
    QAction *showLogData = m_projectMenu->addAction(QIcon(":/image/view.png"), tr("Отобразить данные лога"));
    connect(showLogData, SIGNAL(triggered(bool)), this, SLOT(showLogData()));    
    QAction *closeProject = m_projectMenu->addAction(QIcon(":/image/exit-to-app-button.png"), tr("Закрыть проект"));
    connect(closeProject, SIGNAL(triggered(bool)), this, SLOT(closeProject()));
    QAction *removeProject = m_projectMenu->addAction(QIcon(":/image/cancel.png"),tr("Удалить проект"));
    connect(removeProject, SIGNAL(triggered(bool)), this, SLOT(removeProject()));
    
    m_activeHightlightFilterItem = nullptr;
    m_activeProjectItem = nullptr;
    m_activeSortFilterItem = nullptr;
}

QString DescDbTreeView::getActiveProject()
{
    return m_activeProject;
}

QString DescDbTreeView::getActiveSortFilter()
{
    return m_activeSortFilter;
}

QString DescDbTreeView::getActiveHighlightFilter()
{
    return m_activeHightlightFilter;
}

void DescDbTreeView::updateTree()
{
    m_descDbTreeView->clear();
    QList<QString> listNameDatabase = DatabaseManager::getInstance().getListNameDatabase();
    for (auto it = listNameDatabase.cbegin(); it != listNameDatabase.cend(); ++it) {
        DescDbTreeItem *topItem = addProject(*it);
        topItem->setToolTip(0, (*it));
        m_descDbTreeView->addTopLevelItem(topItem);
    }
}

void DescDbTreeView::newProject(const QString &nameDatabase)
{
    addProject(nameDatabase);
}

void DescDbTreeView::showContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *widgetItem = m_descDbTreeView->currentItem();
    DescDbTreeItem *item = dynamic_cast<DescDbTreeItem*>(widgetItem);
    if (item) {
        QMenu *menu = nullptr;
        switch (item->itemType()) {
        case DescDbTreeItem::TypeItemTree::PROJECT_NAME:
            menu = m_projectMenu;
            break;
        case DescDbTreeItem::TypeItemTree::SORT_FILTER_FOLDER:
        case DescDbTreeItem::TypeItemTree::HIGHLIGHT_FILTER_FOLDER:
            menu = m_filterFolderMenu;
            break;
        case DescDbTreeItem::TypeItemTree::SORT_FILTER_NAME:
        case DescDbTreeItem::TypeItemTree::HIGHLIGHT_FILTER_NAME:
            menu = m_filterMenu;
        default:
            break;
        }
        if (item->itemType() != DescDbTreeItem::PROJECT_NAME && item->editorText() != m_activeProject) {
            return;
        }
        if (menu == nullptr) return;
        menu->exec(m_descDbTreeView->mapToGlobal(pos));
    }
}

void DescDbTreeView::showLogData()
{
    DescDbTreeItem *dbItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (dbItem && dbItem->itemType() == DescDbTreeItem::TypeItemTree::PROJECT_NAME) {
        changeActiveProject(dbItem);
        emit s_showLogData(dbItem->editorText());
        qDebug() << Q_FUNC_INFO << dbItem->editorText();
    }
}

void DescDbTreeView::closeProject()
{
    DescDbTreeItem *dbItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (dbItem && dbItem->itemType() == DescDbTreeItem::TypeItemTree::PROJECT_NAME) {
        m_descDbTreeView->removeItemWidget(m_descDbTreeView->currentItem(), 0);
        m_mapDbItem.remove(dbItem->editorText());
        emit s_closeProject(dbItem->editorText());
        qDebug() << Q_FUNC_INFO << dbItem->editorText();
        delete dbItem;
    }
}

void DescDbTreeView::removeProject()
{
    DescDbTreeItem *dbItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (dbItem && dbItem->itemType() == DescDbTreeItem::TypeItemTree::PROJECT_NAME) {
        m_descDbTreeView->removeItemWidget(m_descDbTreeView->currentItem(), 0);
        m_mapDbItem.remove(dbItem->editorText());
        emit s_removeProject(dbItem->editorText());
        qDebug() << Q_FUNC_INFO << dbItem->editorText();
        delete dbItem;
    }
}

void DescDbTreeView::addFilter()
{
    DescDbTreeItem *filterItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (filterItem && (filterItem->itemType() == DescDbTreeItem::TypeItemTree::SORT_FILTER_FOLDER
                       || filterItem->itemType() == DescDbTreeItem::TypeItemTree::HIGHLIGHT_FILTER_FOLDER)) {
        switch (filterItem->itemType()) {
        case DescDbTreeItem::SORT_FILTER_FOLDER:
            emit s_addFilter(filterItem->editorText(), Filter::SORT);
            break;
        case DescDbTreeItem::HIGHLIGHT_FILTER_FOLDER:
            emit s_addFilter(filterItem->editorText(), Filter::HIGHLIGHT);
            break;
        default:
            break;
        }
        qDebug() << Q_FUNC_INFO << filterItem->text(0) << filterItem->editorText();
    }
}

void DescDbTreeView::changeFilter()
{
    DescDbTreeItem *filterItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (filterItem && (filterItem->itemType() == DescDbTreeItem::TypeItemTree::SORT_FILTER_NAME
                       || filterItem->itemType() == DescDbTreeItem::TypeItemTree::HIGHLIGHT_FILTER_NAME)) {
        switch (filterItem->itemType()) {
        case DescDbTreeItem::SORT_FILTER_NAME:
            emit s_changeFilter(filterItem->editorText(), filterItem->text(0), Filter::SORT);
            break;
        case DescDbTreeItem::HIGHLIGHT_FILTER_NAME:
            emit s_changeFilter(filterItem->editorText(), filterItem->text(0), Filter::HIGHLIGHT);
            break;
        default:
            break;
        }
        qDebug() << Q_FUNC_INFO << filterItem->text(0) << filterItem->editorText();
    }
}

void DescDbTreeView::removeFilter()
{
    DescDbTreeItem *filterItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (filterItem && (filterItem->itemType() == DescDbTreeItem::TypeItemTree::SORT_FILTER_NAME
                       || filterItem->itemType() == DescDbTreeItem::TypeItemTree::HIGHLIGHT_FILTER_NAME)) {
        emit s_removeFilter(filterItem->text(0), filterItem->editorText());
        qDebug() << Q_FUNC_INFO << filterItem->text(0) << filterItem->editorText();
    }
}

void DescDbTreeView::applyFilter()
{
    DescDbTreeItem *filterItem = dynamic_cast<DescDbTreeItem*>(m_descDbTreeView->currentItem());
    if (filterItem && (filterItem->itemType() == DescDbTreeItem::SORT_FILTER_NAME
                       || filterItem->itemType() == DescDbTreeItem::HIGHLIGHT_FILTER_NAME)) {
        emit s_applyFilter(filterItem->text(0), filterItem->editorText());
        qDebug() << Q_FUNC_INFO << filterItem->text(0) << filterItem->editorText();

        switch (filterItem->itemType()) {
        case DescDbTreeItem::SORT_FILTER_NAME:
            changeActiveSortFilter(filterItem);
            break;
        case DescDbTreeItem::HIGHLIGHT_FILTER_NAME:
            changeActiveHighlightFilter(filterItem);
        default:
            break;
        }
    }
}

void DescDbTreeView::itemClicked(QTreeWidgetItem *item, int)
{
    DescDbTreeItem *dtItem = dynamic_cast<DescDbTreeItem*>(item);
    if (!dtItem) return;

    switch (dtItem->itemType()) {
    case DescDbTreeItem::PROJECT_NAME:
        emit s_clickedProject(dtItem->editorText());
        qDebug() << Q_FUNC_INFO << "project info" << dtItem->editorText();
        break;
    case DescDbTreeItem::SORT_FILTER_NAME:
        emit s_clickedSortFilter(dtItem->editorText(), dtItem->text(0));
        qDebug() << Q_FUNC_INFO << "sort filter" << dtItem->editorText() << dtItem->text(0);
        break;
    case DescDbTreeItem::HIGHLIGHT_FILTER_NAME:
        emit s_clickedHighlightFilter(dtItem->editorText(), dtItem->text(0));
        qDebug() << Q_FUNC_INFO << "highlight filter" << dtItem->editorText() << dtItem->text(0);
        break;
    default:
        break;
    }
}

DescDbTreeItem *DescDbTreeView::addItem(const QString &displayText, const QString &editorText,
                                        DescDbTreeItem::TypeItemTree type, QTreeWidgetItem *parent)
{
    DescDbTreeItem *newItem = new DescDbTreeItem(parent);
    newItem->setText(0, displayText);
    newItem->setEditorText(editorText);
    newItem->setType(type);
    return newItem;
}

DescDbTreeItem* DescDbTreeView::addProject(const QString &nameDatabase)
{
    QString shortNameDatabase = DatabaseManager::getInstance().getShortNameDatabase(nameDatabase);
    DescDbTreeItem *topItem = addItem(shortNameDatabase, nameDatabase, DescDbTreeItem::PROJECT_NAME);
    topItem->setToolTip(0, nameDatabase);
    m_descDbTreeView->addTopLevelItem(topItem);
    m_mapDbItem.insert(nameDatabase, topItem);
    updateFilter(tr("Сортировка"), DescDbTreeItem::SORT_FILTER_FOLDER, nameDatabase, topItem);
    updateFilter(tr("Подсветка"), DescDbTreeItem::HIGHLIGHT_FILTER_FOLDER, nameDatabase, topItem);
    return topItem;
}

void DescDbTreeView::updateFilter(const QString &nameDatabase)
{
    DescDbTreeItem *dbItem = m_mapDbItem.value(nameDatabase, nullptr);
    if (dbItem && dbItem->childCount() > 0) {
        deleteChild(dbItem);
        updateFilter(tr("Сортировка"), DescDbTreeItem::SORT_FILTER_FOLDER, nameDatabase, dbItem);
        updateFilter(tr("Подсветка"), DescDbTreeItem::HIGHLIGHT_FILTER_FOLDER, nameDatabase, dbItem);
    }
}

void DescDbTreeView::updateFilter(const QString& name, DescDbTreeItem::TypeItemTree type, const QString &nameDatabase, DescDbTreeItem *parent)
{
    DescDbTreeItem *filterFolder = addItem(name, nameDatabase, type, parent);
    DescDbTreeItem::TypeItemTree filterType;
    switch (type) {
    case DescDbTreeItem::SORT_FILTER_FOLDER:
        filterType = DescDbTreeItem::SORT_FILTER_NAME;
        break;
    case DescDbTreeItem::HIGHLIGHT_FILTER_FOLDER:
        filterType = DescDbTreeItem::HIGHLIGHT_FILTER_NAME;
        break;
    default:
        qWarning() << Q_FUNC_INFO << "Bad parent type";
        break;
    }
    QList<QString> filter = DatabaseManager::getInstance().getListNameSortFilter(nameDatabase);
    for (auto it = filter.cbegin(); it != filter.cend(); ++it) {
        addItem((*it), nameDatabase, filterType, filterFolder);
    }
}

void DescDbTreeView::deleteChild(DescDbTreeItem *parent)
{
    for (int i = 0; i < parent->childCount(); ++i) {
        DescDbTreeItem *childItem = dynamic_cast<DescDbTreeItem*>(parent->child(i));
        parent->removeChild(childItem);
        delete childItem;
    }
}

void DescDbTreeView::changeActiveProject(DescDbTreeItem *projectItem)
{
    if (projectItem == nullptr) {
        m_activeProject.clear();
        if (m_activeProjectItem != nullptr) {
            m_activeProjectItem->changeActive(false);
            m_activeProjectItem = nullptr;
        }
        emit s_activeProjectChanged(m_activeProject);
    } else {
        if (m_activeProject != projectItem->editorText()) {
            m_activeProject = projectItem->editorText();
            if (m_activeProjectItem != nullptr) {
                m_activeProjectItem->changeActive(false);
            }
            m_activeProjectItem = projectItem;
            m_activeProjectItem->changeActive(true);
            emit s_activeProjectChanged(m_activeProject);
            changeActiveHighlightFilter(nullptr);
            changeActiveSortFilter(nullptr);
        }
    }
}

void DescDbTreeView::changeActiveSortFilter(DescDbTreeItem *filterItem)
{
    if (filterItem == nullptr) {
        m_activeSortFilter.clear();
        if (m_activeSortFilterItem != nullptr) {
            m_activeSortFilterItem->changeActive(false);
            m_activeSortFilterItem = nullptr;
        }
        emit s_activeSortFilterChanged(m_activeProject, m_activeSortFilter);
    } else {
        DescDbTreeItem *dbItem = m_mapDbItem.value(filterItem->editorText());
        if (!dbItem) {
            qWarning() << Q_FUNC_INFO << "not found db item";
            return;
        }
        if (m_activeSortFilter != filterItem->text(0)) {
            m_activeSortFilter = filterItem->text(0);
            if (m_activeSortFilterItem != nullptr) {
                m_activeSortFilterItem->changeActive(false);
            }
            m_activeSortFilterItem = filterItem;
            m_activeSortFilterItem->changeActive(true);
            emit s_activeSortFilterChanged(m_activeProject, m_activeSortFilter);
        }
    }
}

void DescDbTreeView::changeActiveHighlightFilter(DescDbTreeItem *filterItem)
{
    if (filterItem == nullptr) {
        m_activeHightlightFilter.clear();
        if (m_activeHightlightFilterItem != nullptr) {
            m_activeHightlightFilterItem->changeActive(false);
            m_activeHightlightFilterItem = nullptr;
        }
        emit s_activeHighlightFilterChanged(m_activeProject, m_activeHightlightFilter);
    } else {
        DescDbTreeItem *dbItem = m_mapDbItem.value(filterItem->editorText());
        if (!dbItem) {
            qWarning() << Q_FUNC_INFO << "not found db item";
            return;
        }
        if (m_activeHightlightFilter != filterItem->text(0)) {
            m_activeHightlightFilter = filterItem->text(0);
            if (m_activeHightlightFilterItem != nullptr) {
                m_activeHightlightFilterItem->changeActive(false);
            }
            m_activeHightlightFilterItem = filterItem;
            m_activeHightlightFilterItem->changeActive(true);
            emit s_activeHighlightFilterChanged(m_activeProject, m_activeHightlightFilter);
        }
    }
}


