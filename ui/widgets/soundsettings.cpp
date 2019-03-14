#include <qwt_color_map.h>

#include "soundsettings.h"
#include "ui_soundsettings.h"
#include "debug.h"
#include "settings.h"
#include "core.h"
#include "alarmtone.h"

SoundSettings::SoundSettings(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SoundSettings)
    , _alartTone(nullptr)
{
    ui->setupUi(this);
#if !defined(DEBUG)
    ui->testsWidget->hide();
#endif
    _alartTone = Core::instance().alarmTone();
    Q_ASSERT(_alartTone != nullptr);

    ui->volumeSpinBox->setMinimum(0);
    ui->volumeSpinBox->setMaximum(100);
    ui->volumeSpinBox->setStepBy(1);
    ASSERT(connect(ui->volumeSpinBox, &SpinBoxNumber::valueChanged, this, &SoundSettings::volumeChanged));
}

SoundSettings::~SoundSettings()
{
    delete ui;
}

void SoundSettings::setVolume(qreal value)
{
    volumeChanged(value);
}

void SoundSettings::setVisible(bool visible)
{
    if (visible == true) {
        ui->volumeSpinBox->setValue(_alartTone->volume(), false);
    }
    else {
        _alartTone->stopTest();
        unpressAllButtons();
        _alartTone->setVolume(_alartTone->volume());
    }

    QWidget::setVisible(visible);
}

bool SoundSettings::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void SoundSettings::volumeChanged(qreal volume)
{
    if (_alartTone != nullptr) {
        _alartTone->setVolume(volume);
        saveVolume(volume);
    }
}

void SoundSettings::on_test500HzFlashLeftButton_released()
{
    ui->test500HzFlashLeftButton->isChecked() ? _alartTone->testTone(AudioGenerator::left500HzBlink, true) : _alartTone->testTone(AudioGenerator::left500HzBlink, false);
}

void SoundSettings::on_test500HzFlashRightButton_released()
{
    ui->test500HzFlashRightButton->isChecked() ? _alartTone->testTone(AudioGenerator::right500HzBlink, true) : _alartTone->testTone(AudioGenerator::right500HzBlink, false);
}

void SoundSettings::on_test500HzLeftButton_released()
{
    ui->test500HzLeftButton->isChecked() ? _alartTone->testTone(AudioGenerator::left500Hz, true) : _alartTone->testTone(AudioGenerator::left500Hz, false);
}

void SoundSettings::on_test500HzRightButton_released()
{
    ui->test500HzRightButton->isChecked() ? _alartTone->testTone(AudioGenerator::right500Hz, true) : _alartTone->testTone(AudioGenerator::right500Hz, false);
}

void SoundSettings::on_test1000HzLeftButton_released()
{
    ui->test1000HzLeftButton->isChecked() ? _alartTone->testTone(AudioGenerator::left1000Hz, true) : _alartTone->testTone(AudioGenerator::left1000Hz, false);
}

void SoundSettings::on_test1000HzRightButton_released()
{
    ui->test1000HzRightButton->isChecked() ? _alartTone->testTone(AudioGenerator::right1000Hz, true) : _alartTone->testTone(AudioGenerator::right1000Hz, false);
}

void SoundSettings::on_test2000HzLeftButton_released()
{
    ui->test2000HzLeftButton->isChecked() ? _alartTone->testTone(AudioGenerator::left2000Hz, true) : _alartTone->testTone(AudioGenerator::left2000Hz, false);
}

void SoundSettings::on_test2000HzRightButton_released()
{
    ui->test2000HzRightButton->isChecked() ? _alartTone->testTone(AudioGenerator::right2000Hz, true) : _alartTone->testTone(AudioGenerator::right2000Hz, false);
}

void SoundSettings::on_test2000HzFlashLeftButton_released()
{
    ui->test2000HzFlashLeftButton->isChecked() ? _alartTone->testTone(AudioGenerator::left2000HzBlink, true) : _alartTone->testTone(AudioGenerator::left2000HzBlink, false);
}

void SoundSettings::on_test2000HzFlashRightButton_released()
{
    ui->test2000HzFlashRightButton->isChecked() ? _alartTone->testTone(AudioGenerator::right2000HzBlink, true) : _alartTone->testTone(AudioGenerator::right2000HzBlink, false);
}

void SoundSettings::on_testNotificationsSoundButton_released()
{
    _alartTone->notification();
}

void SoundSettings::unpressAllButtons()
{
    ui->test500HzFlashLeftButton->setChecked(false);
    ui->test500HzFlashRightButton->setChecked(false);
    ui->test500HzLeftButton->setChecked(false);
    ui->test500HzRightButton->setChecked(false);
    ui->test1000HzLeftButton->setChecked(false);
    ui->test1000HzRightButton->setChecked(false);
    ui->test2000HzLeftButton->setChecked(false);
    ui->test2000HzRightButton->setChecked(false);
    ui->test2000HzFlashLeftButton->setChecked(false);
    ui->test2000HzFlashRightButton->setChecked(false);
}
