#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new QTcpServer(this);
    m_fileModel = new QFileSystemModel(this);

    QSettings settings("config.ini", QSettings::IniFormat); // It's not working, it needs to be fixed.
    ip = settings.value("Network/Address", "127.0.0.1").toString();
    port = settings.value("Network/Port", 8888).toInt();

    QString path = settings.value("Storage/Path", "./server_files").toString();

    QString m_currentPath = path;

    QDir dir(m_currentPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    m_fileModel->setRootPath(path);
    ui->Tv_listFile->setModel(m_fileModel);
    ui->Tv_listFile->setRootIndex(m_fileModel->index(path));

    ui->Btn_stop->setEnabled(false);
    ui->L_status->setStyleSheet("color: red");

    ui->L_ip->setText(ip);
    ui->L_port->setText(QString::number(port));
    ui->L_dir->setText(path);


    connect(ui->Btn_start, &QPushButton::clicked, this, &MainWindow::startServer);
    connect(ui->Btn_stop, &QPushButton::clicked, this, &MainWindow::stopServer);
    // connect(ui->Btn_update, &QPushButton::clicked, this, &MainWindow::updateFileList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startServer()
{
    if(m_server->listen(QHostAddress(ip), port)) {
        ui->Btn_stop->setEnabled(true);
        ui->Btn_start->setEnabled(false);
        ui->Te_logServer->append("Server started.");

        ui->L_status->setText("Online");
        ui->L_status->setStyleSheet("color: green");

        connect(m_server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
    } else {
        ui->L_status->setText("Failed " + m_server->errorString());
    }
}

void MainWindow::stopServer()
{
    if(m_server->isListening()){
        m_server->close();
        ui->Te_logServer->append("Server stopped.");

        ui->Btn_stop->setEnabled(false);
        ui->Btn_start->setEnabled(true);

        ui->L_status->setText("Offline");
        ui->L_status->setStyleSheet("color: red");

        for(QTcpSocket *socket : m_clients) {
            socket->disconnectFromHost();
        }
    }
}

void MainWindow::newConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();

        m_clients.append(socket);

        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnect);

        QString clientIp = socket->peerAddress().toString();
        ui->Te_logServer->append("New client connected: " + clientIp);

        ui->L_user->setText(QString::number(m_clients.size()));
    }
}

void MainWindow::clientDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();

        ui->Te_logServer->append("Client disconnected.");
        ui->L_user->setText(QString::number(m_clients.size()));
    }
}

// void MainWindow::updateFileList()
// {
//     ui->Tv_listFile->update();
// }
