#ifndef OPERATORSELECTIONWIDGET_H
#define OPERATORSELECTIONWIDGET_H

#include <QWidget>
#include <QListWidget>

namespace Ui
{
class OperatorSelectionWidget;
}

class OperatorSelectionWidget : public QWidget
{
    Q_OBJECT

    QWidget* _parentWidget;

public:
    explicit OperatorSelectionWidget(QWidget* parent = 0);
    ~OperatorSelectionWidget();

    QListWidgetItem* getCurrentItem();
    QList<QListWidgetItem*> getSelectedItems();
    int getOperatorsCount();
    QStringList getAllOperators();
    void popup();

protected:
    bool event(QEvent* e);

private slots:
    void addNewOperator();
    void editOperator();
    void deleteOperator();
    void on_addOperatorButton_released();
    void on_deleteOperatorButton_released();
    void on_editOperatorButton_released();
    void on_okOperatorButton_released();

    void onDeleteDialogExited(bool status);
    void on_closeButton_released();

signals:
    void operatorSelected(QString operatorName);
    void operatorDeleted(QString operatorName);

private:
    Ui::OperatorSelectionWidget* ui;
};

#endif  // OPERATORSELECTIONWIDGET_H
