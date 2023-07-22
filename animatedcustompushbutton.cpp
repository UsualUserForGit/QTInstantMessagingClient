#include "animatedcustompushbutton.h"

#include <QEvent>
#include <QLinearGradient>


AnimatedCustomPushButton::AnimatedCustomPushButton(QWidget *parent) :
    QPushButton(parent),
    currentColor("white"),
    colorAnimation(this, "color")
{
    this->installEventFilter(this);
}

// In this Method we also have to apply our full styling for the button
void AnimatedCustomPushButton::SetColor(const QColor& color)
{
    currentColor = color;
    QString css = "QPushButton {color:black; background-color:white; border-radius: 15px; "
                  "border: 2px solid rgb(145, 145,145); font-size: 18px; ";
    QString strColor = QString("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
    css.append("background-color: " + strColor + "; }");
    setStyleSheet(css);
}

// Returns the current color of our animation
const QColor& AnimatedCustomPushButton::GetColor() const
{
    return currentColor;
}

// If button is hovered we call StartHoverEnterAnimation(); if hover is ended - StartHoverLeaveAnimation();
bool AnimatedCustomPushButton::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::HoverEnter) {
        StartHoverEnterAnimation();
    }

    if (e->type() == QEvent::HoverLeave) {
        StartHoverLeaveAnimation();
    }

    return false;
}

void AnimatedCustomPushButton::StartHoverEnterAnimation()
{
    colorAnimation.stop();
    colorAnimation.setDuration(300); //set  transition
    colorAnimation.setStartValue(GetColor()); //starts from current color
    colorAnimation.setEndValue(QColor(255, 206, 156));//set  hover color
    colorAnimation.setEasingCurve(QEasingCurve::InCubic);//animation style

    colorAnimation.start();
}

void AnimatedCustomPushButton::StartHoverLeaveAnimation()
{
    colorAnimation.stop();

    colorAnimation.setDuration(200); //set  transition
    colorAnimation.setStartValue(GetColor()); //starts from current color
    colorAnimation.setEndValue(QColor(255, 255, 255));//set  regular color

    colorAnimation.setEasingCurve(QEasingCurve::Linear);//animation style

    colorAnimation.start();
}
