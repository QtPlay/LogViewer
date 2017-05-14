#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFileInfo>
#include <QThreadPool>
#include <QDateTime>
#include <QDir>
#include <QLocale>
#include <QMessageBox>

#include "logfilereadertask.h"
#include "logfilereaderwidget.h"
#include "loglineparser.h"
#include "databasecreator.h"
#include "logdataview.h"
#include "descdbtreeview.h"
#include "treeitemdataview.h"
#include "filterdialog.h"
#include "logfiledialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static void showMessage(QMessageBox::Icon icon, const QString& title, const QString& content);

private slots:
    void openLogFile();
    void openProject();
    void addSortFilter();
    void addHighlightFilter();

private:
    Ui::MainWindow *ui;
    LogDataView *m_logDataView;
    DescDbTreeView *m_descDbTreeView;
    TreeItemDataView *m_treeItemDataView;
    FilterDialog *m_filterDialog;
    LogFileDialog *m_logFileDialog;
};

#endif // MAINWINDOW_H
