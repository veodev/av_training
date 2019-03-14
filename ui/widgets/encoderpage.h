#ifndef ENCODERPAGE_H
#define ENCODERPAGE_H

#include <QWidget>
#include <QSharedPointer>

class tDEV_BScan2Head;

namespace Ui
{
class EncoderPage;
}

class QElapsedTimer;

class EncoderPage : public QWidget
{
    Q_OBJECT

public:
    explicit EncoderPage(QWidget* parent = nullptr);
    ~EncoderPage();

    void resetCountStepsForOneMeter();
    void resetCoord();

protected:
    bool event(QEvent* e);

signals:
    void changedSpeed(double value);
    void changedDistance(int value);

private:
    void setupUi();
    void retranslateUi();

    void setSpeed(float value);
    void setDistance();
    void setCountStepsForOneMeter(float value);
    void restoreStepLength();

private slots:
    void onBScanData(QSharedPointer<tDEV_BScan2Head>);
    void onDeviceSpeed(unsigned int);
    void stepChanged(qreal value);
    void on_resetDistanceButton_released();

private:
    bool _isInvert;

    int _countStepsForOneMeter;
    int _countStepsForOneMeterConst;

    float _lastSpeed;
    float _stepLength;

    long int _distance;
    long int _prevSysCoordinat;
    long int _lastSession;

    Ui::EncoderPage* ui;
};

#endif  // ENCODERPAGE_H
