#ifndef SETDATE_H
#define SETDATE_H

#include <QWidget>

namespace Ui
{
class SetDate;
}

class SetDate : public QWidget
{
    Q_OBJECT

public:
    explicit SetDate(QWidget* parent = 0);
    ~SetDate();

signals:
    void leaveTheWidget();
    void dateChanged();

public slots:
    void setVisible(bool visible);

protected:
    bool event(QEvent* e);

private:
    bool runProcess(const QString& program);

private slots:
    void on_okButton_released();
    void on_prevMonthPushButton_released();
    void on_nextMonthPushButton_released();
    void on_prevYearPushButton_released();
    void on_nextYearPushButton_released();

    void onCurrentPageChanged(int year, int month);

private:
    Ui::SetDate* ui;
};

#endif  // SETDATE_H
