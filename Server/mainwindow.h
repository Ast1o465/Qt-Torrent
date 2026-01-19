#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include <QSettings>
#include <QDir>

#include <QTcpServer>
#include <QTcpSocket>

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

private:
    Ui::MainWindow *ui;
    QTcpServer *m_server;
};
#endif // MAINWINDOW_H
