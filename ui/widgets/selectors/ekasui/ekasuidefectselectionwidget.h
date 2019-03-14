#ifndef EKASUIDEFECTSELECTIONWIDGET_H
#define EKASUIDEFECTSELECTIONWIDGET_H

#include <QWidget>

namespace Ui
{
class EKASUIDefectSelectionWidget;
}

class EKASUIDefectSelectionWidget : public QWidget
{
    Q_OBJECT
    QWidget* _parentWidget;

public:
    explicit EKASUIDefectSelectionWidget(QWidget* parent = 0);
    ~EKASUIDefectSelectionWidget();

    void popup();
    void setDefectsList(const QStringList& list);
private slots:
    void on_okSelectionButton_released();
    void on_cancelButton_released();
signals:
    void defectSelected(QString);
    void closeWidget();

private:
    Ui::EKASUIDefectSelectionWidget* ui;
};

#endif  // EKASUIDEFECTSELECTIONWIDGET_H
