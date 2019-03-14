#ifndef SYSTEMPASSWORDFORM_H
#define SYSTEMPASSWORDFORM_H

#include <QWidget>

namespace Ui
{
class SystemPasswordForm;
}

class SystemPasswordForm : public QWidget
{
    Q_OBJECT

public:
    explicit SystemPasswordForm(QWidget* parent = 0);
    ~SystemPasswordForm();

    void changePassButton(bool change);

private slots:
    void on_pushButton_released();

    void on_pushButton_2_released();

    void on_pushButton_3_released();

    void on_pushButton_4_released();

    void on_pushButton_5_released();

    void on_pushButton_6_released();

    void on_pushButton_7_released();

    void on_pushButton_8_released();

    void on_pushButton_9_released();

    void on_pushButton_zero_released();

    void on_clearButton_released();

    void on_enterButton_released();

private:
    Ui::SystemPasswordForm* ui;

    bool _change;
};

#endif  // SYSTEMPASSWORDFORM_H
