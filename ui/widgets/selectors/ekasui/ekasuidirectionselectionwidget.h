#ifndef EKASUIDIRECTIONSELECTIONWIDGET_H
#define EKASUIDIRECTIONSELECTIONWIDGET_H

#include <QWidget>

namespace Ui
{
class EKASUIDirectionSelectionWidget;
}

class EKASUIDirectionSelectionWidget : public QWidget
{
    Q_OBJECT

    QWidget* _parentWidget;

public:
    explicit EKASUIDirectionSelectionWidget(QWidget* parent = 0);
    ~EKASUIDirectionSelectionWidget();
    void setDirectionsList(const QStringList& list);
    void popup();
private slots:
    void on_okSelectionButton_released();
    void on_cancelButton_released();
signals:
    void directionSelected(QString);
    void closeWidget();

private:
    Ui::EKASUIDirectionSelectionWidget* ui;
};

#endif  // EKASUIDIRECTIONSELECTIONWIDGET_H
