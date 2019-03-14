#ifndef SWITCHOPERATORWIDGET_H
#define SWITCHOPERATORWIDGET_H

#include <QWidget>

namespace Ui
{
class SwitchOperatorWidget;
}

class SwitchOperatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SwitchOperatorWidget(QWidget* parent = 0);
    ~SwitchOperatorWidget();

public slots:
    void setOperatorsList(QStringList operators);
    void setActiveOperator(QString name);

signals:
    void operatorSelected(QString);
    void closeWidget();

private slots:
    void on_okOperatorButton_released();
    void on_cancelPushButton_released();

private:
    Ui::SwitchOperatorWidget* ui;
    QString _currentOperator;
};

#endif  // SWITCHOPERATORWIDGET_H
