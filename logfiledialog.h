#ifndef LOGFILEDIALOG_H
#define LOGFILEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QLocale>
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
    void clearHistory();
    void selectFormatFromHistory();
    void resetLocale();
    void updateLocale(int newIndex);

protected:
    QMap <QString, LogLineParser::TokenDescription> getTokenDescription() const;

private:
    Ui::LogFileDialog *ui;
    QStringList m_exampleLogData;
    LogLineParser *m_logLineParser;
    QLocale m_currentLocale;
    QHash <QString, QLocale::Language> m_hashLanguages;
};

#endif // LOGFILEDIALOG_H
