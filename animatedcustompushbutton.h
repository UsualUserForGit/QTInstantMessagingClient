#ifndef ANIMATEDCUSTOMPUSHBUTTON_H
#define ANIMATEDCUSTOMPUSHBUTTON_H

#include <QPushButton>
#include <QColor>
#include <QPropertyAnimation>


class AnimatedCustomPushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor) // Handling color of the animation

public:
    explicit AnimatedCustomPushButton(QWidget *parent = 0);

    void SetColor(const QColor& color);
    const QColor& GetColor() const;

protected:
    bool eventFilter(QObject *, QEvent *e); // Event filter to catch if button is hovered or not

private:
    QColor currentColor; // Current color of the button

    QPropertyAnimation colorAnimation; // Animation of the button, in this case - just color change

    void StartHoverEnterAnimation(); // Starts animation when mouse hovers over the button
    void StartHoverLeaveAnimation(); // Starts animation when mouse leaves the button
};
#endif // ANIMATEDCUSTOMPUSHBUTTON_H
