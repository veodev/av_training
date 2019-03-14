#ifndef BSCANMOVEMENTCONTROLS_H
#define BSCANMOVEMENTCONTROLS_H

#include <QWidget>

namespace Ui
{
class BScanMovementControls;
}

class BScanMovementControls : public QWidget
{
    Q_OBJECT

public:
    explicit BScanMovementControls(QWidget* parent = 0);
    ~BScanMovementControls();
    void changeMode();
    void setMode(bool mode);
    void enableParallelVideoBrowsing(bool isEnabled);
    void setModeButtonTitle(QString modeTitle);

signals:
    void modeChanged(bool);
    void movementLeft();
    void movementRight();

private slots:
    void on_modeButton_released();
    void on_leftButton_released();
    void on_rightButton_released();

private:
    Ui::BScanMovementControls* ui;
    bool _mode;
    bool _isEnabledParallelVideoBrowsing;
};

#endif  // BSCANMOVEMENTCONTROLS_H
