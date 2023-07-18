#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>


//I Create custom QTextEdit, because - on pressing the Enter button, QTextEdit, creates an additional line of input.
//In instant messengers, by clicking on this button, messages are usually sent, and to move to a new line,
//they use Enter + Ctrl, which I implement here. More Explanatory comments will be inside the customtextedit.cpp file
class CustomTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    CustomTextEdit(QWidget *parent = nullptr);
protected:

    void resizeEvent(QResizeEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event);

    void adjustSize();
    QSize sizeHint() const override;
signals:
    void enterPressed();
};

#endif // CUSTOMTEXTEDIT_H
