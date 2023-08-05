#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QTabBar>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    connectedToHost(false),
    socket(new QTcpSocket),
    message_input_text_edit(new CustomTextEdit),
    lastMessageSentTime(),
    login("")
{
    ui->setupUi(this);

    // Creating horizontal label for General chat tab
    tabbar = ui->tabWidget->tabBar();
    ui->tabWidget->setTabText(0, "");

    QLabel *lbl1 = new QLabel();
    lbl1->setText("General Chat");
    tabbar->setTabButton(0, QTabBar::LeftSide, lbl1);

    // Setting up the CustomTextEdit widget and it's properties.
    message_input_text_edit->setMaximumHeight(30);
    message_input_text_edit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    message_input_text_edit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    //message_input_text_edit->setFont(QFont("Times", 10, QFont::Bold));

    // Place text input widget and send button inside input frame
    ui->input_frame_horizontalLayout->addWidget(message_input_text_edit, 2);
    ui->input_frame_horizontalLayout->addWidget(ui->send_Button, 0);

    // Create a shortcut for the Ctrl + Enter key combination it will create a new line inside message_input_text_edit
    QShortcut* enterCtrlShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), message_input_text_edit);

    // Connects section
    connect(enterCtrlShortcut, SIGNAL(activated()), this, SLOT(on_enter_ctrl_pressed()));
    connect(message_input_text_edit, SIGNAL(enterPressed()), this, SLOT(on_sendButton_clicked()));
    connect(ui->send_Button, SIGNAL(clicked()), this, SLOT(on_sendButton_clicked()));
    connect(ui->tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(tabBar_clicked(int)));
}



// This function adds some HTML to the message text and prints it in a chat display.
// If no more than 10 minutes have passed between the user's first and second message,
// the message is displayed without login
void MainWindow::printMessage(QString text, QString name, QDateTime time)
{
    QString dayTime = time.toString().mid(9, 8);

    QString message = "<p><font size=\"5\" color=\"#1CE5F3\" face=\"Arial\">" + name + "</font></p> "
                                                                                       "<font size=\"5\" color=\"black\" face=\"Calibri\">"+ text + "</font>"
                               " <font size=\"3\" color=\"grey\" face=\"Calibri\">"+ dayTime + "</font>";

    if (lastMessageSender.isNull() || !lastMessageSentTime.isValid())
    {
        lastMessageSentTime = time;
        lastMessageSender = name;
    }
    else
    {
        if (time.date().dayOfWeek() != lastMessageSentTime.date().dayOfWeek())
        {
            ui->chatDisplay->append("<p style = \"text-align: center;\"><hr><font size=\"5\" color=\"grey\" face=\"Arial\">" +
                                    time.date().toString() + "</font></p><hr><br>");
        }
        if(time.time().minute() - lastMessageSentTime.time().minute() <= 10 && lastMessageSender == name)
        {
            message = "<font size=\"5\" color=\"black\" face=\"Calibri\">"
                      + text + "</font>"
                               " <font size=\"3\" color=\"grey\" face=\"Calibri\">"+ dayTime + "</font>";
        }
    }

    qDebug() << "Last message sender - " << lastMessageSender << " New message sender - " << name;
    lastMessageSentTime = time;
    lastMessageSender = name;


    ui->chatDisplay->append(message);
}

// This function gets JSON object, converts it to an indented, UTF-8 encoded JSON document and sends it to the server
void MainWindow::sendJsonToServer(const QJsonObject &jsonObject)
{
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson();

    // Send the JSON data to the server
    socket->write(jsonData);
}

// Setter for client socket
void MainWindow::setSocket(QTcpSocket *socket)
{
    if (this->socket != socket)
    {
        this->socket = socket;

        connectedToHost = true;

        connect(this->socket, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
        connect(this->socket, &QTcpSocket::stateChanged, this, &MainWindow::socketStateChanged);
        connect(this->socket, &QTcpSocket::readyRead, this, &MainWindow::socketReadyRead);
    }
}

// Gettter for client socket
QTcpSocket *MainWindow::getSocket()
{
    return this->socket;
}

// Setter for client login
void MainWindow::setLogin(QString login)
{
    if (this->login != login)
    {
        this->login = login;
    }
}

// Getter for client login
QString MainWindow::getLogin()
{
    return this->login;
}

// If we are connected to the server then, messages will be sent to the server
// in other case we will just clear the input field and set it's default height
void MainWindow::on_sendButton_clicked()
{
    if (!message_input_text_edit->toPlainText().isEmpty())
    {
        QJsonObject jsonObject = createJsonObject(JsonFileType::Message, tabbar->currentIndex());
        sendJsonToServer(jsonObject);
        message_input_text_edit->clear();
        message_input_text_edit->setMaximumHeight(24);
    }

}

// If Ctrl + Enter is pressed while user is in Input field, we add another line of input
void MainWindow::on_enter_ctrl_pressed()
{
    message_input_text_edit->append("");
}

// If user is disconnected then, it prints to the console "Disconnected from server." and gives connectedToHost value - false
void MainWindow::socketDisconnected()
{
    qDebug() << "Disconnected from server.";
    connectedToHost = false;
}

// If socket state has been changed then this function will prints it to the console
void MainWindow::socketStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QString socketIpAddress = client->peerAddress().toString();
    int port = client->peerPort();
    QString desc;
    // simply print out a relevant message according to its new state
    if (state == QAbstractSocket::UnconnectedState)
        desc = "The socket is not connected.";
    else if (state == QAbstractSocket::HostLookupState)
        desc = "The socket is performing a host name lookup.";
    else if (state == QAbstractSocket::ConnectingState)
        desc = "The socket has started establishing a connection.";
    else if (state == QAbstractSocket::ConnectedState)
        desc = "A connection is established.";
    else if (state == QAbstractSocket::BoundState)
        desc = "The socket is bound to an address and port.";
    else if (state == QAbstractSocket::ClosingState)
        desc = "The socket is about to close (data may still be waiting to be written).";
    else if (state == QAbstractSocket::ListeningState)
        desc = "For internal use only.";
    qDebug() << "Socket state changed (" + socketIpAddress + ":" + QString::number(port) + "): " + desc;
}

// This slot invokes whenever a new JSON file is sent to the client
// and if data and JSON document are not empty. it invokes processServerResponse() function
void MainWindow::socketReadyRead()
{
    QTcpSocket* server = qobject_cast<QTcpSocket*>(QObject::sender());
    QByteArray jsonData = server->readAll();

    // Parse the JSON data
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

    // Check if parsing was successful
    if (!jsonDocument.isNull() && jsonDocument.isObject())
    {
        processServerResponse(jsonDocument.object());
    }
    else
    {
        qDebug() << "jsonDocument error";
    }
}

void MainWindow::tabBar_clicked(int index)
{
    if (index != ui->tabWidget->currentIndex())
    {
        QJsonObject jsonObject = createJsonObject(JsonFileType::LoadChannelMessages, index);
        sendJsonToServer(jsonObject);
    }
}

// This function creates JSON objects depending on the JsonFileType enum
QJsonObject MainWindow::createJsonObject(JsonFileType jsonFile,int index)
{

    QJsonObject mainJsonObject;
    QJsonObject innerJsonObject;

    if (index != -1)
    {
        QLabel *tabButton_label = qobject_cast<QLabel*>(tabbar->tabButton(index, QTabBar::LeftSide));
        switch (jsonFile)
        {
        case JsonFileType::Message:

            innerJsonObject["sender"] = this->login;
            innerJsonObject["message_text"] = message_input_text_edit->toPlainText();
            innerJsonObject["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");
            innerJsonObject["chatline_name"] = tabButton_label->text();

            mainJsonObject["message"] = innerJsonObject;
            break;
        case JsonFileType::LoadChannelMessages:
            // Доделать

            innerJsonObject["chatline_name"] = tabButton_label->text();
            mainJsonObject["load_chatline_messages"] = innerJsonObject;
            break;
        default:
            break;
        }

    }
    else
    {
        qDebug() << "Wrong index - " << index;
    }

    return mainJsonObject;
}

// This function processes JSON files which are sent from the server and prints them to the chat display
void MainWindow::processServerResponse(QJsonObject jsonObject)
{
    const QString key = jsonObject.begin().key();

    qDebug() << "Key:" << key;

    if (key == "message")
    {
        QJsonObject messageObject = jsonObject.value("message").toObject();

        // Extract individual values from the "message" object
        QDateTime created_at = QDateTime::fromString(messageObject.value("created_at").toString(), "yyyy-MM-ddTHH:mm:ss");
        QString message = messageObject.value("message_text").toString();
        QString sender = messageObject.value("sender").toString();

        // Remove the date part from the original string
        qDebug() << "Message created at - " << created_at.toString();
        // Print the extracted values
        printMessage(message, sender, created_at);
    }
    else if (key == "messages_to_load")
    {
        QJsonArray jsonArray = jsonObject["messages_to_load"].toArray();

        // Iterate over the messages and output their contents
        for (const QJsonValue& value : jsonArray)
        {
            QJsonObject messageObject = value.toObject();

            //qDebug() << "Chatline Name:" << messageObject["chatline_name"].toString();
            QDateTime created_at = QDateTime::fromString(messageObject["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzz");
            QString message = messageObject["message_text"].toString();
            QString sender = messageObject["sender"].toString();

            qDebug() << "loaded message created at - " << created_at.toString();

            printMessage(message, sender, created_at);
        }
    }
}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}
