#ifndef STATUSVIEWLOGFILELOADER_H
#define STATUSVIEWLOGFILELOADER_H

#include <QWidget>

namespace Ui {
class StatusViewLogFileLoader;
}

class StatusViewLogFileLoader : public QWidget
{
    Q_OBJECT

public:
    explicit StatusViewLogFileLoader(const QString& fileName, QWidget *parent = 0);
    ~StatusViewLogFileLoader();

public slots:
    void updateProgressValue(int newValue);

signals:
    void s_cancelLoader();

private:
    Ui::StatusViewLogFileLoader *ui;
};

#endif // STATUSVIEWLOGFILELOADER_H
