#include "filterdialog.h"
#include "ui_filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    this->setWindowTitle("Add\Change filter");
    ui->setupUi(this);
    ui->project->setReadOnly(true);
    ui->sortRelationship->addItems(RelationSign::toStringList());
    ui->highlightRelationship->addItems(RelationSign::toStringList());

    connect(ui->sortColumn, SIGNAL(currentIndexChanged(QString)), this, SLOT(settingForColumnType(QString)));
    connect(ui->highlightColumn, SIGNAL(currentIndexChanged(QString)), this, SLOT(settingForColumnType(QString)));
    connect(ui->highlightChooseColor, SIGNAL(released()), this, SLOT(chooseColor()));
    connect(ui->sortAdd, SIGNAL(released()), this, SLOT(addDescSortFilter()));
    connect(ui->highlightAdd, SIGNAL(released()), this, SLOT(addDescHighlightFilter()));
    connect(ui->sortDelete, SIGNAL(released()), this, SLOT(removeDescSortFilter()));
    connect(ui->highlightDelete, SIGNAL(released()), this, SLOT(removeDescHighlightFilter()));

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(released()), this, SLOT(cancelChanges()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(released()), this, SLOT(applyChanges()));
    connect(ui->filter, SIGNAL(editingFinished()), this, SLOT(checkExistFilter()));

    QPalette redColorTextPalette;
    redColorTextPalette.setColor(QPalette::WindowText, Qt::red);
    ui->errorMessage->setPalette(redColorTextPalette);

    m_descSortDataModel = new DescSortDataModel();
    m_descHighlightDataModel = new DescHighlightDataModel();

    ui->sortTableView->setModel(m_descSortDataModel);
    ui->sortTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->highlightTableView->setModel(m_descHighlightDataModel);
    ui->highlightTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_inputSortValidator = new QRegExpValidator();
    m_inputHighlightValidator = new QRegExpValidator();
    ui->sortValue->setValidator(m_inputSortValidator);
    ui->highlightValue->setValidator(m_inputHighlightValidator);

    ui->highlightColor->setAutoFillBackground(true);
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::changeFilter(const QString &project, const QString &filter, Filter::Type type)
{
    changeDialogStatus(CHANGE_FILTER);
    ui->project->setText(project);
    ui->filter->setText(filter);
    ui->filter->setReadOnly(true);
    ui->tabWidget->tabBar()->hide();
    m_mapFieldTypes = DatabaseManager::getInstance().getFieldTypes(project);

    switch (type) {
    case Filter::SORT:
        ui->tabWidget->setCurrentWidget(ui->tabSort);
        ui->sortColumn->addItems(QStringList::fromSet(m_mapFieldTypes.keys().toSet()));
        m_editSortFilter = DatabaseManager::getInstance().getSortFilter(project, filter);
        m_descSortDataModel->addDescFilter(m_editSortFilter.listDescriptionFilter);
        break;
    case Filter::HIGHLIGHT:
        ui->tabWidget->setCurrentWidget(ui->tabHighlight);
        ui->highlightColumn->addItems(QStringList::fromSet(m_mapFieldTypes.keys().toSet()));
        m_editHighlightFilter = DatabaseManager::getInstance().getHighlightFilter(project, filter);
        m_descHighlightDataModel->addDescFilter(m_editHighlightFilter.listDescriptionFilter);
        break;
    default:
        break;
    }
}

void FilterDialog::addFilter(const QString &project, Filter::Type type)
{
    changeDialogStatus(ADD_FILTER);
    if (type == Filter::HIGHLIGHT) {
        ui->tabWidget->setCurrentWidget(ui->tabHighlight);
    } else if (type == Filter::SORT) {
        ui->tabWidget->setCurrentWidget(ui->tabSort);
    }
    ui->project->setText(project);
    ui->filter->setPlaceholderText("Input filte name...");
    ui->filter->setReadOnly(false);

    m_mapFieldTypes = DatabaseManager::getInstance().getFieldTypes(project);
    ui->sortColumn->addItems(QStringList::fromVector(m_mapFieldTypes.keys().toVector()));
    ui->highlightColumn->addItems(QStringList::fromVector(m_mapFieldTypes.keys().toVector()));
}

void FilterDialog::settingForColumnType(QString column)
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox->currentIndex() < 0) return;

    QLineEdit *inputText = nullptr;
    QComboBox *relationshipComboBox = nullptr;
    QCheckBox *signRegExpCheckBox = nullptr;
    QRegExpValidator *inputValidator = nullptr;
    if (comboBox == ui->sortColumn) {
        inputText = ui->sortValue;
        relationshipComboBox = ui->sortRelationship;
        inputValidator = m_inputSortValidator;
        signRegExpCheckBox = ui->sortRegExp;
        ui->sortValue->clear();
    } else if (comboBox == ui->highlightColumn) {
        inputText = ui->highlightValue;
        relationshipComboBox = ui->highlightRelationship;
        inputValidator = m_inputHighlightValidator;
        signRegExpCheckBox = ui->highlightRegExp;
        ui->highlightValue->clear();
    } else {
        return;
    }

    signRegExpCheckBox->setChecked(false);
    signRegExpCheckBox->setEnabled(false);
    relationshipComboBox->setEnabled(true);

    QVariant::Type columnType = m_mapFieldTypes.value(column);
    QString regExpInput;
    QString inputMask;

    switch (columnType) {
    case QVariant::LongLong:
        regExpInput = "-?[0-9]*";
        break;
    case QVariant::Double:
        regExpInput = "[+-]?\\d*\\.?\\d+";
        break;
    case QVariant::String: {
        regExpInput = ".{0,}";
        signRegExpCheckBox->setEnabled(true);
        relationshipComboBox->setCurrentText("=");
        relationshipComboBox->setEnabled(false);
        break;
    }
    case QVariant::Time:
        regExpInput = ".{0,}"; //"\\d{2}:\\d{2}";
        inputMask = "99:99:99";
        break;
    case QVariant::Date:
        regExpInput = ".{0,}"; //\\d{2}/\\d{2}/\\d{4}";
        inputMask = "99/99/9999";
        break;
    case QVariant::DateTime:
        regExpInput = ".{0,}"; //\\d{2}/\\d{2}/\\d{4} \\d{2}:\\d{2}";
        inputMask = "99/99/9999 99:99:99";
        break;
    default:
        regExpInput = ".{0,}";
        break;
    }

    inputText->setInputMask(inputMask);
    inputValidator->setRegExp(QRegExp(regExpInput));
}

void FilterDialog::addDescSortFilter()
{
    if (checkParamSortFilter()) {
        DescriptionFilter descFilter;
        descFilter.columnName = ui->sortColumn->currentText();
        descFilter.signRegExp = ui->sortRegExp->isChecked();
        descFilter.relationSign = RelationSign::fromText(ui->sortRelationship->currentText());
        descFilter.filterValue = ui->sortValue->text();
        m_descSortDataModel->addDescFilter(descFilter);
    }
}

void FilterDialog::addDescHighlightFilter()
{
    if (checkParamHighlightFilter()) {
        DescriptionHighlighFilter descFilter;
        descFilter.columnName = ui->highlightColumn->currentText();
        descFilter.signRegExp = ui->highlightRegExp->isChecked();
        descFilter.relationSign = RelationSign::fromText(ui->highlightRelationship->currentText());
        descFilter.filterValue = ui->highlightValue->text();
        descFilter.color = m_highlightColor;
        m_descHighlightDataModel->addDescFilter(descFilter);
    }
}

void FilterDialog::removeDescSortFilter()
{
    QModelIndex currentIndex = ui->sortTableView->currentIndex();
    if (!currentIndex.isValid()) return;
    m_descSortDataModel->removeDescFilter(currentIndex);
}

void FilterDialog::removeDescHighlightFilter()
{
    QModelIndex currentIndex = ui->highlightTableView->currentIndex();
    if (!currentIndex.isValid()) return;
    m_descHighlightDataModel->removeDescFilter(currentIndex);
}

void FilterDialog::chooseColor()
{
    m_highlightColor = QColorDialog::getColor(Qt::green, this);
    QPalette palette;
    palette.setColor(ui->highlightColor->backgroundRole(), m_highlightColor);
    ui->highlightColor->setPalette(palette);
}

void FilterDialog::applyChanges()
{
    switch (m_dialogStatus) {
    case CHANGE_FILTER:
        emit s_changedFilter(ui->project->text(), ui->filter->text(),
                             m_descSortDataModel->getAllData(), m_descHighlightDataModel->getAllData());
        break;
    case ADD_FILTER:
        emit s_addedFilter(ui->project->text(), ui->filter->text(),
                           m_descSortDataModel->getAllData(), m_descHighlightDataModel->getAllData());
        break;
    }
    clear();
}

void FilterDialog::cancelChanges()
{
    clear();
}

bool FilterDialog::checkExistFilter()
{
    if (DatabaseManager::getInstance().checkExistFilter(ui->project->text(), ui->filter->text())) {
        ui->errorMessage->setText(QString("Filter with name %1 exist.").arg(ui->filter->text()));
        return true;
    } else {
        ui->errorMessage->clear();
        return false;
    }
}

bool FilterDialog::checkParamSortFilter()
{
    return ui->sortColumn->currentIndex() >= 0 && !ui->sortValue->text().isEmpty() && ui->sortRelationship->currentIndex() >= 0;
}

bool FilterDialog::checkParamHighlightFilter()
{
    return ui->highlightColumn->currentIndex() >= 0 && !ui->highlightValue->text().isEmpty()
            && ui->highlightRelationship->currentIndex() >= 0 && m_highlightColor.isValid();
}

void FilterDialog::changeDialogStatus(FilterDialog::DialogStatus dialogStatus)
{
    m_dialogStatus = dialogStatus;
}

void FilterDialog::clear()
{
    m_descHighlightDataModel->clear();
    m_descSortDataModel->clear();
    ui->sortColumn->clear();
    ui->highlightColumn->clear();
    ui->sortValue->clear();
    ui->highlightValue->clear();
    m_highlightColor = QColor();
    ui->tabWidget->tabBar()->show();
    m_mapFieldTypes.clear();
}
