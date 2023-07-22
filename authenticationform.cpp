#include <QPainter>
#include <QDebug>
#include <QMessageBox>

#include "authenticationform.h"
#include "ui_authenticationform.h"


AuthenticationForm::AuthenticationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthenticationForm),
    socket(new QTcpSocket()),
    login_Button(new AnimatedCustomPushButton())
{
    ui->setupUi(this);

    // Set up icons for password and login labels
    QPixmap password_pixmap(":/images/password_logo.png");
    ui->password_label->setPixmap(password_pixmap.scaled(QSize(40, 40), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    QPixmap login_pixmap(":/images/login_logo.png");
    ui->login_label->setPixmap(login_pixmap.scaled(QSize(40, 40), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // Set up styling for login button. Styling is done here because login_Button is a custom QWidget
    this->login_Button->setStyleSheet("QPushButton { color:black; background-color:white; border-radius: 15px;"
                                      "border: 2px solid rgb(145, 145,145); font-size: 18px;}");

    this->login_Button->setText("Login");


    // Set maximum height for line edits and login button
    ui->login_lineEdit->setMaximumHeight(40);
    ui->password_lineEdit->setMaximumHeight(40);
    this->login_Button->setMaximumHeight(60);

    // Finally, add our custom login button to the layout
    ui->login_button_frame_horizontalLayout->addWidget(login_Button);

    // Making Form not resizable
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    // Connections
    connect(login_Button, SIGNAL(clicked()), this, SLOT(login_Button_clicked()));

    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));

    socket->connectToHost("127.0.0.1", 8001);
}

// Destructor
AuthenticationForm::~AuthenticationForm()
{
    delete ui;
}


QJsonObject AuthenticationForm::createJsonObject(JsonFileType jsonFile)
{
    QJsonObject mainJsonObject;
    QJsonObject innerJsonObject;


    innerJsonObject["login"] = ui->login_lineEdit->text();
    innerJsonObject["password"] = ui->password_lineEdit->text();

    switch (jsonFile) {
    case JsonFileType::SignInData:

        mainJsonObject["login"] = innerJsonObject;
        break;
    case JsonFileType::RegisterUser:

        mainJsonObject["register"] = innerJsonObject;
        break;
    default:
        break;
    }

    // Convert the JSON object to a JSON document
    QJsonDocument jsonDocument(mainJsonObject);

    // Convert the JSON document to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    // Create and open a file for writing
    QFile file("data.json");
    if (file.open(QIODevice::WriteOnly))
    {
        // Write the JSON data to the file
        file.write(jsonData);
        file.close();
        qDebug() << "JSON file created successfully.";
    } else
    {
        qDebug() << "Failed to create JSON file.";
    }

    return mainJsonObject;
}

void AuthenticationForm::sendJsonToServer(const QJsonObject &jsonObject)
{
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson();

    // Send the JSON data to the server
    socket->write(jsonData);
}

void AuthenticationForm::handleServerResponse(QJsonObject jsonObject)
{
    const QString key = jsonObject.begin().key();
    const QJsonValue value = jsonObject.begin().value();

    qDebug() << "Key:" << key;

    if (key == "login_reslt")
    {
        bool login_results = value.toBool();
        if (login_results)
        {
            w.show();
            this->close();
        }
        else
        {

        }
    }
    else if (key == "register_result")
    {
        bool login_results = value.toBool();
        qDebug() << login_results << " - register_result";
    }
    else
    {
        qDebug() << "Unkown JSON key - " << key;
    }
}


// Override paintEvent to make background for AuthenticationForm Widget and logo_label.
// I don't do it in constructor because  size gives false values in it
void AuthenticationForm::paintEvent(QPaintEvent *event)
{
    QPixmap pixmap(":/images/background.jpg");
    QPainter paint(this);
    int widWidth = ui->main_frame->width();
    int widHeight = ui->main_frame->height();
    paint.drawPixmap(0, 0, pixmap.scaled(widWidth, widHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    QPixmap logo_pixmap(":/images/inside_logo.png");
    ui->logo_label->setPixmap(logo_pixmap.scaled(QSize(ui->logo_label->width(), ui->logo_label->height()),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QWidget::paintEvent(event);
}

// Functionality after login button is clicked
void AuthenticationForm::login_Button_clicked()
{
    if (!ui->login_lineEdit->text().isEmpty() && !ui->password_lineEdit->text().isEmpty())
    {
        QJsonObject jsonObject = createJsonObject(JsonFileType::SignInData);
        sendJsonToServer(jsonObject);
    }
    else
    {
        QMessageBox emptyFieldsBox(QMessageBox::Information, "Empty fields", "Please, fill in all fields", QMessageBox::Ok, this);
        emptyFieldsBox.exec();
    }
}

void AuthenticationForm::socketConnected()
{
    qDebug() << "Connected to server.";
}

void AuthenticationForm::socketDisconnected()
{
    qDebug() << "Disconnected from server.";

}

void AuthenticationForm::socketReadyRead()
{
    // I use QObject::sender() to get the pointer of the object that emitted the readyRead signal
    // And convert it to the QTcpSocket class so that I can access its readAll() function.
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QByteArray jsonData = client->readAll();

    // Parse the JSON data
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

    // Check if parsing was successful
    if (!jsonDocument.isNull() && jsonDocument.isObject())
    {
        handleServerResponse(jsonDocument.object());
    }
}

// Chain of responsibility for server responses


