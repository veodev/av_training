#include "tablesettingsitem.h"
#include "ui_tablesettingsitem.h"

TableSettingsItem::TableSettingsItem(bool isBothRail, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TableSettingsItem)
{
    ui->setupUi(this);
    ui->itemSingleRail->setVisible(!isBothRail);
    ui->itemBothRail->setVisible(isBothRail);
}

TableSettingsItem::~TableSettingsItem()
{
    delete ui;
}

void TableSettingsItem::setChannelName(const QString& channelName)
{
    ui->chanelNameLabel->setText(channelName);
}

void TableSettingsItem::setChannelNameSingle(const QString& channelName)
{
    ui->chanelNameLabelSingle->setText(channelName);
}

void TableSettingsItem::setLeftTimeDelay(double timeDelay)
{
    ui->timeDelayLabelLeft->setText(QString::number(timeDelay / 10, 'f', 1));
}

void TableSettingsItem::setRightTimeDelay(double timeDelay)
{
    ui->timeDelayLabelRight->setText(QString::number(timeDelay / 10, 'f', 1));
}

void TableSettingsItem::setTimeDelay(double timeDelay)
{
    ui->timeDelayLabel->setText(QString::number(timeDelay / 10, 'f', 1));
}

void TableSettingsItem::setLeftSensAndRecommendedSens(int sens, int recommendedSens)
{
    ui->sensLabelLeft->setText(QString::number(sens) + " (" + QString::number(recommendedSens) + ")");
}

void TableSettingsItem::setRightSensAndRecommendedSens(int sens, int recommendedSens)
{
    ui->sensLabelRight->setText(QString::number(sens) + " (" + QString::number(recommendedSens) + ")");
}

void TableSettingsItem::setSensAndRecommendedSens(int sens, int recommendedSens)
{
    ui->sensLabel->setText(QString::number(sens) + " (" + QString::number(recommendedSens) + ")");
}

void TableSettingsItem::setLeftGain(int gain)
{
    ui->gainLabelLeft->setText(QString::number(gain));
}

void TableSettingsItem::setRightGain(int gain)
{
    ui->gainLabelRight->setText(QString::number(gain));
}

void TableSettingsItem::setGain(int gain)
{
    ui->gainLabel->setText(QString::number(gain));
}

void TableSettingsItem::setLeftTvg(int tvg)
{
    ui->tvgLabelLeft->setText(QString::number(tvg));
}

void TableSettingsItem::setRightTvg(int tvg)
{
    ui->tvgLabelRight->setText(QString::number(tvg));
}

void TableSettingsItem::setTvg(int tvg)
{
    ui->tvgLabel->setText(QString::number(tvg));
}

void TableSettingsItem::setLeftGate(int startGate, int endGate)
{
    ui->gateStartEndLabelLeft->setText(QString::number(startGate) + " - " + QString::number(endGate));
}

void TableSettingsItem::setRightGate(int startGate, int endGate)
{
    ui->gateStartEndLabelRight->setText(QString::number(startGate) + " - " + QString::number(endGate));
}

void TableSettingsItem::setGate(int startGate, int endGate)
{
    ui->gateStartEndLabel->setText(QString::number(startGate) + " - " + QString::number(endGate));
}

void TableSettingsItem::setItemBackgroundColor(int itemIndex)
{
    (itemIndex == 0 || itemIndex % 2 == 0) ? this->setStyleSheet("background-color: #ffffff") : this->setStyleSheet("background-color: #e6e6e6");
}
