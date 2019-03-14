#include "screenshotsserviceview.h"
#include "ui_screenshotsserviceview.h"
#include "appdatapath.h"

#include <debug.h>
#include <QLabel>
#include <QFileInfo>
#include <QDateTime>

ScreenShotsServiceView::ScreenShotsServiceView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ScreenShotsServiceView)
{
    ui->setupUi(this);
    _dateLabel = new QLabel(ui->label);
    _dateLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _dateLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _dateLabel->setStyleSheet("background-color: rgba(255, 255, 255, 255); color: rgba(0, 0, 0, 255); font: bold 16px; border: 2px solid red; border-radius: 4px; font: italic bold");
    _dateLabel->setVisible(false);
}

ScreenShotsServiceView::~ScreenShotsServiceView()
{
    delete ui;
}

void ScreenShotsServiceView::showScreen(QString& path) const
{
    QFileInfo currentFileInfo(path);
    if (currentFileInfo.exists()) {
        _dateLabel->setText(currentFileInfo.created().toString(" dd.MM.yyyy - hh:mm:ss "));
        _dateLabel->show();
        QPixmap image(path);
        ui->label->setPixmap(image.scaled(ui->label->width(), ui->label->height(), Qt::KeepAspectRatio));
    }
    else {
        _dateLabel->hide();
        ui->label->setText(tr("No screenshot!"));
    }
}

void ScreenShotsServiceView::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    this->hide();
}
