#ifndef TESTEXTERNALKEYBOARDWIDGET_H
#define TESTEXTERNALKEYBOARDWIDGET_H

#include <QWidget>

namespace Ui {
class TestExternalKeyboardWidget;
}

class TestExternalKeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestExternalKeyboardWidget(QWidget *parent = 0);
    ~TestExternalKeyboardWidget();

signals:
    void testTrackMarkButtonPressed();
    void testServiceMarkButtonPressed();
    void testBoltJointButtonPressed();
    void testBoltJointButtonReleased();

private slots:
    void on_trackMarkButton_released();
    void on_serviceMarkButton_released();
    void on_boltJointButton_released();
    void on_boltJointButton_pressed();

private:
    Ui::TestExternalKeyboardWidget *ui;
};

#endif // TESTEXTERNALKEYBOARDWIDGET_H
