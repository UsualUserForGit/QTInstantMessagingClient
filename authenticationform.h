#ifndef AUTHENTICATIONFORM_H
#define AUTHENTICATIONFORM_H

#include "animatedcustompushbutton.h"
#include "mainwindow.h"

#include <QWidget>
#include <QPaintEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>


namespace Ui {
class AuthenticationForm;
}

class AuthenticationForm : public QWidget
{
    Q_OBJECT

public:
    explicit AuthenticationForm(QWidget *parent = nullptr);
    ~AuthenticationForm();

private:
    Ui::AuthenticationForm *ui;
    MainWindow w;
    QTcpSocket* socket;


    enum class JsonFileType {SignInData, RegisterUser, SignInResults, RegisterUserResults};

    QJsonObject createJsonObject(JsonFileType jsonFile);
    void sendJsonToServer(const QJsonObject& jsonObject);
    void handleServerResponse(QJsonObject jsonObject);

protected:
    AnimatedCustomPushButton *login_Button;

    void paintEvent(QPaintEvent *event) override;

private slots:
    void login_Button_clicked();

    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();

};

#endif // AUTHENTICATIONFORM_H
