#include "mainwindow.h"
#include "authenticationform.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AuthenticationForm form;
    form.setWindowTitle("Login");
    form.setWindowIcon(QIcon(":/images/inside_logo.png"));
    form.show();

    return a.exec();
}
