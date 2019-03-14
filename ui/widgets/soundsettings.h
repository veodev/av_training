#ifndef SOUNDSETTINGS_H
#define SOUNDSETTINGS_H

#include <QWidget>

namespace Ui
{
class SoundSettings;
}

class AlarmTone;

class SoundSettings : public QWidget
{
    Q_OBJECT

public:
    explicit SoundSettings(QWidget* parent = 0);
    ~SoundSettings();

public slots:
    void setVisible(bool visible);
    void setVolume(qreal value);

protected:
    bool event(QEvent* e);

private slots:
    void volumeChanged(qreal volume);
    void on_test500HzFlashLeftButton_released();
    void on_test500HzFlashRightButton_released();
    void on_test500HzLeftButton_released();
    void on_test500HzRightButton_released();
    void on_test1000HzLeftButton_released();
    void on_test1000HzRightButton_released();
    void on_test2000HzLeftButton_released();
    void on_test2000HzRightButton_released();
    void on_test2000HzFlashLeftButton_released();
    void on_test2000HzFlashRightButton_released();
    void on_testNotificationsSoundButton_released();

private:
    void unpressAllButtons();

private:
    Ui::SoundSettings* ui;
    AlarmTone* _alartTone;
};

#endif  // SOUNDSETTINGS_H
