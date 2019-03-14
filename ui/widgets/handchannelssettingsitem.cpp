#include "handchannelssettingsitem.h"
#include "ui_handchannelssettingsitem.h"

HandChannelsSettingsItem::HandChannelsSettingsItem(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HandChannelsSettingsItem)
{
    ui->setupUi(this);
}

HandChannelsSettingsItem::~HandChannelsSettingsItem()
{
    delete ui;
}

void HandChannelsSettingsItem::setAngle(int angle)
{
    ui->channelNameLabel->setText(QString::number(angle) + QString(0x00B0));
}

void HandChannelsSettingsItem::setFrequency(double frequency)
{
    ui->frequencyLabel->setText(QString::number(frequency, 'f', 1));
}

void HandChannelsSettingsItem::setMethod(InspectMethod method)
{
    QString methodName;
    switch (method) {
    case Echo:
        methodName = tr("ECHO");
        break;
    case MirrorShadow:
        methodName = tr("MSM");
        break;
    default:
        methodName = "-";
        break;
    }
    ui->methodLabel->setText(methodName);
}

void HandChannelsSettingsItem::setTimeDelay(double timeDelay)
{
    ui->timeDelayLabel->setText(QString::number(timeDelay / 10, 'f', 1));
}

void HandChannelsSettingsItem::setSensAndRecommendedSens(int sens, int recommendedSens)
{
    ui->sensLabel->setText(QString::number(sens) + " (" + QString::number(recommendedSens) + ")");
}

void HandChannelsSettingsItem::setGain(int gain)
{
    ui->gainLabel->setText(QString::number(gain));
}

void HandChannelsSettingsItem::setTvg(int tvg)
{
    ui->tvgLabel->setText(QString::number(tvg));
}

void HandChannelsSettingsItem::setGate(int startGate, int endGate)
{
    ui->gateStartEndLabel->setText(QString::number(startGate) + " - " + QString::number(endGate));
}

void HandChannelsSettingsItem::setItemBackgroundColor(int itemIndex)
{
    (itemIndex == 0 || itemIndex % 2 == 0) ? this->setStyleSheet("background-color: #ffffff") : this->setStyleSheet("background-color: #e6e6e6");
}
