#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionSortFilter, SIGNAL(triggered(bool)), this, SLOT(addSortFilter()));
    connect(ui->actionHighlightFilter, SIGNAL(triggered(bool)), this, SLOT(addHighlightFilter()));
    connect(ui->actionOpenProject, SIGNAL(triggered(bool)), this, SLOT(openProject()));
    connect(ui->actionOpenLogFile, SIGNAL(triggered(bool)), this, SLOT(openLogFile()));

    m_logFileDialog = new LogFileDialog();

    m_logDataView = new LogDataView();
    m_logDataView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_descDbTreeView = new DescDbTreeView();
    m_descDbTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_treeItemDataView = new TreeItemDataView();
    m_treeItemDataView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_treeItemDataView->setMaximumHeight(300);

    connect(&DatabaseManager::getInstance(), SIGNAL(s_addDatabase(QString)), m_descDbTreeView, SLOT(newProject(QString)));
    connect(m_descDbTreeView, SIGNAL(s_closeProject(QString)), &DatabaseManager::getInstance(), SLOT(closeDatabase(QString)));
    connect(m_descDbTreeView, SIGNAL(s_clickedProject(QString)), m_treeItemDataView, SLOT(changeProject(QString)));
    connect(m_descDbTreeView, SIGNAL(s_clickedSortFilter(QString,QString)), m_treeItemDataView, SLOT(changeSortFilter(QString,QString)));
    connect(m_descDbTreeView, SIGNAL(s_clickedHighlightFilter(QString,QString)), m_treeItemDataView, SLOT(changeHighlightFilter(QString,QString)));
    connect(m_descDbTreeView, SIGNAL(s_activeProjectChanged(QString)), m_logDataView, SLOT(changeProject(QString)));
    connect(m_descDbTreeView, SIGNAL(s_activeSortFilterChanged(QString,QString)), m_logDataView, SLOT(changeSortFilter(QString,QString)));
    connect(m_descDbTreeView, SIGNAL(s_activeHighlightFilterChanged(QString,QString)), m_logDataView, SLOT(changeHighlightFilter(QString,QString)));

    m_filterDialog = new FilterDialog();
    connect(m_filterDialog, SIGNAL(s_addedFilter(QString, QString,QList<DescriptionFilter>,QList<DescriptionHighlighFilter>)),
            &DatabaseManager::getInstance(), SLOT(addFilter(QString,QString,QList<DescriptionFilter>,QList<DescriptionHighlighFilter>)));
    connect(m_filterDialog, SIGNAL(s_changedFilter(QString, QString,QList<DescriptionFilter>,QList<DescriptionHighlighFilter>)),
            &DatabaseManager::getInstance(), SLOT(changeFilter(QString,QString,QList<DescriptionFilter>,QList<DescriptionHighlighFilter>)));
    connect(m_descDbTreeView, SIGNAL(s_addFilter(QString, Filter::Type)), m_filterDialog, SLOT(addFilter(QString, Filter::Type)));
    connect(m_descDbTreeView, SIGNAL(s_changeFilter(QString,QString,Filter::Type)), m_filterDialog, SLOT(changeFilter(QString,QString,Filter::Type)));

    QVBoxLayout *containerLeftPanelLayout = new QVBoxLayout();
    containerLeftPanelLayout->setMargin(0);
    containerLeftPanelLayout->setSpacing(1);
    containerLeftPanelLayout->addWidget(m_descDbTreeView);
    containerLeftPanelLayout->addWidget(m_treeItemDataView);
    QWidget *containerLeftPanel = new QWidget();
    containerLeftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    containerLeftPanel->setLayout(containerLeftPanelLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(containerLeftPanel);
    splitter->addWidget(m_logDataView);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 3);

    ui->gridLayout->setMargin(0);
    ui->gridLayout->setSpacing(1);
    ui->mainLayout->setMargin(0);
    ui->mainLayout->setSpacing(1);
    ui->mainLayout->addWidget(splitter);

    DatabaseManager::getInstance().openDatabase("C:\\Users\\alexsandr\\Desktop\\test.db");
    DatabaseManager::getInstance().openDatabase("C:\\Users\\alexsandr\\Desktop\\test1.db");

    //    FilterDialog fileDialog;
    //    fileDialog.addFilter(QString("C:\\Users\\alexsandr\\Desktop\\test.db"), Filter::SORT);
    //    if (fileDialog.exec()) {

    //    }
    //return;

    //connect(ui->pb_openFile, SIGNAL(released()), this, SLOT(openFile()));

    //    QString str = "64.242.88.10 - - [07/Mar/2004:16:05:49 -0800] \"GET /twiki/bin/edit/Main/Double_bounce_sender?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401.45 12846";
    //    qDebug() << str;
    //    QString inputFormat = "%s %s %s [%d:%t %i] \"%s %s %s\" %f %i";
    //    QString outputFormat;

    //    QMap <QChar, QString> convertMap;
    //    convertMap.insert('s', "(.+)");
    //    convertMap.insert('i', "([-]?\\d+)");
    //    convertMap.insert('f', "([-]?\\d+\\.\\d+)");
    //    convertMap.insert('d', "(\\d+\\/.+\\/\\d+)");
    //    convertMap.insert('t', "(\\d+\\:\\d+\\:\\d+)");

    //    QMap <QChar, QVariant::Type> typeLexeme;
    //    typeLexeme.insert('s', QVariant::String);
    //    typeLexeme.insert('i', QVariant::Int);
    //    typeLexeme.insert('f', QVariant::Double);
    //    typeLexeme.insert('d', QVariant::Date);
    //    typeLexeme.insert('t', QVariant::Time);

    //    QList< QVariant::Type> listTypeLexeme;
    //    QChar lastSymbol;
    //    foreach (QChar currentSymbol, inputFormat) {
    //        if (!lastSymbol.isNull() && lastSymbol == '%' && convertMap.contains(currentSymbol)) {
    //            outputFormat.append(convertMap.value(currentSymbol));
    //            listTypeLexeme.append(typeLexeme.value(currentSymbol));
    //            lastSymbol = QChar();
    //            continue;
    //        }
    //        if (lastSymbol.isNull()) {
    //            lastSymbol = currentSymbol;
    //            continue;
    //        } else {
    //            switch (lastSymbol.toLatin1()) {
    //            case ' ':
    //                outputFormat.append("\\s+");
    //                break;
    //            default:
    //                outputFormat.append("\\").append(lastSymbol);
    //                break;
    //            }
    //            lastSymbol = currentSymbol;
    //        }
    //    }
    //    qDebug() << outputFormat;

    //    QRegExp rx = QRegExp(outputFormat);
    //    qDebug() << rx.exactMatch(str);
    //    rx.indexIn(str);
    //    QStringList qsl = rx.capturedTexts();
    //    for(int i=1; i<qsl.count(); i++)
    //    {
    //        qDebug()<< i << " Da Value Thing: " << qsl.at(i) << QVariant::typeToName(listTypeLexeme.at(i - 1));
    //    }

    //    QString inputFormat = "%s %s %s [%d:%t %i] \"%s %s %s\" %i %i";
    //    QFile file("C:\\Users\\alexsandr\\Desktop\\access_log_1.log");
    //    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //        QTextStream textStream(&file);
    //        LogLineParser lineParser(inputFormat);
    //        for (int i = 0; i < 4; ++i) {
    //            QString text = textStream.readLine();
    //            QStringList values;
    //            LogLineParser::ResultCode resultCode;
    //            std::tie(values, resultCode) = lineParser.parseLine(text);
    //            qDebug() << values << resultCode;
    //        }
    //        file.close();
    //    }

    //    DatabaseCreator *dbCreator = new DatabaseCreator();
    //    dbCreator->create("testDB", QList<QVariant::Type>() << QVariant::Int << QVariant::String << QVariant::Time
    //                      << QVariant::Date << QVariant::Double);

    //    qDebug() << QDir::current().absolutePath();
    //    qDebug() << QDir::currentPath();

    //QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("C:\\Users\\alexsandr\\Desktop\\test.db");
    //if (db.open()) {
    //        QSqlQuery query(db);
    //        if (!query.exec("select * from info where field2 regexp('86.98.12.*')")) {
    //            qDebug() << query.lastError().text();
    //        } else {
    //            int count = 0;
    //            while (query.next() && count < 5) {
    //                qDebug() << query.record().value(2);
    //                count++;
    //            }
    //        }
    //        return;

    //        QString textInsertQuery = QString("insert into info (field1, field2, field3, field4, field5) values %1");
    //        QString textValues = QString("(%1, '%2', %3, %4, %5)");
    //        QString resultValue;
    //        QSqlQuery insertQuery(db);
    //        int c = 5000;
    //        int count = 0;
    //        for (int i = 0; i < 100000; ++i) {
    //            QString textValuesComplete = textValues.arg(i).arg(QString("sasha") + QString::number(i)).arg(i + 0.4568f).arg(QDateTime::currentSecsSinceEpoch()).arg(QTime::currentTime().msecsSinceStartOfDay());
    //            //qDebug() << textCompleteQuery;
    //            resultValue += textValuesComplete;
    //            if (count == c || i + 1 == 100000) {
    //                if (!insertQuery.exec(textInsertQuery.arg(resultValue))) {
    //                    qDebug() << insertQuery.lastError().text();
    //                }
    //                //qDebug() << textInsertQuery.arg(resultValue);
    //                resultValue.clear();
    //                count = 0;
    //            } else {
    //                resultValue += ", ";
    //                count++;
    //            }
    //        }

    //        m_logDataView->setDatabase(db);
    //        m_logDataView->setDatabaseTable("info");
    //        m_logDataView->updateData();
    //        //ui->logDataView->hideColumn(0);

    //        QMap<QString, QVariant::Type> mapFieldTypes;
    //        mapFieldTypes["id"] = QVariant::Int;
    //        mapFieldTypes["field1"] = QVariant::Int;
    //        mapFieldTypes["field2"] = QVariant::String;
    //        mapFieldTypes["field3"] = QVariant::Double;
    //        mapFieldTypes["field4"] = QVariant::Date;
    //        mapFieldTypes["field5"] = QVariant::Time;
    //        m_logDataView->setInfoFieldTypes(mapFieldTypes);


    //        SortFilter sortFilter;

    //        DescriptionFilter descFilter;
    //        descFilter.columnName = "field2";
    //        descFilter.signRegExp = true;
    //        descFilter.relationSign = RelationSign::E;
    //        descFilter.filterValue = "^p.*";
    //        sortFilter.listDescriptionFilter.append(descFilter);

    //        DescriptionFilter descFilter2;
    //        descFilter2.columnName = "field1";
    //        descFilter2.signRegExp = false;
    //        descFilter2.relationSign = RelationSign::E;
    //        descFilter2.filterValue = "7";
    //        sortFilter.listDescriptionFilter.append(descFilter2);
    //        ui->logDataView->setSortFilter(sortFilter);

    //        HighlighFilter highlightFilter;
    //        DescriptionHighlighFilter f1;
    //        f1.color = QColor(Qt::red);
    //        f1.filterValue = "1";
    //        f1.relationSign = RelationSign::GE;
    //        highlightFilter.listDescriptionFilter.append(f1);

    //        DescriptionHighlighFilter f2;
    //        f2.color = QColor(Qt::red);
    //        f2.filterValue = "5";
    //        f2.relationSign = RelationSign::L;
    //        highlightFilter.listDescriptionFilter.append(f2);

    //        DescriptionHighlighFilter f3;
    //        f3.color = QColor(Qt::green);
    //        f3.filterValue = "4";
    //        f3.relationSign = RelationSign::E;
    //        highlightFilter.listDescriptionFilter.append(f3);

    //        ui->logDataView->setHighlightFilter(highlightFilter, "field1");

    //    }
    //    m_databaseDataView->updateTreeView();

    QString input = "64.242.88.10 - - [07/Mar/2004:16:10:02 -0800] \"GET /mailman/listinfo/hsdivision HTTP/1.1\" 200 6291";
    QString regExp = "(.+)\\s+(.+)\\s+(.+)\\s+\\\[(.+)\\s+([-]?\\d+)\]\\s+\"(.+)\\s+(.+)\\s+(.+)\\\"\\s+([-]?\\d+)\\s+([-]?\\d+)";
    qDebug() << regExp;
    QRegExp rx = QRegExp(regExp);
    qDebug() << "result = " << rx.exactMatch(input);

    QLocale locale(QLocale::English);
    qDebug() << locale.toDate("07/Mar/2004", "dd/MMM/yyyy");

    qDebug() << QDate(2004, 3, 7).toString("dd/MMM/yyyy");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openLogFile()
{
    if (m_logFileDialog->exec()) {}
}

void MainWindow::showMessage(QMessageBox::Icon iconType, const QString &title, const QString &content)
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(content);
    msg.setIcon(iconType);
    if (msg.exec()) {}
}

//void MainWindow::openFile()
//{
//    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose project file"), tr(""), tr("Project (*.db)"));
//    DatabaseManager::getInstance().openDatabase(fileName);
////    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose log file"), tr(""), tr("Log (*.log)"));
////    QFileInfo fileInfo(fileName);
////    if (!fileInfo.exists() || !fileInfo.isReadable()) return;

////    LogFileReaderTask *readTask = new LogFileReaderTask(fileName, "");
////    LogFileReaderWidget *readWidget = new LogFileReaderWidget(fileName);
////    connect(readTask, SIGNAL(s_progressChanged(int,int)), readWidget, SLOT(onProgress(int,int)));
////    connect(readTask, SIGNAL(s_readingFinished(int)), readWidget, SLOT(onReadingFinished(int)));
////    connect(readWidget, SIGNAL(cancelReading()), readTask, SLOT(cancel()));
////    //connect(readTask, SIGNAL(s_readingData(QString)), this, SLOT(addMessage(QString)));

////    //ui->l_saContent_logFileReaderWidget->addWidget(readWidget);
//    //    QThreadPool::globalInstance()->start(readTask);
//}

void MainWindow::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose project file"), tr(""), tr("Project (*.db)"));
    DatabaseManager::getInstance().openDatabase(fileName);
}

void MainWindow::addSortFilter()
{
    QString activeProject = m_descDbTreeView->getActiveProject();
    if (activeProject.isEmpty()) {
        MainWindow::showMessage(QMessageBox::Information, "Unknow project", "Choose project.");
    } else {
        m_filterDialog->addFilter(activeProject, Filter::SORT);
        if (m_filterDialog->exec()){}
    }
}

void MainWindow::addHighlightFilter()
{
    QString activeProject = m_descDbTreeView->getActiveProject();
    if (activeProject.isEmpty()) {
        MainWindow::showMessage(QMessageBox::Information, "Unknow project", "Choose project.");
    } else {
        m_filterDialog->addFilter(activeProject, Filter::HIGHLIGHT);
        if (m_filterDialog->exec()) {};
    }
}
