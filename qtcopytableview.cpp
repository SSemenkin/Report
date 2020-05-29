#include "qtcopytableview.h"

QtCopyTableView::QtCopyTableView()
{

}

void QtCopyTableView::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Copy)){
        if(!selectedIndexes().isEmpty()){
            QString text;
            QItemSelectionRange range = selectionModel()->selection().first();
            for (auto i = range.top(); i <= range.bottom(); ++i){
                QStringList rowContents;
                for (auto j = range.left(); j <= range.right(); ++j)
                rowContents << model()->index(i,j).data().toString();
                text += rowContents.join("\t");
                text += "\n";
            }
            QApplication::clipboard()->setText(text);
        }
    } else {
        QTableView::keyPressEvent(event);
    }
}
