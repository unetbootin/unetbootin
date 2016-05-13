#ifndef DATAIMAGECONFIGDIALOG_H
#define DATAIMAGECONFIGDIALOG_H

#include <QDialog>
#include <stdint.h>

namespace Ui {
class DataImageConfigDialog;
}

class DataImageConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataImageConfigDialog(uint64_t freeBytesAvailable, QWidget *parent = 0);
    ~DataImageConfigDialog();
    uint64_t maxSize;

private slots:
    void on_pushButton_clicked();

private:
    Ui::DataImageConfigDialog *ui;
};

#endif // DATAIMAGECONFIGDIALOG_H
