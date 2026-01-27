#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>

#include <QStorageInfo>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>

#include <QFileSystemModel>
#include <QSettings>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

private slots:
    void startServer();
    void stopServer();

    void uploadFile();
    void deleteFile();
    void updateDiskSpace();

    void newConnection();
    void clientDisconnect();

private:
    Ui::MainWindow *ui;
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    QFileSystemModel *m_fileModel;

    void createSettingsFile();
    void sendFileList(QTcpSocket *socket);

};
#endif // MAINWINDOW_H
