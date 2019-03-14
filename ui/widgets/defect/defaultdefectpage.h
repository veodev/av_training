#ifndef DEFAULTDEFECTPAGE_H
#define DEFAULTDEFECTPAGE_H

#include "defectpage.h"

namespace Ui
{
class DefaultDefectPage;
}

class DefaultDefectPage : public DefectPage
{
    Q_OBJECT

public:
    explicit DefaultDefectPage(QWidget* parent = 0);
    ~DefaultDefectPage();

private slots:
    void on_codeEditButton_released();
    void on_linkEditButton_released();
    void on_serviceMarksDefectChangeCommentButton_released();
    void on_serviceMarksDefectSetButton_released();

private:
    Ui::DefaultDefectPage* ui;

    void setDefectCode();
    void setLink();
    void setComment();
    // DefectPage interface
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
};

#endif  // DEFAULTDEFECTPAGE_H
