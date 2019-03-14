#ifndef EKASUIDEFECTPAGE_H
#define EKASUIDEFECTPAGE_H

#include "defectpage.h"

class EKASUIDefectSelectionWidget;

namespace Ui
{
class EKASUIDefectPage;
}

class EKASUIDefectPage : public DefectPage
{
    Q_OBJECT

    EKASUIDefectSelectionWidget* _defectSelectionWidget;

public:
    explicit EKASUIDefectPage(QWidget* parent = 0);
    ~EKASUIDefectPage();

private:
    bool _inited;
    Ui::EKASUIDefectPage* ui;
    void changeServiceMarksDefectComment();
    void setEKASUILink();
    void changeServiceMarksDefectCode();

    void init();

public:
    int getSide();
    int getDefectLength();
    int getDefectWidth();
    int getDefectDepth();
    int getSpeedLimit();
    QString getDefectCode();
    QString getLink();
    QString getComment();
    void clearFields();

public slots:
    void setVisible(bool visible);

private slots:
    void on_serviceMarksDefectChangeCommentButton_released();
    void on_linkEditButton_released();
    void on_serviceMarksDefectSetButton_released();
    void on_defectCodePushButton_released();
    void onDefectSelected(QString defect);
    void onDefectSelectorClosed();
};

#endif  // EKASUIDEFECTPAGE_H
