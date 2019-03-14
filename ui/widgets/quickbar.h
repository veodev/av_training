#ifndef QUICKBAR_H
#define QUICKBAR_H

#include <QWidget>

namespace Ui
{
class QuickBar;
}

class QuickBar : public QWidget
{
    Q_OBJECT

public:
    explicit QuickBar(QWidget* parent = 0);
    ~QuickBar();

    void setVolume(qreal value);
    void setBrightness(qreal value);

signals:
    void volumeChanged(qreal volume);
    void brightnessChanged(qreal volume);

    void backPushButtonReleased();
    void menuButtonReleased();
    void boltJointButtonReleased();
    void notesButtonReleased();
    void railTypeButtonReleased();
    void handButtonReleased();
    void pauseButtonReleased();
    void scanButtonReleased();
    void showHideControlsButtonReleased();

    void wireOutputSetted();
    void btWireOutputSetted();

    void bluetoothButtonReleased();
    void wifiButtonReleased();

public slots:
    void setVisible(bool visible);

protected:
    void mousePressEvent(QMouseEvent* e);

private:
    Ui::QuickBar* ui;

private slots:
    void onBrightnessChanged(qreal value);
    void onVolumeChanged(qreal value);

    void on_backPushButton_released();
    void on_menuButton_released();
    void on_boltJointButton_released();
    void on_notesButton_released();
    void on_railTypeButton_released();
    void on_handButton_released();
    void on_pauseButton_released();
    void on_scanButton_released();
    void on_showHideControlsButton_released();
    void on_btWireOutputButton_released();
    void on_wireOutputButton_released();
    void on_playMediaButton_released();
    void on_wifiPageButton_released();
    void on_bluetoothPageButton_released();
    void on_crashButton_released();
};

#endif  // QUICKBAR_H
