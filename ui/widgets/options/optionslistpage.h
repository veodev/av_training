#ifndef OPTIONSLISTPAGE_H
#define OPTIONSLISTPAGE_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class OptionsListPage;
}

class OptionsListPage : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsListPage(QWidget *parent = nullptr);
    ~OptionsListPage();

    void addWidget(QWidget *widget);
    void setItemEnabled(QString objectName, bool isEnabled);
    void setItemVisible(QString objectName, bool isVisible);
    void setItemText(QString objectName, QString text);

private:
    Ui::OptionsListPage *ui;
    QVector<QWidget*> _items;
};

#endif // OPTIONSLISTPAGE_H
