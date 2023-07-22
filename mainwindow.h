#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customtextedit.h"

#include <QMainWindow>
#include <QDebug>
#include <QShortcut>

#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    MainWindow(QTcpSocket *socket, QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();

    void on_sendButton_clicked();
    void on_enter_ctrl_pressed();
private:
    Ui::MainWindow* ui;
    bool connectedToHost;
    QTcpSocket* socket;
    CustomTextEdit* message_input_text_edit;

    void printMessage(QString message);
    void sendJsonToServer(const QJsonObject& jsonObject);
};

#endif // MAINWINDOW_H
