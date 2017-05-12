#include "logfilereaderwidget.h"
#include "ui_logfilereaderwidget.h"

LogFileReaderWidget::LogFileReaderWidget(const QString &fileName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogFileReaderWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->pageProgressBar);
    ui->l_fileName->setText(fileName);
}

LogFileReaderWidget::~LogFileReaderWidget()
{
    delete ui;
    qDebug() << Q_FUNC_INFO;
}

void LogFileReaderWidget::onProgress(int doneCount, int sumCount)
{
    ui->progressBar->setMaximum(sumCount);
    ui->progressBar->setValue(doneCount);
}

void LogFileReaderWidget::onReadingFinished(int resultCode)
{
    ui->stackedWidget->setCurrentWidget(ui->pageResultLabel);
    switch (resultCode) {
    case LogFileReaderTask::RESULT_OK:
        ui->l_resultReading->setText("File reading success.");
        break;
    case LogFileReaderTask::RESULT_FAILED:
        ui->l_resultReading->setText("File reading failed.");
        break;
    case LogFileReaderTask::RESULT_CANCELED:
        ui->l_resultReading->setText("File reading canceled.");
        break;
    case LogFileReaderTask::RESULT_BAD_FORMAT:
        ui->l_resultReading->setText("File bad format.");
        break;
    default:
        break;
    }
}
