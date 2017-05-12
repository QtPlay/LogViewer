#include "treeitemdataview.h"

TreeItemDataView::TreeItemDataView(QWidget *parent) : QWidget(parent)
{
    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_dataDbView = new QPlainTextEdit(this);
    m_dataDbView->setSizePolicy(sizePolicy);

    m_dataSortView = new QTableView(this);
    m_dataSortView->verticalHeader()->hide();
    m_dataSortView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dataSortView->setSizePolicy(sizePolicy);
    m_dataSortView->setItemDelegateForColumn(3, new RelationshipDelegate());
    m_sortDataModel = new DataModel();
    m_dataSortView->setItemDelegateForColumn(4, new FormatTextDelegate(m_sortDataModel));
    m_dataSortView->setModel(m_sortDataModel);

    m_dataHighlightView = new QTableView(this);
    m_dataHighlightView->verticalHeader()->hide();
    m_dataHighlightView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dataHighlightView->setSizePolicy(sizePolicy);
    m_dataHighlightView->setItemDelegateForColumn(5, new ColorDelegate());
    m_dataHighlightView->setItemDelegateForColumn(3, new RelationshipDelegate());
    m_highlightDataModel = new DataModel();
    m_dataHighlightView->setItemDelegateForColumn(4, new FormatTextDelegate(m_highlightDataModel));
    m_dataHighlightView->setModel(m_highlightDataModel);

    m_labelDataDbView = new QLabel(tr("Информация о проекте"));
    m_labelDataSortView = new QLabel(tr("Сортировка"));
    m_labelDataHighlightView = new QLabel(tr("Подсветка"));

    m_containerDataDbView = new QWidget();
    m_containerDataDbView->setSizePolicy(sizePolicy);
    m_containerDataSortView = new QWidget();
    m_containerDataSortView->setSizePolicy(sizePolicy);
    m_containerDataHighlightView = new QWidget();
    m_containerDataHighlightView->setSizePolicy(sizePolicy);

    QVBoxLayout *dataDbViewLayout = new QVBoxLayout();
    dataDbViewLayout->setMargin(0);
    dataDbViewLayout->setSpacing(1);
    dataDbViewLayout->addWidget(m_labelDataDbView);
    dataDbViewLayout->addWidget(m_dataDbView);
    m_containerDataDbView->setLayout(dataDbViewLayout);

    QVBoxLayout *dataSortViewLAyout = new QVBoxLayout();
    dataSortViewLAyout->setMargin(0);
    dataSortViewLAyout->setSpacing(1);
    dataSortViewLAyout->addWidget(m_labelDataSortView);
    dataSortViewLAyout->addWidget(m_dataSortView);
    m_containerDataSortView->setLayout(dataSortViewLAyout);

    QVBoxLayout *dataHighlightViewLAyout = new QVBoxLayout();
    dataHighlightViewLAyout->setMargin(0);
    dataHighlightViewLAyout->setSpacing(1);
    dataHighlightViewLAyout->addWidget(m_labelDataHighlightView);
    dataHighlightViewLAyout->addWidget(m_dataHighlightView);
    m_containerDataHighlightView->setLayout(dataHighlightViewLAyout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(m_containerDataDbView);
    mainLayout->addWidget(m_containerDataSortView);
    mainLayout->addWidget(m_containerDataHighlightView);
    setLayout(mainLayout);

    m_listHiddenColumnSortView << 0 << 2 << 5 << 6;
    m_listHiddenColumnHighlightView << 0 << 2 << 6 << 7;

    m_columnNameSortView << tr("id") << tr("Поле") << tr("Рег.зн") << tr("Знак")
                         << tr("Фильтр") << tr("Тип");
    m_columnNameHighlightView << tr("id") << tr("Поле") << tr("Рег.зн")
                              << tr("Знак") << tr("Фильтр") << tr("Цвет") << tr("Тип");
}

void TreeItemDataView::changeSortFilter(const QString &project, const QString &filter)
{
    if (filter.isEmpty()) {
        changeVisisbleSortFilterView(false);
        m_sortDataModel->clear();
        return;
    }
    if (project == m_currentSortFilterProject && filter == m_currentSortFilter) return;
    QString textQuery = QString("SELECT %1 FROM %2 JOIN %3 ON %2.%4 = %3.%5 and %3.%6 = '%7' JOIN %8 ON %2.%9 = %8.%10");
    textQuery = textQuery.arg("sort_description.*, field_description.type").arg("sort_description")
            .arg("filter").arg("filter_id").arg("id").arg("name").arg(filter)
            .arg("field_description").arg("column_name").arg("name");
    QSqlQuery *query = DatabaseManager::getInstance().getQuery(textQuery, project);
    if (query != nullptr) {
        m_sortDataModel->setQuery(*query);
        setHeaderName(m_sortDataModel, m_columnNameSortView);
        hideColumn(m_dataSortView, m_listHiddenColumnSortView);
        if (!m_dataSortView->isVisible()) {
            changeVisisbleSortFilterView(true);
        }
    }
}

void TreeItemDataView::changeHighlightFilter(const QString &project, const QString &filter)
{
    if (filter.isEmpty()) {
        changeVisibleHighlightFilterView(false);
        m_highlightDataModel->clear();
        return;
    }
    if (project == m_currentHighlightFilterProject && filter == m_currentHighlightFilter) return;
    QString textQuery = QString("SELECT %1 FROM %2 JOIN %3 ON %2.%4 = %3.%5 and %3.%6 = '%7' JOIN %8 ON %2.%9 = %8.%10");
    textQuery = textQuery.arg("highlight_description.*, field_description.type").arg("highlight_description")
            .arg("filter").arg("filter_id").arg("id").arg("name").arg(filter)
            .arg("field_description").arg("column_name").arg("name");
    QSqlQuery *query = DatabaseManager::getInstance().getQuery(textQuery, project);
    if (query != nullptr) {
        m_highlightDataModel->setQuery(*query);
        setHeaderName(m_highlightDataModel, m_columnNameHighlightView);
        hideColumn(m_dataHighlightView, m_listHiddenColumnHighlightView);
        if (!m_dataHighlightView->isVisible()) {
            changeVisibleHighlightFilterView(true);
        }
    }
}

void TreeItemDataView::changeProject(const QString &project)
{
    if (project.isEmpty()) {
        m_dataDbView->clear();
        changeVisibleProjectView(false);
        return;
    }
    if (project == m_currentProject) return;

    m_dataDbView->clear();
    QString textQuery = QString("SELECT %1 FROM %2").arg(tr("*")).arg(tr("information"));
    QSqlQuery *query = DatabaseManager::getInstance().getQuery(textQuery, project);
    if (query != nullptr && query->next()) {
        QString text;
        for (int i = 0; i < query->record().count(); ++i) {
            switch (i) {
            case 0:
                text.append("Название:").append(query->value(i).toString());
                break;
            case 1:
                text.append("Имя файла:").append(query->value(i).toString());
                break;
            case 2:
                text.append("Размер в байтах:").append(query->value(i).toString());
                break;
            case 3:
                text.append("Последнее чтение:").append(query->value(i).toString());
                break;
            case 4:
                text.append("Формат:").append(query->value(i).toString());
                break;
            default:
                text.append(query->value(i).toString());
                break;
            }
            text.append("\n");
        }
        query->finish();
        delete query;
        m_dataDbView->appendPlainText(text);

        if (!m_dataDbView->isVisible()) {
            changeVisibleProjectView(true);
        }
    }
}

void TreeItemDataView::changeVisibleProjectView(bool visible)
{
    m_containerDataDbView->setVisible(visible);
}

void TreeItemDataView::changeVisisbleSortFilterView(bool visible)
{
    m_containerDataSortView->setVisible(visible);
}

void TreeItemDataView::changeVisibleHighlightFilterView(bool visible)
{
    m_containerDataSortView->setVisible(visible);
}

void TreeItemDataView::hideColumn(QTableView *tableView, const QList<int> &hiddenColumn)
{
    for (auto it = hiddenColumn.cbegin(); it != hiddenColumn.cend(); ++it) {
        tableView->hideColumn((*it));
    }
}

void TreeItemDataView::setHeaderName(QSqlQueryModel *dataModel, const QStringList &headerName)
{
    qDebug() << dataModel->columnCount() << headerName.size();
    if (dataModel == nullptr || dataModel->columnCount() != headerName.size()) return;

    qDebug() << "test";
    int section = 0;
    for (auto it = headerName.cbegin(); it != headerName.cend(); ++it) {
        dataModel->setHeaderData(section, Qt::Horizontal, QVariant(*it), Qt::DisplayRole);
        section++;
    }
}
