#ifndef LOGFILEDIALOG_H
#define LOGFILEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "loglineparser.h"

namespace Ui {
class LogFileDialog;
}

class LogFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogFileDialog(QWidget *parent = 0);
    ~LogFileDialog();

private slots:
    void openFile();
    void checkLogData();

protected:
    QMap <QString, LogLineParser::TokenDescription> getTokenDescription() const;

private:
    Ui::LogFileDialog *ui;
    QStringList m_exampleLogData;
};

#endif // LOGFILEDIALOG_H
