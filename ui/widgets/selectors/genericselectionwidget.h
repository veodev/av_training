#ifndef GENERICSELECTIONWIDGET_H
#define GENERICSELECTIONWIDGET_H

#include <QWidget>

namespace Ui
{
class GenericSelectionWidget;
}

class GenericSelectionWidget : public QWidget
{
    Q_OBJECT

    QWidget* _parentWidget;

public:
    explicit GenericSelectionWidget(QWidget* parent = 0);
    ~GenericSelectionWidget();
    void popup();
    void setValues(const QString& title, const std::vector<std::pair<QString, int>>& valuesWithId);
signals:
    void selected(QString value, int id);

private slots:
    void on_okButton_released();

    void on_closeButton_released();

private:
    Ui::GenericSelectionWidget* ui;
    std::vector<std::pair<QString, int>> _valuesWithId;
};

#endif  // GENERICSELECTIONWIDGET_H
