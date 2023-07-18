#include <QPainter>
#include <QDebug>

#include "authenticationform.h"
#include "ui_authenticationform.h"


AuthenticationForm::AuthenticationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthenticationForm),
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

    //Finally, add our custom login button to the layout
    ui->login_button_frame_horizontalLayout->addWidget(login_Button);

    // Making Form not resizable
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    //connection section
    connect(login_Button, SIGNAL(clicked()), this, SLOT(login_Button_clicked()));
}

AuthenticationForm::~AuthenticationForm()
{
    delete ui;
}


// Override paintEvent to make background for AuthenticationForm Widget and logo_label.
// I don't do it in constructor because in it, size gives false values
void AuthenticationForm::paintEvent(QPaintEvent *event)
{
    QPixmap pixmap(":/images/background.jpg");
    QPainter paint(this);
    int widWidth = ui->main_frame->width();
    int widHeight = ui->main_frame->height();
    paint.drawPixmap(0, 0, pixmap.scaled(widWidth, widHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    QPixmap logo_pixmap(":/images/inside_logo.png");
    ui->logo_label->setPixmap(logo_pixmap.scaled(ui->logo_label->size(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QWidget::paintEvent(event);
}

// Functionality after login button is clicked
void AuthenticationForm::login_Button_clicked()
{
    qDebug() << "Clicked";
}
