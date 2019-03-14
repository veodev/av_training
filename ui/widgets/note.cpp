#include <QPalette>

#include "note.h"
#include "ui_note.h"

Note::Note(int index, QColor color, QString message, QPixmap pixmap, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Note)
    , _index(index)
    , _message(message)
    , _icon(pixmap)
{
    ui->setupUi(this);
    QPalette palette = ui->iconLabel->palette();
    if (color.isValid() == true) {
        palette.setColor(ui->iconLabel->backgroundRole(), color);
    }
    else {
        palette.setColor(ui->iconLabel->backgroundRole(), Qt::white);
    }
    ui->iconLabel->setPalette(palette);
    setInfoLabelText(message);
    if (pixmap.isNull() != true) {
        ui->iconLabel->setPixmap(pixmap);
    }
    ui->infoLabel->setStyleSheet("background-color: black; border-style: none; border-width: 0px; border-radius: 5px;");
}

Note::~Note()
{
    delete ui;
}

void Note::setInfoLabelText(const QString& text)
{
    ui->infoLabel->setText(text);
}

void Note::setIconLabelText(const QString& text)
{
    ui->iconLabel->setText(text);
}

int Note::getIndex()
{
    return _index;
}

void Note::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);

    this->hide();
    emit deleteNote(_index);
}
