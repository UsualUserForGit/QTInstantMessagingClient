#include "customtextedit.h"
#include <mainwindow.h>


CustomTextEdit::CustomTextEdit(QWidget *parent): QTextEdit(parent)
{
    // Connect the textChanged signal to the adjustSize slot
    connect(this, &QTextEdit::textChanged, this, &CustomTextEdit::adjustSize);
}

void CustomTextEdit::resizeEvent(QResizeEvent *event)
{
    // Adjust the size of the text edit when the widget is resized. In our case it's needed when a new input line is created
    adjustSize();
    QTextEdit::resizeEvent(event);
}

void CustomTextEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event); // Call the base class's keyPressEvent to handle default behavior

    // If the Enter key is pressed, emit the enterPressed signal to send the message
    if (event->key() == Qt::Key_Return)
    {
        emit enterPressed();
    }
}

void CustomTextEdit::adjustSize()
{
    // Adjust the maximum height of the text edit based on its content height
    setMaximumHeight(document()->size().toSize().height());
}

QSize CustomTextEdit::sizeHint() const
{
    // Customize the size hint to use the current width but set the height to the text edit's current height
    QSize hint = QTextEdit::sizeHint();
    hint.setHeight(size().height());
    return hint;
}
