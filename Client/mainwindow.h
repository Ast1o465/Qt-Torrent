#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include <QSettings>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

#include <QTcpSocket>
#include <QProgressBar>

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
    void connectToServer();
    void disconnectFromServer();

private:
    Ui::MainWindow *ui;
    QTcpSocket *m_socket;
    QFile *m_file;
    qint64 m_fileSize;
    qint64 m_bytesReceived;
    bool m_isDownloading;

    void retrieveDataFromServer();
    void sendFileRequest(QString fileName);
};
#endif // MAINWINDOW_H
