#ifndef LOGDATAVIEW_H
#define LOGDATAVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QHeaderView>
#include <QScrollBar>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QSpacerItem>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSplitter>
#include "datamodel.h"
#include "filter.h"
#include "painttextdelegate.h"
#include "databasemanager.h"

class LogDataView : public QWidget
{
    Q_OBJECT
public:
    explicit LogDataView(QWidget *parent = 0);
    void hideColumn(int columnIndex);
    void showColumn(int columnIndex);

signals:
    void s_selectedRecord(QSqlRecord record);

public slots:
    void changeProject(const QString& project);
    void changeSortFilter(const QString& project, const QString& filter);
    void changeHighlightFilter(const QString& project, const QString& filter);

private slots:
    void doubleClickedData(QModelIndex index);
    void showContextMenu(const QPoint& pos);
    void showNextData();
    void showPrevData();

protected:
    QString formTextSortFilter(SortFilter filter);
    qlonglong getCountRecod(const QString& textQuery);
    bool updateData();

private:
    PaintTextDelegate *m_paintTextDelegate;
    QTableView *m_logDataView;
    DataModel *m_logDataModel;
    QString m_filter;
    QString m_filterSelectedFields;
    QStringList m_selectedFields;
    QMap <QString, QVariant::Type> m_mapFieldTypes;
    QSpinBox *m_sampleOffset;
    QSpinBox *m_sampleSize;
    QPlainTextEdit *m_fullInfoLogRecord;
    QLabel *m_countRecord;
    QMenu *m_contextMenu;
    QPushButton *m_forwardButton;
    QPushButton *m_backwardButton;
    QString m_currentProject;
    QString m_currentSortFilter;
    QString m_currentHighlightFilter;
};

#endif // LOGDATAVIEW_H
