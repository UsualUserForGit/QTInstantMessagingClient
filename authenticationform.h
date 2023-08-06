#ifndef AUTHENTICATIONFORM_H
#define AUTHENTICATIONFORM_H

#include "animatedcustompushbutton.h"
#include "mainwindow.h"

#include <QWidget>
#include <QPaintEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>


// More Explanatory comments will be inside the .cpp file
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
    enum class JsonFileType {SignInData, RegisterUser, SignInResults, RegisterUserResults};

    QJsonObject createJsonObject(JsonFileType jsonFile);
    void sendJsonToServer(const QJsonObject& jsonObject);
    void processServerResponse(QJsonObject jsonObject);

    Ui::AuthenticationForm *ui;
    QTcpSocket* socket;
    MainWindow mainMessengerWindow;
    AnimatedCustomPushButton *login_Button;
    AnimatedCustomPushButton *register_Button;
    bool connectedToHost;

protected:


    void paintEvent(QPaintEvent *event) override;

private slots:
    void login_Button_clicked();
    void register_Button_clicked();

    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();

};

#endif // AUTHENTICATIONFORM_H
