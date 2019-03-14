#ifndef DEFAULTSCANNERDEFECTREGISTRATIONPAGE_H
#define DEFAULTSCANNERDEFECTREGISTRATIONPAGE_H

#include "scannerdefectregistrationpage.h"

namespace Ui
{
class DefaultScannerDefectRegistrationPage;
}

class DefaultScannerDefectRegistrationPage : public ScannerDefectRegistrationPage
{
    Q_OBJECT

public:
    explicit DefaultScannerDefectRegistrationPage(QWidget* parent = 0);
    ~DefaultScannerDefectRegistrationPage();

private:
    Ui::DefaultScannerDefectRegistrationPage* ui;
    void changeComment();
    void setLink();
    void setDefectCode();

    // ScannerDefectRegistrationPage interface
    void retranslateRegistrationPage();

public:
    void switchState(int state);
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

private slots:
    void on_defectCodeEditButton_released();
    void on_saveHeadScanResultButton_released();
    void on_nextHeadScanButton_released();
    void on_startWorkHeadScanButton_released();
    void on_changeCommentButton_released();
    void on_changeLinkButton_released();
    void onRuLocationSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void on_controledRailSpinBox_valueChanged(int value);
};

#endif  // DEFAULTSCANNERDEFECTREGISTRATIONPAGE_H
