#ifndef LOGFILEREADERWIDGET_H
#define LOGFILEREADERWIDGET_H

#include <QWidget>
#include "logfilereadertask.h"

namespace Ui {
class LogFileReaderWidget;
}

class LogFileReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogFileReaderWidget(const QString& fileName, QWidget *parent = 0);
    ~LogFileReaderWidget();

public slots:
    void onProgress(int doneCount, int sumCount);
    void onReadingFinished(int resultCode);

signals:
    void cancelReading();

private:
    Ui::LogFileReaderWidget *ui;
};

#endif // LOGFILEREADERWIDGET_H
