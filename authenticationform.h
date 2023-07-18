#ifndef AUTHENTICATIONFORM_H
#define AUTHENTICATIONFORM_H

#include <QWidget>
#include <QPaintEvent>

#include "animatedcustompushbutton.h"


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

protected:
    AnimatedCustomPushButton *login_Button;

    void paintEvent(QPaintEvent *event) override;

private slots:
    void login_Button_clicked();
};

#endif // AUTHENTICATIONFORM_H
