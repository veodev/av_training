#ifndef SCANNERENCODERSPAGE_H
#define SCANNERENCODERSPAGE_H

#include <QWidget>
#include <QSharedPointer>

class tDEV_PathStepData;
class tDEV_BScan2Head;

namespace Ui
{
class ScannerEncodersPage;
}

class ScannerEncodersPage : public QWidget
{
    Q_OBJECT

public:
    explicit ScannerEncodersPage(QWidget* parent = 0);
    ~ScannerEncodersPage();

    void resetEncodersDistances();

protected:
    bool event(QEvent* e);

private:
    void setupUi();
    void retranslateUi();

    void setHorizontalDistance();
    void setVerticalDistance();

private slots:
    void on_resetHorizontalDistanceButton_released();
    void on_resetVerticalDistanceButton_released();
    void onHorizontalSpinBoxChanged(qreal step);
    void onVerticalSpinBoxChanged(qreal step);
    void onPathStepData(QSharedPointer<tDEV_PathStepData>);
    void onBScanData(QSharedPointer<tDEV_BScan2Head>);

private:
    Ui::ScannerEncodersPage* ui;

    long int _horizontalDistance;
    long int _verticalDistance;
    long int _prevHorizontalSysCoordinat;
    long int _prevVerticalSysCoordinat;
    double _horizontalStep;
    double _verticalStep;
};

#endif  // SCANNERENCODERSPAGE_H
