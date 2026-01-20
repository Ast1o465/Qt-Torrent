#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

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
    void newConnection();
    void clientDisconect();

private:
    Ui::MainWindow *ui;
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;

};
#endif // MAINWINDOW_H
