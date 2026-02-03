#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new QTcpServer(this);
    m_fileModel = new QFileSystemModel(this);
    createSettingsFile();

    m_fileModel->setRootPath(ui->L_dir->text());
    ui->Tv_listFile->setModel(m_fileModel);
    ui->Tv_listFile->setRootIndex(m_fileModel->index(ui->L_dir->text()));
    ui->Tv_listFile->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->Tv_listFile->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->Btn_stop->setEnabled(false);
    ui->L_status->setStyleSheet("color: red");

    connect(ui->Btn_start, &QPushButton::clicked, this, &MainWindow::startServer);
    connect(ui->Btn_stop, &QPushButton::clicked, this, &MainWindow::stopServer);

    connect(ui->Btn_UploadFile, &QPushButton::clicked, this, &MainWindow::uploadFile);
    connect(ui->Btn_DeleteFile,&QPushButton::clicked, this, &MainWindow::deleteFile);
    connect(ui->Btn_settings, &QPushButton::clicked, this, &MainWindow::openSettings);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startServer()
{
    if(m_server->listen(QHostAddress(ui->L_ip->text()), ui->L_port->text().toInt())) {
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

void MainWindow::uploadFile()
{
    QString sourcePath = QFileDialog::getOpenFileName(this, "Select a file to upload");

    if (sourcePath.isEmpty())return;

    QString serverPath = ui->L_dir->text();
    QString fileName = QFileInfo(sourcePath).fileName();
    QString destinationPath = serverPath + "/" + fileName;

    if (QFile::exists(destinationPath)) {
        QMessageBox::warning(this, "Error", "A file with this name already exists on the server!");
        return;
    }

    if (QFile::copy(sourcePath, destinationPath)) {
        ui->Te_logServer->append("File added: " + fileName);
    } else {
        ui->Te_logServer->append("File copy error! ");
        QMessageBox::critical(this, "Error", "Failed to copy file.");
    }
}

void MainWindow::deleteFile()
{
    QModelIndex index = ui->Tv_listFile->currentIndex();
    auto reply = QMessageBox::question(this, "delete", "Delete this file?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_fileModel->remove(index);
    }
}

void MainWindow::updateDiskSpace()
{

}

void MainWindow::openSettings()
{
    settingsdialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
        QSettings settings(configPath, QSettings::IniFormat);

        ui->L_ip->setText(settings.value("Network/Address").toString());
        ui->L_port->setText(settings.value("Network/Port").toString());
        ui->Te_logServer->append("Settings successfully updated!");
    } else {
        ui->Te_logServer->append("Settings change canceled.");
    }
}


void MainWindow::newConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();

        m_clients.append(socket);

        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnect);
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readsRequests);

        QString clientIp = socket->peerAddress().toString();
        ui->Te_logServer->append("New client connected: " + clientIp);

        ui->L_user->setText(QString::number(m_clients.size()));
        sendFileList(socket);
    }
}

void MainWindow::readsRequests()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);

    while (true) {
        in.startTransaction();

        quint16 type;
        QString fileName;

        in >> type >> fileName;

        if (!in.commitTransaction()) break;

        if (type == 0x02) {
            ui->Te_logServer->append("The client wants the file: " + fileName);
            sendFileContent(socket, fileName);
        }
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

void MainWindow::createSettingsFile()
{
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    if (!QFile::exists(configPath)) {
        settings.beginGroup("Network");
        settings.setValue("Address", "127.0.0.1");
        settings.setValue("Port", 8888);
        settings.endGroup();

        settings.beginGroup("Storage");
        settings.setValue("Path", "./server_files");
        settings.endGroup();

        settings.sync();
    }

    settings.beginGroup("Network");
    QString ip = settings.value("Address", "127.0.0.1").toString();
    int port = settings.value("Port", 8888).toInt();
    settings.endGroup();

    settings.beginGroup("Storage");
    QString path = settings.value("Path", "./server_files").toString();
    settings.endGroup();

    ui->L_ip->setText(ip);
    ui->L_port->setText(QString::number(port));
    ui->L_dir->setText(path);

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

void MainWindow::sendFileContent(QTcpSocket *socket, QString fileName)
{
    QFile file(ui->L_dir->text() + "/" + fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << (quint16)0x03 << file.size() << fileName;
    socket->write(packet);

    while (!file.atEnd()) {
        packet.clear(); out.device()->seek(0);
        out << (quint16)0x04 << file.read(64 * 1024);
        socket->write(packet);

        socket->waitForBytesWritten(10);
    }
}

void MainWindow::sendFileList(QTcpSocket *socket)
{
    if (!socket) return;

    QJsonArray fileArray;
    QDir dir(ui->L_dir->text());
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &info : list) {
        fileArray.append(QJsonObject{
            {"name", info.fileName()},
            {"size", info.size()}
        });
    }

    QJsonDocument doc(fileArray);
    QByteArray jsonData =doc.toJson();

    //Package formation
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << (quint16)0x01;
    out << jsonData;

    socket->write(packet);
    ui->Te_logServer->append("Sent file list to client. ");
 }
