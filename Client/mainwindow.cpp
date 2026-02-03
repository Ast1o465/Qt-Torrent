#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socket = new QTcpSocket(this);

    ui->Tw_main->setColumnCount(3);
    ui->Tw_main->setHorizontalHeaderLabels({"Title", "Size", "Status"});
    ui->Tw_main->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    ui->Btn_disconnect->setEnabled(false);
    ui->Le_ip->setPlaceholderText("127.0.0.1");
    ui->Le_port->setPlaceholderText("8888");

    ui->Le_port->setText("8888");
    ui->Le_ip->setText("127.0.0.1");

    ui->L_status->setStyleSheet("color: red");

    connect(ui->Btn_connect, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(ui->Btn_disconnect, &QPushButton::clicked, this, &MainWindow::disconnectFromServer);

    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::retrieveDataFromServer);

    connect(ui->Tw_main, &QTableWidget::cellDoubleClicked, [this](int r, int){ sendFileRequest(ui->Tw_main->item(r, 0)->text()); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToServer()
{
    QString ip = ui->Le_ip->text();
    int port = ui->Le_port->text().toInt();

    m_socket->connectToHost(ip, port);
    if (m_socket->waitForConnected()) {
        ui->Btn_connect->setEnabled(false);
        ui->Btn_disconnect->setEnabled(true);

        ui->L_status->setStyleSheet("color: green");
        ui->L_status->setText("Connected to server.");
    } else {
        ui->L_status->setStyleSheet("color: red");
        ui->L_status->setText("Failed to connect to server.");
    }
}

void MainWindow::disconnectFromServer()
{
    ui->L_status->setText("You are disconnected");
    ui->L_status->setStyleSheet("color: orange");

    ui->Btn_connect->setEnabled(true);
    ui->Btn_disconnect->setEnabled(false);
    m_socket->disconnectFromHost();
}

void MainWindow::retrieveDataFromServer()
{
    QDataStream reader(m_socket);
    reader.setVersion(QDataStream::Qt_5_15);

    while (true) {
        reader.startTransaction();

        quint16 type;
        reader >> type;

        if (type == 0x01) {
            QByteArray data;
            reader >> data;

            if (!reader.commitTransaction()) return;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray array = doc.array();

            ui->Tw_main->setRowCount(0);
            for (const QJsonValue &value : array) {
                QJsonObject obj = value.toObject();
                QString name = obj["name"].toString();
                qint64 size = obj["size"].toVariant().toLongLong();

                int row = ui->Tw_main->rowCount();
                ui->Tw_main->insertRow(row);
                ui->Tw_main->setItem(row, 0, new QTableWidgetItem(name));

                double sizeMB = size / 1024.0 / 1024.0;
                ui->Tw_main->setItem(row, 1, new QTableWidgetItem(QString::number(sizeMB, 'f', 2) + " MB"));

                QProgressBar *bar = new QProgressBar();
                bar->setValue(0);
                ui->Tw_main->setCellWidget(row, 2, bar);
            }
            ui->Te_log->append("The list of files has been updated.");

        }
        else if (type == 0x03) {
            qint64 fileSize;
            QString fileName;
            reader >> fileSize >> fileName;

            if (!reader.commitTransaction()) return;

            QString savePath = QSettings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat)
                                   .value("Storage/Path", ".").toString();
            QDir().mkpath(savePath);

            m_file = new QFile(savePath + "/" + fileName);
            if (m_file->open(QIODevice::WriteOnly)) {
                m_isDownloading = true;
                m_fileSize = fileSize;
                m_bytesReceived = 0;
                ui->Te_log->append("Started downloading: " + fileName);
            } else {
                ui->Te_log->append("Error: I can't create the file!");
                delete m_file;
                m_file = nullptr;
            }

        }
        else if (type == 0x04) {
            QByteArray chunk;
            reader >> chunk;

            if (!reader.commitTransaction()) return;

            if (m_isDownloading && m_file) {
                m_file->write(chunk);
                m_bytesReceived += chunk.size();

                // Progress bar updates

                if (m_bytesReceived >= m_fileSize) {
                    m_file->close();
                    delete m_file;
                    m_file = nullptr;
                    m_isDownloading = false;
                    ui->Te_log->append("File downloaded successfully!");
                }
            }
        }
        else {
            // Невідомий тип пакету? Просто виходимо
            if (!reader.commitTransaction()) return;
        }
    }
}

void MainWindow::sendFileRequest(QString fileName)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << (quint16)0x02 << fileName;

    m_socket->write(packet);
    ui->Te_log->append("File request: " + fileName);
}
