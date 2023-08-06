#include <QDebug>
#include <QMessageBox>

#include "authenticationform.h"
#include "ui_authenticationform.h"


AuthenticationForm::AuthenticationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthenticationForm),
    socket(new QTcpSocket()),
    login_Button(new AnimatedCustomPushButton()),
    register_Button(new AnimatedCustomPushButton()),
    connectedToHost(false)
{
    ui->setupUi(this);

    // Set up icons for password and login labels
    QPixmap password_pixmap(":/images/password_logo.png");
    ui->password_label->setPixmap(password_pixmap.scaled(QSize(40, 40), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    QPixmap login_pixmap(":/images/login_logo.png");
    ui->login_label->setPixmap(login_pixmap.scaled(QSize(40, 40), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // Set up styling for login button. Styling is done here because login_Button is a custom QWidget
    QString styleSheetForButtons = "QPushButton { color:black; background-color:white; border-radius: 15px;"
                                   "border: 2px solid rgb(145, 145,145); font-size: 18px;}";
    this->login_Button->setStyleSheet(styleSheetForButtons);
    this->register_Button->setStyleSheet(styleSheetForButtons);

    this->login_Button->setText("Sign In");
    this->register_Button->setText("Sign Up");


    // Set maximum height for line edits and login button
    ui->login_lineEdit->setMaximumHeight(40);
    ui->password_lineEdit->setMaximumHeight(40);
    this->login_Button->setMaximumHeight(60);
    this->register_Button->setMaximumHeight(60);

    // Finally, add our custom login button to the layout
    ui->login_button_frame_horizontalLayout->addWidget(login_Button);
    ui->login_button_frame_horizontalLayout->addWidget(register_Button);

    // Making Form not resizable
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    // Connections
    connect(login_Button, QPushButton::clicked, this, AuthenticationForm::login_Button_clicked);
    connect(register_Button, QPushButton::clicked, this, AuthenticationForm::register_Button_clicked);

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

// This function creates JSON objects depending on the JsonFileType enum
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

    return mainJsonObject;
}

// This function sends data (QJSonObject) to the server
void AuthenticationForm::sendJsonToServer(const QJsonObject &jsonObject)
{
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson();

    // Send the JSON data to the server
    socket->write(jsonData);
}

// This function proceeds JSON data depending on the key of the JSON file
void AuthenticationForm::processServerResponse(QJsonObject jsonObject)
{
    const QString key = jsonObject.begin().key();
    const QJsonValue value = jsonObject.begin().value();

    qDebug() << "Key:" << key;

    if (key == "login_reslt")
    {
        bool login_results = value.toBool();
        if (login_results)
        {
            ui->data_correctness_label->setStyleSheet("color:green;");
            ui->data_correctness_label->setText("Welcome " + ui->login_lineEdit->text());

            disconnect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
            disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
            disconnect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));

            mainMessengerWindow.setSocket(socket);
            mainMessengerWindow.setLogin(ui->login_lineEdit->text());
            mainMessengerWindow.setWindowTitle("Whisper Talk");
            mainMessengerWindow.setWindowIcon(QIcon(":/images/inside_logo.png"));
            mainMessengerWindow.show();
            this->close();
        }
        else
        {
            ui->data_correctness_label->setStyleSheet("color:red;");
            ui->data_correctness_label->setText("Invalid login or password");
        }
    }
    else if (key == "register_result")
    {
        bool register_results = value.toBool();
        if (register_results)
        {
            ui->data_correctness_label->setStyleSheet("color:green;");
            ui->data_correctness_label->setText("Successful registration " + ui->login_lineEdit->text());
        }
        else
        {
            ui->data_correctness_label->setStyleSheet("color:red;");
            ui->data_correctness_label->setText("User " + ui->login_lineEdit->text() + " already exists, create different login");
            ui->login_lineEdit->clear();
        }
    }
    else
    {
        qDebug() << "unkown key - " << key;
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

// Functionality after sign in button is clicked
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

// Functionality after sign up button is clicked
void AuthenticationForm::register_Button_clicked()
{
    if (!ui->login_lineEdit->text().isEmpty() && !ui->password_lineEdit->text().isEmpty())
    {
        QJsonObject jsonObject = createJsonObject(JsonFileType::RegisterUser);
        sendJsonToServer(jsonObject);
    }
    else
    {
        QMessageBox emptyFieldsBox(QMessageBox::Information, "Empty fields", "Please, fill in all fields", QMessageBox::Ok, this);
        emptyFieldsBox.exec();
    }
}

// This slot invokes whenever socket is connected to the server
void AuthenticationForm::socketConnected()
{
    qDebug() << "Connected to server.";
    connectedToHost = true;
}

// This slot invokes whenever socket is disconnected from the server
void AuthenticationForm::socketDisconnected()
{
    qDebug() << "Disconnected from server.";
    connectedToHost = false;
}

// This slot invokes when the server sends some data to the socket and it is ready to be read
void AuthenticationForm::socketReadyRead()
{
    // I use QObject::sender() to get the pointer of the object that emitted the readyRead signal
    // And convert it to the QTcpSocket class so that I can access its readAll() function.
    QTcpSocket* server = qobject_cast<QTcpSocket*>(QObject::sender());
    QByteArray jsonData = server->readAll();

    // Parse the JSON data
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

    // Check if parsing was successful
    if (!jsonDocument.isNull() && jsonDocument.isObject())
    {
        processServerResponse(jsonDocument.object());
    }
}



