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
        QByteArray data;

        reader >> type;
        reader >> data;

        if (!reader.commitTransaction()) {
            return;
        }

        if (type == 0x01) {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray array = doc.array();

            ui->Tw_main->setRowCount(0);

            for (const QJsonValue &value : array) {
                QJsonObject obj = value.toObject();

                QString name = obj["name"].toString();
                qint64 size = obj["size"].toVariant().toLongLong();

                int row = ui->Tw_main->rowCount();
                ui->Tw_main->insertRow(row);

                // Column 0
                ui->Tw_main->setItem(row, 0, new QTableWidgetItem(name));

                // Column 1
                double sizeMB = size / 1024.0 / 1024.0;
                QString sizeStr = QString::number(sizeMB, 'f', 2) + " MB";
                ui->Tw_main->setItem(row, 1, new QTableWidgetItem(sizeStr));

                // Column 2
                QProgressBar *bar = new QProgressBar();
                bar->setValue(0);
                ui->Tw_main->setCellWidget(row, 2, bar);
            }

            ui->Te_log->append("Data received: " + QString::number(array.size()));
        }
    }
}

void MainWindow::sendFileRequest(QString fileName)
{

}
