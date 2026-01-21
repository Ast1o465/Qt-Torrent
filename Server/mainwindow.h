#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>

#include <QFileDialog>
#include <QFileInfo>
#include <QFile>

#include <QFileSystemModel>
#include <QSettings>
#include <QDir>

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int port;
    QString ip;

private slots:
    void startServer();
    void stopServer();

    void uploadFile();
    void deleteFile();

    void newConnection();
    void clientDisconnect();

private:
    Ui::MainWindow *ui;
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    QFileSystemModel *m_fileModel;

};
#endif // MAINWINDOW_H
