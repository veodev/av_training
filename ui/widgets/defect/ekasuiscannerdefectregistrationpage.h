#ifndef EKASUISCANNERDEFECTREGISTRATIONPAGE_H
#define EKASUISCANNERDEFECTREGISTRATIONPAGE_H

#include <QWidget>
#include "scannerdefectregistrationpage.h"

class EKASUIDefectSelectionWidget;

namespace Ui
{
class EKASUIScannerDefectRegistrationPage;
}

class EKASUIScannerDefectRegistrationPage : public ScannerDefectRegistrationPage
{
    Q_OBJECT

    EKASUIDefectSelectionWidget* _defectSelectionWidget;

public:
    explicit EKASUIScannerDefectRegistrationPage(QWidget* parent = 0);
    ~EKASUIScannerDefectRegistrationPage();
    void switchState(int state);
private slots:
    void onRuLocationSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void on_headScannerDefectChangeCommentButton_released();
    void on_ekasuiChangeHeadLinkButton_released();
    void on_controledRailHeadScannerSpinBox_valueChanged(int value);
    void on_workEdgeHeadScannerSpinBox_valueChanged(int value);
    void on_saveHeadScanResultButton_released();
    void on_nextHeadScanButton_released();
    void on_startWorkHeadScanButton_released();
    void onDefectSeleced(QString defect);
    void onDefectSelectorClosed();
    void on_ekasuiDefectCodeSelectionButton_released();

private:
    Ui::EKASUIScannerDefectRegistrationPage* ui;
    void retranslateRegistrationPage();
    void changeLSizeHeadScanner();
    void changeHSizeHeadScanner();
    void changeHeadScannerDefectComment();
    void setEKASUIHeadLink();
    void setDHead();

public:
    int getWorkRail();
    int getEdge();
    int getHSize();
    int getLSize();
    int getDSize();
    int getSpeedLimit();
    QString getDefectCode();
    QString getLink();
    QString getComment();
    void resetPage();
};

#endif  // EKASUISCANNERDEFECTREGISTRATIONPAGE_H
