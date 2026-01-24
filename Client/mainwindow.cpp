#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socket = new QTcpSocket(this);

    ui->Btn_disconnect->setEnabled(false);
    ui->Le_ip->setPlaceholderText("127.0.0.1");
    ui->Le_port->setPlaceholderText("8888");
    ui->L_status->setStyleSheet("color: red");

    connect(ui->Btn_connect, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(ui->Btn_disconnect, &QPushButton::clicked, this, &MainWindow::disconnectFromServer);
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
