#include "logfilereadertask.h"

LogFileReaderTask::LogFileReaderTask(const QString &fileName, const QString& formatDataFile, QObject *parent) : QObject(parent),
    m_fileName(fileName), m_formatDataFile(formatDataFile), m_cancelMarker(false), m_stopMarker(false)
{

}

LogFileReaderTask::~LogFileReaderTask()
{
    qDebug() << Q_FUNC_INFO;
}

void LogFileReaderTask::run()
{
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit s_readingFinished(RESULT_FAILED);
        return;
    }

    QTextStream textStream(&file);
    QString text;
    quint64 bytesReaded = 0;
    quint64 bytesAvailable = file.bytesAvailable();
    do {
        if (m_cancelMarker.testAndSetAcquire(true, true)) {
            emit s_readingFinished(RESULT_CANCELED);
            return;
        }

        try {
            QString text = textStream.readLine();
            bytesReaded += text.size();
            emit s_progressChanged(bytesReaded * 100 / bytesAvailable, 100);
        } catch (...) {
            emit s_readingFinished(RESULT_FAILED);
            return;
        }
    } while (!textStream.atEnd());

    if (file.error() != QFile::NoError) {
        emit s_readingFinished(RESULT_FAILED);
        return;
    }
    emit s_readingFinished(RESULT_OK);
}

void LogFileReaderTask::cancel()
{
    m_cancelMarker.fetchAndStoreAcquire(true);
}

void LogFileReaderTask::pause()
{
    m_stopMarker.fetchAndStoreAcquire(true);
}

void LogFileReaderTask::resume()
{
    m_stopMarker.fetchAndStoreAcquire(false);
}
