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
            while (!textStream.atEnd() || m_exampleLogData.size() < 3) {
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

    }
}

QMap<QString, LogLineParser::TokenDescription> LogFileDialog::getTokenDescription() const
{
    QMap <QString, LogLineParser::TokenDescription> mapTokenDesciption;
    mapTokenDesciption.insert(ui->integerLabel, LogLineParser::TokenDescription(ui->integerRegExp, "", QVariant::LongLong));
    mapTokenDesciption.insert(ui->floatLabel, LogLineParser::TokenDescription(ui->floatRegExp, "", QVariant::Double));
    mapTokenDesciption.insert(ui->stringLabel, LogLineParser::TokenDescription(ui->stringRegExp, "", QVariant::String));
    mapTokenDesciption.insert(ui->timeLabel, LogLineParser::TokenDescription(ui->timeRegExp, ui->timeFormat, QVariant::Time));
    mapTokenDesciption.insert(ui->dateLabel, LogLineParser::TokenDescription(ui->dateRegExp, ui->dateFormat, QVariant::Date));
    mapTokenDesciption.insert(ui->datetimeLabel, LogLineParser::TokenDescription(ui->datetimeRegExp, ui->datetimeFormat, QVariant::DateTime));
    return mapTokenDesciption;
}
