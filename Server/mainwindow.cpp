#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("config.ini", QSettings::IniFormat); // It's not working, it needs to be fixed.

    ip = settings.value("Network/Address", "127.0.0.1").toString();
    port = settings.value("Network/Port", 8888).toInt();

    QString path = settings.value("Storage/Path", "./server_files").toString();

    ui->L_ip->setText(ip);
    ui->L_port->setText(QString::number(port));

    ui->L_dir->setText(path);

    m_server = new QTcpServer(this);

    connect(ui->Btn_start, &QPushButton::clicked, this, &MainWindow::startServer);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_server;
}

void MainWindow::startServer()
{
    if(m_server->listen(QHostAddress(ip), port)) {
        ui->Btn_start->setEnabled(false);
        ui->L_status->setText("Online");
    } else {
        ui->L_status->setText("Failed " + m_server->errorString());
    }
}
