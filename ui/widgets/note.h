#ifndef NOTE_H
#define NOTE_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui
{
class Note;
}

class Note : public QWidget
{
    Q_OBJECT

public:
    explicit Note(int index, QColor color, QString message = "", QPixmap pixmap = QPixmap(), QWidget* parent = 0);
    ~Note();

    void setInfoLabelText(const QString& text);
    void setIconLabelText(const QString& text);
    int getIndex();

signals:
    void deleteNote(int index);

protected:
    void mousePressEvent(QMouseEvent* e);

private:
    Ui::Note* ui;

    int _index;
    QString _message;
    QPixmap _icon;
};

#endif  // NOTE_H
