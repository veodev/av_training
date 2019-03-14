#ifndef POPUPDIALOGWIDGET_H
#define POPUPDIALOGWIDGET_H

#include <QWidget>

namespace Ui
{
class PopupDialogWidget;
}

class PopupDialogWidget : public QWidget
{
    Q_OBJECT

    QWidget* parentWidget;
    QString message;

public:
    explicit PopupDialogWidget(QWidget* parent = 0);
    ~PopupDialogWidget();
    void popup();

    QString getMessage() const;
    void setMessage(const QString& value);
signals:
    void dialogExited(bool status);


private slots:
    void on_yesButton_released();

    void on_cancelButton_released();

private:
    Ui::PopupDialogWidget* ui;
};

#endif  // POPUPDIALOGWIDGET_H
