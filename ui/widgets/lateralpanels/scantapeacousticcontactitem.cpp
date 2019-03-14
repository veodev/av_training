#include "scantapeacousticcontactitem.h"
#include "ui_scantapeacousticcontactitem.h"

ScanTapeAcousticContactItem::ScanTapeAcousticContactItem(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ScanTapeAcousticContactItem)
    , _prevState(false)
    , _side(LeftPanel)
{
    ui->setupUi(this);
    ui->topAcLabel->hide();
    ui->bottomAcLabel->hide();
}

ScanTapeAcousticContactItem::~ScanTapeAcousticContactItem()
{
    delete ui;
}

void ScanTapeAcousticContactItem::showBottomWidget()
{
    ui->bottomWidget->show();
}

void ScanTapeAcousticContactItem::hideBottomWidget()
{
    ui->bottomWidget->hide();
}

void ScanTapeAcousticContactItem::hideBottomWidget(bool isHide)
{
    ui->bottomWidget->setVisible(isHide);
}

void ScanTapeAcousticContactItem::setSide(LateralPanelType side)
{
    _side = side;
}

void ScanTapeAcousticContactItem::setAcousticContactState(AcousticContactSegment segment, bool isAcousticContact)
{
    switch (segment) {
    case TopSegment:
        ui->topAcLabel->setVisible(!isAcousticContact);
        break;
    case BottomSegment:
        ui->bottomAcLabel->setVisible(!isAcousticContact);
        break;
    }
}

void ScanTapeAcousticContactItem::updateState(bool isAcousticContact)
{
    _prevState = isAcousticContact;
}

bool ScanTapeAcousticContactItem::prevState() const
{
    return _prevState;
}
