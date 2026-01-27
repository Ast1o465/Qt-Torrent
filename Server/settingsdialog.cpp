#include "settingsdialog.h"
#include "ui_settingsdialog.h"

settingsdialog::settingsdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingsdialog)
{
    ui->setupUi(this);

    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    ui->Sc_Le_IP->setText(settings.value("Network/Address", "127.0.0.1").toString());
    ui->Sc_Le_port->setText(settings.value("Network/Port", "8888").toString());
    ui->SC_Le_dir->setText(settings.value("Storage/Path", "./server_files").toString());

    // connect(ui->Btn_cancel, &QPushButton::clicked, this, &settingsdialog::saveAndClose);
    // connect(ui->Tb_dir, &QToolButton::clicked, this, &settingsdialog::selectFolder);
}

settingsdialog::~settingsdialog()
{
    delete ui;
}

void settingsdialog::saveAndClose()
{

}

void settingsdialog::selectFolder()
{

}
