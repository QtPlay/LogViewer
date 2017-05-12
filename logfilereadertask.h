#ifndef LOGFILEREADERTASK_H
#define LOGFILEREADERTASK_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QAtomicInt>
#include <QDebug>

class LogFileReaderTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum ResultCode {
        RESULT_OK,
        RESULT_FAILED,
        RESULT_CANCELED,
        RESULT_BAD_FORMAT
    };

    explicit LogFileReaderTask(const QString& fileName, const QString& formatDataFile, QObject *parent = 0);
    ~LogFileReaderTask();
    void run();

public slots:
    void cancel();
    void pause();
    void resume();

signals:
    void s_readingData(const QString& text);
    void s_progressChanged(int doneCount, int sumCount);
    void s_readingFinished(int resultCode);

private:
    QString m_fileName;
    QString m_formatDataFile;
    QAtomicInt m_cancelMarker;
    QAtomicInt m_stopMarker;
};

#endif // LOGFILEREADERTASK_H
