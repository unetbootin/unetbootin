#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QThread>

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(QString cmd, QString paras, QString info, QWidget *parent = 0);
    ~WaitingDialog();
    int returnCode;
    QString command;
    QString parameters;

private:
    Ui::WaitingDialog *ui;
};

class WorkerThread : public QThread
{
    Q_OBJECT
    void run();
signals:
    void resultReady(const QString &s);
public:
    WorkerThread(WaitingDialog* dlg);
    WaitingDialog* waitingDialog;
};

#endif // WAITINGDIALOG_H
