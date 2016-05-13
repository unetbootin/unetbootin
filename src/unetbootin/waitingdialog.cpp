#include "waitingdialog.h"
#include "ui_waitingdialog.h"
#include "unetbootin.h"

WaitingDialog::WaitingDialog(QString cmd, QString paras, QString info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitingDialog),
    returnCode(-1),
    command(cmd),
    parameters(paras)
{
    ui->setupUi(this);
    ui->infolabel->setText(info);

    WorkerThread *workerThread = new WorkerThread(this);
    connect(workerThread, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

WorkerThread::WorkerThread(WaitingDialog* dlg)
{
    waitingDialog = dlg;
}

void WorkerThread::run()
{
    QString result;
    /* expensive or blocking operation  */
    waitingDialog->returnCode = unetbootin::callexternapp(waitingDialog->command, waitingDialog->parameters).toInt();
    waitingDialog->close();
    emit resultReady(result);
}
