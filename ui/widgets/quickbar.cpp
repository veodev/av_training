#include "quickbar.h"
#include "ui_quickbar.h"
#include "settings.h"
#include "debug.h"
#include "androidJNI.h"
#include "core.h"

QuickBar::QuickBar(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::QuickBar)
{
    ui->setupUi(this);
    ui->brightnessSpinBox->setMinimum(0);
    ui->brightnessSpinBox->setMaximum(100);
    ui->brightnessSpinBox->setStepBy(1);
    ui->brightnessSpinBox->setValue(restoreBrightness(), false);

    ui->volumeSpinbox->setMinimum(0);
    ui->volumeSpinbox->setMaximum(100);
    ui->volumeSpinbox->setStepBy(1);
    ui->volumeSpinbox->setValue(getVolume(), false);

    ASSERT(connect(ui->brightnessSpinBox, &SpinBoxNumber::valueChanged, this, &QuickBar::onBrightnessChanged));
    ASSERT(connect(ui->volumeSpinbox, &SpinBoxNumber::valueChanged, this, &QuickBar::onVolumeChanged));

#if defined DEBUG && defined IMX_DEVICE
    ui->bottomButtonsWidget->show();
#elif defined DEBUG && !defined IMX_DEVICE
    ui->bottomButtonsWidget->show();
    ui->wireOutputButton->hide();
    ui->btWireOutputButton->hide();
    ui->playMediaButton->hide();
#else
    ui->bottomButtonsWidget->hide();
#endif

#if defined TARGET_AVICON15 && defined ANDROID
    setBrightnessJNI(restoreBrightness());
#endif

#if defined TARGET_AVICON15
    ui->wifiPageButton->hide();
    ui->bluetoothPageButton->hide();
#endif
}

QuickBar::~QuickBar()
{
    delete ui;
}

void QuickBar::setVolume(qreal value)
{
    ui->volumeSpinbox->setValue(value, false);
}

void QuickBar::setBrightness(qreal value)
{
    ui->brightnessSpinBox->setValue(value, false);
}


void QuickBar::setVisible(bool visible)
{
    if (visible == true) {
        setVolume(getVolume());
        setBrightness(restoreBrightness());
    }
    QWidget::setVisible(visible);
}

void QuickBar::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    this->hide();
}

void QuickBar::onBrightnessChanged(qreal value)
{
#ifdef ANDROID
    setBrightnessJNI((int) value);
#else
    setBrightness(value);
#endif
    saveBrightness(value);
    emit brightnessChanged(value);
}

void QuickBar::onVolumeChanged(qreal value)
{
    setVolume(value);
    saveVolume(value);
    emit volumeChanged(value);
}

void QuickBar::on_backPushButton_released()
{
    emit backPushButtonReleased();
}

void QuickBar::on_menuButton_released()
{
    emit menuButtonReleased();
}

void QuickBar::on_boltJointButton_released()
{
    emit boltJointButtonReleased();
}

void QuickBar::on_notesButton_released()
{
    emit notesButtonReleased();
}

void QuickBar::on_railTypeButton_released()
{
    emit railTypeButtonReleased();
}

void QuickBar::on_handButton_released()
{
    emit handButtonReleased();
}

void QuickBar::on_pauseButton_released()
{
    emit pauseButtonReleased();
}

void QuickBar::on_scanButton_released()
{
    emit scanButtonReleased();
}

void QuickBar::on_showHideControlsButton_released()
{
    emit showHideControlsButtonReleased();
}

void QuickBar::on_btWireOutputButton_released()
{
    emit btWireOutputSetted();
}

void QuickBar::on_wireOutputButton_released()
{
    emit wireOutputSetted();
}

void QuickBar::on_playMediaButton_released()
{
    Core::instance().playSound(ACNotify);
}

void QuickBar::on_wifiPageButton_released()
{
    emit wifiButtonReleased();
}

void QuickBar::on_bluetoothPageButton_released()
{
    emit bluetoothButtonReleased();
}

void QuickBar::on_crashButton_released()
{
    int* x = Q_NULLPTR;
    *x = 10;
}
