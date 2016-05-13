#include "dataimageconfigdialog.h"
#include "ui_dataimageconfigdialog.h"
#include "constant.h"

DataImageConfigDialog::DataImageConfigDialog(uint64_t freeBytesAvailable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataImageConfigDialog)
{
    ui->setupUi(this);

    if (freeBytesAvailable < (_32GB + REMIXOS_SYS_CAPACITY))
    {
        ui->radioButton2->setText(tr("16GB(Recommended)"));
        ui->radioButton2->setChecked(true);
        ui->radioButton3->setText(tr("32GB"));
        ui->radioButton3->setEnabled(false);
        ui->radioButton3->hide();
    }
    maxSize = 0;
}

DataImageConfigDialog::~DataImageConfigDialog()
{
    delete ui;
}

void DataImageConfigDialog::on_pushButton_clicked()
{
    if(ui->radioButton1->isChecked())
    {
        maxSize = _8GB;
    }
    else if(ui->radioButton2->isChecked())
    {
        maxSize = _16GB;
    }
    else if(ui->radioButton3->isChecked())
    {
        maxSize = _32GB;
    }
    else
    {
        return;
    }
    close();
}
