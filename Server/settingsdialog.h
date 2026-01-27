#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <QCoreApplication>


namespace Ui {
class settingsdialog;
}

class settingsdialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsdialog(QWidget *parent = nullptr);
    ~settingsdialog();

private:
    Ui::settingsdialog *ui;

private slots:
    void saveAndClose();
    void selectFolder();
};

#endif // SETTINGSDIALOG_H
