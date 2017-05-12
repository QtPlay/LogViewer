#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QList>
#include <QColorDialog>
#include <QMap>
#include <QRegExp>
#include <QRegExpValidator>

#include "filter.h"
#include "databasemanager.h"
#include "deschighlightdatamodel.h"
#include "descsortdatamodel.h"

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogStatus {
        CHANGE_FILTER,
        ADD_FILTER,
        INVALID
    };

    explicit FilterDialog(QWidget *parent = 0);
    ~FilterDialog();

signals:
    void s_changedFilter(const QString& project, const QString& name, QList<DescriptionFilter> descSort, QList<DescriptionHighlighFilter> descHighlight);
    void s_addedFilter(const QString& project, const QString& name, QList<DescriptionFilter> descSort, QList<DescriptionHighlighFilter> descHighlight);

public slots:
    void changeFilter(const QString& project, const QString& filter, Filter::Type type);
    void addFilter(const QString& project, Filter::Type type = Filter::SORT);

private slots:
    void settingForColumnType(QString column);
    void addDescSortFilter();
    void addDescHighlightFilter();
    void removeDescSortFilter();
    void removeDescHighlightFilter();
    void chooseColor();
    void applyChanges();
    void cancelChanges();
    bool checkExistFilter();

protected:
    bool checkParamSortFilter();
    bool checkParamHighlightFilter();
    void changeDialogStatus(DialogStatus dialogStatus);
    void clear();

private:
    Ui::FilterDialog *ui;
    QMap<QString, QVariant::Type> m_mapFieldTypes;
    SortFilter m_editSortFilter;
    HighlighFilter m_editHighlightFilter;
    DescHighlightDataModel *m_descHighlightDataModel;
    DescSortDataModel *m_descSortDataModel;
    QColor m_highlightColor;
    DialogStatus m_dialogStatus;
    QRegExpValidator *m_inputSortValidator;
    QRegExpValidator *m_inputHighlightValidator;
};

#endif // FILTERDIALOG_H
