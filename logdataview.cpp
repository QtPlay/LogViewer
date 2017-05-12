#include "logdataview.h"

LogDataView::LogDataView(QWidget *parent) : QWidget(parent),
    m_logDataModel(nullptr), m_logDataView(nullptr)
{
    m_logDataModel = new DataModel();
    m_logDataView = new QTableView(this);

    m_paintTextDelegate = new PaintTextDelegate();
    m_logDataView->setItemDelegate(m_paintTextDelegate);

    m_logDataView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_logDataView->setModel(m_logDataModel);
    connect(m_logDataView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickedData(QModelIndex)));
    m_logDataView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_logDataView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_logDataView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    QHBoxLayout *controlPanelLayout = new QHBoxLayout();
    controlPanelLayout->setSpacing(2);
    m_countRecord = new QLabel();
    controlPanelLayout->addWidget(new QLabel(tr("Всего строк:")));
    controlPanelLayout->addWidget(m_countRecord);
    controlPanelLayout->addWidget(new QLabel(tr("Параметры выборки:")));
    controlPanelLayout->addWidget(new QLabel(tr("Смещение:")));
    m_sampleOffset = new QSpinBox();
    m_sampleOffset->setMinimumWidth(80);
    m_sampleOffset->setRange(0, INT32_MAX);
    controlPanelLayout->addWidget(m_sampleOffset);
    controlPanelLayout->addWidget(new QLabel(tr("Размер:")));
    m_sampleSize = new QSpinBox();
    m_sampleSize->setMinimumWidth(80);
    m_sampleSize->setRange(0, 1000);
    controlPanelLayout->addWidget(m_sampleSize);

    m_forwardButton = new QPushButton(QIcon(":/image/forward-arrow.png"), tr(""), this);
    m_backwardButton = new QPushButton(QIcon(":/image/backward-arrow.png"), tr(""), this);
    connect(m_forwardButton, SIGNAL(released()), this, SLOT(showNextData()));
    connect(m_backwardButton, SIGNAL(released()), this, SLOT(showPrevData()));
    controlPanelLayout->addWidget(m_backwardButton);
    controlPanelLayout->addWidget(m_forwardButton);

    m_fullInfoLogRecord = new QPlainTextEdit();
    m_fullInfoLogRecord->setReadOnly(true);
    m_fullInfoLogRecord->setWordWrapMode(QTextOption::WrapAnywhere);
    m_fullInfoLogRecord->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *upperPartViewLayout = new QVBoxLayout();
    upperPartViewLayout->setMargin(0);
    upperPartViewLayout->setSpacing(2);
    upperPartViewLayout->addWidget(m_logDataView);
    upperPartViewLayout->addLayout(controlPanelLayout);

    QVBoxLayout *lowerPartViewLayout = new QVBoxLayout();
    lowerPartViewLayout->setMargin(0);
    lowerPartViewLayout->setSpacing(2);
    lowerPartViewLayout->addWidget(m_fullInfoLogRecord);

    QWidget *upperPartView = new QWidget();
    upperPartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    upperPartView->setLayout(upperPartViewLayout);
    QWidget *lowerPartView = new QWidget();
    lowerPartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lowerPartView->setLayout(lowerPartViewLayout);

    QSplitter *splitterWidget = new QSplitter(Qt::Vertical);
    splitterWidget->setMinimumHeight(2);
    splitterWidget->addWidget(upperPartView);
    splitterWidget->addWidget(lowerPartView);
    splitterWidget->setStretchFactor(0, 7);
    splitterWidget->setStretchFactor(1, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(splitterWidget);
    setLayout(mainLayout);

    m_contextMenu = new QMenu(this);
    QAction *actionForwardSample = m_contextMenu->addAction(QIcon(":/image/forward-arrow.png"), tr("Вперед"));
    connect(actionForwardSample, SIGNAL(triggered(bool)), m_forwardButton, SLOT(click()));
    QAction *actionBackwardSample = m_contextMenu->addAction(QIcon(":/image/backward-arrow.png"), tr("Назад"));
    connect(actionBackwardSample, SIGNAL(triggered(bool)), m_backwardButton, SLOT(click()));

    m_filterSelectedFields = QString("*");
    m_sampleSize->setValue(100);
}

void LogDataView::hideColumn(int columnIndex)
{
    m_logDataView->hideColumn(columnIndex);
}

void LogDataView::showColumn(int columnIndex)
{
    m_logDataView->showColumn(columnIndex);
}

QString LogDataView::formTextSortFilter(SortFilter filter)
{
    QString textFilter;
    if (m_mapFieldTypes.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Not found info about field types";
        return textFilter;
    }

    for (auto it = filter.listDescriptionFilter.cbegin(); it != filter.listDescriptionFilter.cend(); ++it) {
        QString sortCondition;
        if ((*it).signRegExp) {
            sortCondition = QString("%1 regexp('%2')").arg((*it).columnName).arg((*it).filterValue);
        } else {
            QVariant::Type fieldType = m_mapFieldTypes.value((*it).columnName, QVariant::Invalid);
            if (fieldType == QVariant::Invalid) {
                qWarning() << Q_FUNC_INFO << "Not found info about field type.";
                continue;
            }
            switch (fieldType) {
            case QVariant::String:
                sortCondition = QString("%1 %2 '%3'").arg((*it).columnName).arg("=").arg((*it).filterValue);
                break;
            default:
                sortCondition = QString("%1 %2 %3").arg((*it).columnName).arg(RelationSign::convert2TextValue((*it).relationSign)).arg((*it).filterValue);
                break;
            }
        }
        textFilter.append(sortCondition);
        if (it + 1 != filter.listDescriptionFilter.cend()) {
            textFilter.append(" AND ");
        }
    }
    if (!textFilter.isEmpty()) {
        textFilter = QString("WHERE %1").arg(textFilter);
    }
    return textFilter;
}

void LogDataView::changeProject(const QString &project)
{
    if (project.isEmpty()) {
        m_currentProject.clear();
        m_currentSortFilter.clear();
        m_currentHighlightFilter.clear();
        m_logDataView->setEnabled(false);
        m_sampleOffset->setEnabled(false);
        m_sampleSize->setEnabled(false);
        m_forwardButton->setEnabled(false);
        m_backwardButton->setEnabled(false);
    }
    if (project == m_currentProject) {
        return;
    } else {
        m_currentProject = project;
        m_currentSortFilter.clear();
        m_currentHighlightFilter.clear();
        m_logDataView->setEnabled(true);
        m_sampleOffset->setEnabled(true);
        m_sampleSize->setEnabled(true);
        m_forwardButton->setEnabled(true);
        m_backwardButton->setEnabled(true);
        QString textQuery = QString("SELECT %1 FROM %2").arg("COUNT(id)").arg("data");
        int m_countData = getCountRecod(textQuery);
        m_countRecord->setNum(m_countData);
        m_sampleOffset->setValue(0);
        m_logDataModel->setOffsetHeaderNumber(0);
        updateData();
    }
}

void LogDataView::changeSortFilter(const QString &project, const QString &filter)
{
    if (m_currentSortFilter == filter) return;
    QString textQuery;
    if (filter.isEmpty()) {
        m_filter.clear();
        textQuery = QString("SELECT %1 FROM %2 %3").arg("COUNT(id)").arg("data");
    } else {
        SortFilter sortFilter = DatabaseManager::getInstance().getSortFilter(project, filter);
        m_filter = formTextSortFilter(sortFilter);
        textQuery = QString("SELECT %1 FROM %2 %3").arg("COUNT(id)").arg("data").arg(m_filter);
    }
    int m_countData = getCountRecod(textQuery);
    m_countRecord->setNum(m_countData);
    m_sampleOffset->setValue(0);
    updateData();
}

void LogDataView::changeHighlightFilter(const QString &project, const QString& filter)
{
    if (filter.isEmpty()) {
        m_currentHighlightFilter = filter;
        m_paintTextDelegate->resetFilter();
    }
    if (m_currentHighlightFilter == filter) return;

    HighlighFilter highlightFilter = DatabaseManager::getInstance().getHighlightFilter(project, filter);
    m_paintTextDelegate->setFilter(highlightFilter);
}

bool LogDataView::updateData()
{
    QString textQuery = QString("SELECT %1 FROM %2 %3 LIMIT %4 OFFSET %5")
            .arg(tr("*")).arg(tr("data")).arg(m_filter)
            .arg(m_sampleSize->value()).arg(m_sampleOffset->value());
    qDebug() << Q_FUNC_INFO << textQuery;
    QSqlQuery *query = DatabaseManager::getInstance().getQuery(textQuery, m_currentProject);
    if (query == nullptr) return false;
    m_logDataModel->setQuery(*query);
    m_logDataView->hideColumn(0);
    return true;
}

void LogDataView::doubleClickedData(QModelIndex index)
{
    if (!index.isValid() || m_logDataModel == nullptr)return;

    QString description;
    for (int i = 0; i < m_logDataModel->columnCount(); ++i) {
        if (!m_logDataView->isColumnHidden(i)) {
            description += QString("%1: %2, ")
                    .arg(m_logDataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString())
                    .arg(m_logDataModel->index(index.row(), i).data().toString());
        }
    }
    m_fullInfoLogRecord->clear();
    m_fullInfoLogRecord->appendPlainText(description);
}

void LogDataView::showContextMenu(const QPoint &pos)
{
    qDebug() << Q_FUNC_INFO;
    m_contextMenu->exec(m_logDataView->mapToGlobal(pos));
}

void LogDataView::showNextData()
{
    int offset = m_sampleOffset->value();
    offset += m_sampleSize->value();
    int countData = m_countRecord->text().toInt();
    if (offset > countData) {
        offset = countData;
    }
    m_sampleOffset->setValue(offset);
    m_logDataModel->setOffsetHeaderNumber(offset);
    updateData();
}

void LogDataView::showPrevData()
{
    int offset = m_sampleOffset->value();
    offset -= m_sampleSize->value();
    if (offset < 0) offset = 0;
    m_sampleOffset->setValue(offset);
    m_logDataModel->setOffsetHeaderNumber(offset);
    updateData();
}

qlonglong LogDataView::getCountRecod(const QString &textQuery)
{
    QSqlQuery *query = DatabaseManager::getInstance().getQuery(textQuery, m_currentProject);
    if (query == nullptr || !query->next()) return 0;
    qlonglong countRecord = query->value(0).toLongLong();
    query->finish();
    delete query;
    return countRecord;
}
