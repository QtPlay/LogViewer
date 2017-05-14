#include "logfiledialog.h"
#include "ui_logfiledialog.h"
#include "mainwindow.h"

LogFileDialog::LogFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogFileDialog)
{
    ui->setupUi(this);
    connect(ui->chooseLogFile, SIGNAL(released()), this, SLOT(openFile()));
    connect(ui->check, SIGNAL(released()), this, SLOT(checkLogData()));
    connect(ui->clearHistory, SIGNAL(released()), this, SLOT(clearHistory()));

    m_logLineParser = new LogLineParser();
}

LogFileDialog::~LogFileDialog()
{
    delete ui;
}

void LogFileDialog::openFile()
{
    m_exampleLogData.clear();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose log file"), tr(""), tr("Log (*.log)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream textStream(&file);
            while (!textStream.atEnd() && m_exampleLogData.size() < 3) {
                QString text;
                text =  textStream.readLine();
                ui->exampleLogData->appendPlainText(text);
                m_exampleLogData << text;
            }
            file.close();
            if (m_exampleLogData.size() > 0) {
                ui->fileName->setText(fileName);
            }
        } else {
            MainWindow::showMessage(QMessageBox::Warning, "Open log file", file.errorString());
        }
    }
}

void LogFileDialog::checkLogData()
{
    if (ui->formatData->text().isEmpty()) {
        MainWindow::showMessage(QMessageBox::Warning, "Check log data", "Enter log file format line");
    } else {
        QString inputFormat = ui->formatData->text();
        if ((inputFormat.contains("%t") && ui->timeFormat->text().isEmpty()) ||
                (inputFormat.contains("%d") && ui->dateFormat->text().isEmpty()) ||
                (inputFormat.contains("%l") && ui->datetimeFormat->text().isEmpty())) {
            MainWindow::showMessage(QMessageBox::Warning, "Empty format", "Fill format field");
        } else {
            m_logLineParser->setTokenDescription(getTokenDescription());
            m_logLineParser->setLineFormat(inputFormat);
            QList<LogLineParser::TokenInfo> tokenInfo = m_logLineParser->tokenInfo();
            ui->regExpFormat->setText(m_logLineParser->regExpFormat());
            ui->history->addItem(ui->formatData->text());
            for (auto it = m_exampleLogData.cbegin(); it != m_exampleLogData.cend(); ++it) {
                QStringList parsedData = m_logLineParser->parseLine((*it));
                QString formatMessage = QString("%1: %2 -> %3");
                if (m_logLineParser->error().errorCode == LogLineParser::NO_ERROR) {
                    QString text;
                    for (int i = 0; i < parsedData.size(); ++i) {
                        text.append(QString("(%1, %2, %3)").arg(parsedData.at(i)).arg(tokenInfo.at(i).token).arg(QVariant(tokenInfo.at(i).type).typeName()));
                        if (i + 1 != parsedData.size()) text.append(", ");
                    }
                    QString outputMessage = formatMessage.arg("OK").arg((*it)).arg(text);
                    ui->result->appendPlainText(outputMessage);
                } else {
                    QString outputMessage = formatMessage.arg("BAD").arg((*it)).arg(QString(parsedData.join(", "))) + "|" + m_logLineParser->error().errorText;
                    ui->result->appendPlainText(outputMessage);
                }
            }
        }
    }
}

void LogFileDialog::clearHistory()
{
    ui->history->clear();
}

QMap<QString, LogLineParser::TokenDescription> LogFileDialog::getTokenDescription() const
{
    QMap <QString, LogLineParser::TokenDescription> mapTokenDesciption;
    mapTokenDesciption.insert(ui->integerLabel->text(), LogLineParser::TokenDescription(ui->integerRegExp->text(), "", QVariant::LongLong));
    mapTokenDesciption.insert(ui->floatLabel->text(), LogLineParser::TokenDescription(ui->floatRegExp->text(), "", QVariant::Double));
    mapTokenDesciption.insert(ui->stringLabel->text(), LogLineParser::TokenDescription(ui->stringRegExp->text(), "", QVariant::String));
    mapTokenDesciption.insert(ui->timeLabel->text(), LogLineParser::TokenDescription(ui->timeRegExp->text(), ui->timeFormat->text(), QVariant::Time));
    mapTokenDesciption.insert(ui->dateLabel->text(), LogLineParser::TokenDescription(ui->dateRegExp->text(), ui->dateFormat->text(), QVariant::Date));
    mapTokenDesciption.insert(ui->datetimeLabel->text(), LogLineParser::TokenDescription(ui->datetimeRegExp->text(), ui->datetimeFormat->text(), QVariant::DateTime));
    return mapTokenDesciption;
}
