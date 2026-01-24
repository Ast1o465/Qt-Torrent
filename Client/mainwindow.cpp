#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socket = new QTcpSocket(this);

    ui->Le_ip->setPlaceholderText("127.0.0.1");
    ui->Le_port->setPlaceholderText("8888");
    ui->L_status->setStyleSheet("color: red");

    connect(ui->Btn_connect, &QPushButton::clicked, this, &MainWindow::connectToServer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToServer()
{
    if (!m_socket) {
        m_socket = new QTcpSocket(this);
    }

    QString ip = ui->Le_ip->text();
    int port = ui->Le_port->text().toInt();

    m_socket->connectToHost(ip, port);
    if (m_socket->waitForConnected()) {
        ui->L_status->setStyleSheet("color: green");
        ui->L_status->setText("Connected to server.");
    } else {
        ui->L_status->setStyleSheet("color: red");
        ui->L_status->setText("Failed to connect to server.");
    }
}
