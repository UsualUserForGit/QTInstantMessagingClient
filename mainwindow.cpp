#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), connectedToHost(false), message_input_text_edit(new CustomTextEdit)
{
    ui->setupUi(this);

    // Setting up the CustomTextEdit widget and it's properties.
    message_input_text_edit->setMaximumHeight(30);
    message_input_text_edit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    message_input_text_edit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    ui->gridLayout->addWidget(message_input_text_edit, 2, 0, 1, 2);

    // Create a shortcut for the Ctrl + Enter key combination it will create a new line inside message_input_text_edit
    QShortcut* enterCtrlShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), message_input_text_edit);

    // Connects section
    connect(enterCtrlShortcut, SIGNAL(activated()), this, SLOT(on_enter_ctrl_pressed()));
    connect(message_input_text_edit, SIGNAL(enterPressed()), this, SLOT(on_sendButton_clicked()));
    connect(ui->send_Button, SIGNAL(clicked()), this, SLOT(on_sendButton_clicked()));
}

MainWindow::MainWindow(QTcpSocket *socket, QWidget *parent): MainWindow(parent)
{
    this->socket = socket;
}

// Connection routine
void MainWindow::on_connectButton_clicked()
{
    // If user connected
    if (!connectedToHost)
    {
        // create a QTcpSocket object called socket and make it connect to a host at 127.0.0.1 or 45.95.203.159 on port 8801.
        socket = new QTcpSocket();
        // connected the socket object to its respective slot functions when connected(),disconnected(), and readReady() signals were triggered.
        // This is exactly the same as the server code
        connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
        // Since this is only for testing purposes, we will connect the client to our test server,
        // which is located on the same computer.
        // If you're running the server on another computer,
        // you may change the IP address to a LAN or WAN address, depending on your need.
        socket->connectToHost("127.0.0.1", 8001);
    }
    else
    {
        // If user disconnected
        QString name = ui->nameInput->text();
        socket->write("<font color=\"orange\">" + name.toUtf8() + " has left the chat room.</font>");
        socket->disconnectFromHost();
    }

}

// Socker Routine. Sending message that user join the chat and changing button text to disconnected
void MainWindow::socketConnected()
{
    qDebug() << "Connected to server.";
    printMessage("<font color=\"Green\">Connected to server.</font>");
    QString name = ui->nameInput->text();
    //socket->write("<font color=\"purple\">" + name.toUtf8() + " has joined the chat room.</font>");
    ui->connectButton->setText("Disconnect");
    connectedToHost = true;
}

// Sending message that user left the chat and changing button text to connected
void MainWindow::socketDisconnected()
{
    qDebug() << "Disconnected from server.";
    printMessage("<font color=\"red\">Disconnected from server.</font>");
    ui->connectButton->setText("Connect");
    connectedToHost = false;
}

// If someone has send the message to the chat and socket is ready to read it, the message will be displayed in the chatDisplay
void MainWindow::socketReadyRead()
{
    ui->chatDisplay->append(socket->readAll());
}

// Same goes for the user
void MainWindow::printMessage(QString message)
{
    ui->chatDisplay->append(message);
}

void MainWindow::sendJsonToServer(const QJsonObject &jsonObject)
{
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson();

    // Send the JSON data to the server
    socket->write(jsonData);
}

// If we are connected to the server then, messages will be sent to the server
// in other case we will just clear the input field and set it's default height
void MainWindow::on_sendButton_clicked()
{
    if (connectedToHost)
    {


        //QString name = ui->nameInput->text();
        //QString message = message_input_text_edit->toPlainText();
        //socket->write("<font color=\"blue\">" + name.toUtf8() + "</font>: " + message.toUtf8());
    }
    message_input_text_edit->clear();
    message_input_text_edit->setMaximumHeight(24);
}

// If Ctrl + Enter is pressed while user is in Input field, we add another line of input
void MainWindow::on_enter_ctrl_pressed()
{
    message_input_text_edit->setMaximumHeight(message_input_text_edit->maximumHeight() + 16);
    message_input_text_edit->append("");
}

MainWindow::~MainWindow()
{
    delete ui;
}
