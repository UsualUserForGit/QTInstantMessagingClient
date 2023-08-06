#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customtextedit.h"

#include <QMainWindow>
#include <QDebug>
#include <QShortcut>
#include <QHostAddress>
#include <QPaintEvent>
#include <QPainter>

#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


// More Explanatory comments will be inside the .cpp file
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setSocket(QTcpSocket *socket);
    QTcpSocket *getSocket();
    void setLogin(QString login);
    QString getLogin();

    ~MainWindow();

private:
    enum class JsonFileType {Message, LoadChannelMessages};

    Ui::MainWindow* ui;
    bool connectedToHost;
    QTcpSocket* socket;
    CustomTextEdit* message_input_text_edit;
    QTabBar *tabbar;
    QDateTime lastMessageSentTime;
    QString lastMessageSender;
    QString login;

    QJsonObject createJsonObject(JsonFileType jsonFile, int index = -1);
    void processServerResponse(QJsonObject jsonObject);
    void sendJsonToServer(const QJsonObject& jsonObject);

    void printMessage(QString text, QString name, QDateTime time);

private slots:

    void on_sendButton_clicked();
    void on_enter_ctrl_pressed();
    void socketDisconnected();
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketReadyRead();
    void tabBar_clicked(int index);
};

#endif // MAINWINDOW_H
