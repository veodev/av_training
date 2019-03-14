#include "bscanmovementcontrols.h"
#include "ui_bscanmovementcontrols.h"
#include "core.h"
#include "settings.h"

BScanMovementControls::BScanMovementControls(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BScanMovementControls)
    , _isEnabledParallelVideoBrowsing(restoreParallelVideoBrowsingStatus())
{
    ui->setupUi(this);
    _mode = false;
    changeMode();
}

BScanMovementControls::~BScanMovementControls()
{
    delete ui;
}

void BScanMovementControls::changeMode()
{
    if (_mode) {
        ui->modeButton->setText(tr("View mode"));
        // ui->modeButton->setAutoFillBackground(true);
        ui->modeButton->setStyleSheet("color: rgb(255, 30, 30)");
        ui->leftButton->setDisabled(false);
        ui->rightButton->setDisabled(false);
        ui->leftButton->setStyleSheet("background-color: rgb(255, 64, 64)");
        ui->rightButton->setStyleSheet("background-color: rgb(255, 64, 64)");
        if (_isEnabledParallelVideoBrowsing) {
            Core::instance().videoModeView();
        }
    }
    else {
        ui->modeButton->setText(tr("Realtime mode"));
        // ui->modeButton->setAutoFillBackground(true);
        ui->modeButton->setStyleSheet("color: rgb(64, 255, 128)");
        ui->leftButton->setDisabled(true);
        ui->rightButton->setDisabled(true);
        ui->leftButton->setStyleSheet("background-color: rgb(128, 128, 128)");
        ui->rightButton->setStyleSheet("background-color: rgb(128, 128, 128)");
        if (_isEnabledParallelVideoBrowsing) {
            Core::instance().videoModeRealtime();
        }
    }
    emit modeChanged(_mode);
}

void BScanMovementControls::setMode(bool mode)
{
    _mode = mode;
    changeMode();
}

void BScanMovementControls::enableParallelVideoBrowsing(bool isEnabled)
{
    _isEnabledParallelVideoBrowsing = isEnabled;
}

void BScanMovementControls::setModeButtonTitle(QString modeTitle)
{
    ui->modeButton->setText(modeTitle);
}

void BScanMovementControls::on_modeButton_released()
{
    _mode ? _mode = false : _mode = true;
    changeMode();
}

void BScanMovementControls::on_leftButton_released()
{
    emit movementLeft();
}

void BScanMovementControls::on_rightButton_released()
{
    emit movementRight();
}
