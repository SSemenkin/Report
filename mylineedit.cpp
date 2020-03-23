#include "mylineedit.h"

MyLineEdit::MyLineEdit()
{

}

void MyLineEdit::keyPressEvent(QKeyEvent *e)
{
     if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
     {
         emit command(text());
     }
     else QLineEdit::keyPressEvent(e);
}
