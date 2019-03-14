#include <QCoreApplication>

#include "notifier.h"
#include "ui_notifier.h"
#include "note.h"
#include "debug.h"

Notifier& Notifier::instance()
{
    static Notifier _instance;
    return _instance;
}

Notifier::Notifier(QWidget* parent)
    : QFrame(parent)
    , ui(new Ui::Notifier)
    , _countOfNotes(0)
{
    ui->setupUi(this);
    ui->label->setVisible(false);
    this->hide();
    ui->label->setStyleSheet("background-color: black; border-color: white; border-style: outset; border-width: 1px; border-radius: 2px; ");
#if defined TARGET_AVICONDB || defined TARGET_AVICONDBHS
    this->setMinimumHeight(500);
#endif
}

Notifier::~Notifier()
{
    qDebug() << "Deleting notifier...";
    delete ui;
    qDebug() << "Notifier deleted!";
}

void Notifier::showNotifier(bool isVisible)
{
    this->setVisible(isVisible);
}

void Notifier::addNote(QColor color, const QString& message, QPixmap icon)
{
    ItemOfNotifier element;
    element.color = color;
    element.message = message;
    element.icon = icon;

    _notes.insert(_countOfNotes, element);
    ++_countOfNotes;
    updateView();
}

void Notifier::updateView()
{
    clearView();
    if (_notes.isEmpty()) {
        this->hide();
        return;
    }

    this->show();
    if (_notes.count() > MAX_COUNT_NOTES) {
        ui->label->setVisible(true);
        for (auto it = _notes.end() - MAX_COUNT_NOTES; it != _notes.end(); ++it) {
            Note* note = new Note(it.key(), it.value().color, it.value().message, it.value().icon);
            ASSERT(connect(note, &Note::deleteNote, this, &Notifier::onDeleteNote));
            ui->verticalLayout->addWidget(note);
            _notesPtr.append(note);
        }
        ui->label->setText("+ " + QString::number(_notes.count() - MAX_COUNT_NOTES) + tr(" messages"));
    }
    else {
        auto it = _notes.begin();
        for (int i = 0; i < _notes.count(); ++i, ++it) {
            Note* note = new Note(it.key(), it.value().color, it.value().message, it.value().icon);
            ASSERT(connect(note, &Note::deleteNote, this, &Notifier::onDeleteNote));
            ui->verticalLayout->addWidget(note);
            _notesPtr.append(note);
        }
        ui->label->setVisible(false);
    }
}

void Notifier::clearView()
{
    for (Note* item : _notesPtr) {
        item->setParent(Q_NULLPTR);
        delete item;
    }
    _notesPtr.clear();
}

void Notifier::setParent(QWidget* parent)
{
    ASSERT(connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &Notifier::aboutToQuit));
    QFrame::setParent(parent);
}

void Notifier::onDeleteNote(int index)
{
    _notes.erase(_notes.find(index));
    updateView();
}

void Notifier::aboutToQuit()
{
    QFrame::setParent(Q_NULLPTR);
}
