#ifndef ELECTRICTESTPAGE_H
#define ELECTRICTESTPAGE_H

#include <QWidget>
#include <QPlainTextEdit>

namespace Ui
{
class ElectricTestPage;
}

class ElectricTestPage : public QWidget
{
    Q_OBJECT

public:
    explicit ElectricTestPage(QWidget* parent = 0);
    ~ElectricTestPage();

    void setCountCrashes(int count);
    void setCountCduReboot(int count);
    void setCountUmuDisconnections(int count);
    QPlainTextEdit* getViewObject();

    void startElectricTestAfterCrash();
    void stopElectricTest();

signals:
    void doStartElTest();
    void doStopElTest();

protected:
    bool event(QEvent* e);

private slots:
    void on_startElTestButton_released();
    void on_stopElTestButton_released();
    void on_closePageButton_released();

private:
    Ui::ElectricTestPage* ui;

    QPlainTextEdit* _textEdit;
};

#endif  // ELECTRICTESTPAGE_H
