#ifndef EKASUITRACKNUMBERSELECTIONWIDGET_H
#define EKASUITRACKNUMBERSELECTIONWIDGET_H

#include <QWidget>

namespace Ui
{
class EKASUITrackNumberSelectionWidget;
}

class EKASUITrackNumberSelectionWidget : public QWidget
{
    Q_OBJECT
    QWidget* _parentWidget;

public:
    explicit EKASUITrackNumberSelectionWidget(QWidget* parent = 0);
    ~EKASUITrackNumberSelectionWidget();
    void popup();
    void setTrackNumbersList(const QStringList& list);
private slots:
    void on_okSelectionButton_released();
    void on_cancelButton_released();
signals:
    void trackNumberSelected(QString);
    void closeWidget();

private:
    Ui::EKASUITrackNumberSelectionWidget* ui;
};

#endif  // EKASUITRACKNUMBERSELECTIONWIDGET_H
