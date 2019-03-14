#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QWidget>
#include <QMouseEvent>
#include <QFrame>
#include <QMap>

#define MAX_COUNT_NOTES 4

class Note;

namespace Ui
{
class Notifier;
}

struct ItemOfNotifier
{
    QColor color;
    QString message;
    QPixmap icon;
};

class Notifier : public QFrame
{
    Q_OBJECT

public:
    static Notifier& instance();

    void showNotifier(bool isVisible = false);
    void addNote(QColor color = Qt::white, const QString& message = "", QPixmap icon = QPixmap());
    void updateView();
    void clearView();

    void setParent(QWidget* parent);

private:
    explicit Notifier(QWidget* parent = 0);
    ~Notifier();

private slots:
    void onDeleteNote(int index);
    void aboutToQuit();

private:
    Ui::Notifier* ui;

    int _countOfNotes;
    QMap<int, ItemOfNotifier> _notes;
    QList<Note*> _notesPtr;
};

#endif  // NOTIFIER_H
