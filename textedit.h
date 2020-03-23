#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QCompleter>
#include <QTextCursor>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = nullptr);
    ~TextEdit();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *c = nullptr;
};

#endif // TEXTEDIT_H
