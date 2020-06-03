#include "mylineedit.h"

MyLineEdit::MyLineEdit()
{

}

MyLineEdit::MyLineEdit(QWidget *parent)
{
    setParent(parent);
}

void MyLineEdit::keyPressEvent(QKeyEvent *e)
{
     if((e->key () == Qt::Key_Enter || e->key() == Qt::Key_Return) && e->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier)){
         emit letsGetEDR (text());
     }
     else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return){
         emit command(text());
     }
     else QLineEdit::keyPressEvent(e);
}
