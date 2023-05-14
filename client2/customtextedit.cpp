#include "customtextedit.h"

void TCustomTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Return) {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        setTextCursor(cursor);
    }
    else if (event->key() == Qt::Key_Return) {
        emit sendMsg();
    }
    else {
        QPlainTextEdit::keyPressEvent(event);
    }
}
