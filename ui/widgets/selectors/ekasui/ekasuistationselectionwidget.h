#ifndef EKASUISTATIONSELECTIONWIDGET_H
#define EKASUISTATIONSELECTIONWIDGET_H

#include <QWidget>

namespace Ui
{
class EKASUIStationSelectionWidget;
}

class EKASUIStationSelectionWidget : public QWidget
{
    Q_OBJECT
    QWidget* _parentWidget;

public:
    explicit EKASUIStationSelectionWidget(QWidget* parent = 0);
    ~EKASUIStationSelectionWidget();
    void popup();
    void setStationList(const QStringList& list);
signals:
    void stationSelected(QString);
    void closeWidget();
private slots:
    void on_okSelectionButton_released();

    void on_cancelButton_released();

private:
    Ui::EKASUIStationSelectionWidget* ui;
};

#endif  // EKASUISTATIONSELECTIONWIDGET_H
