#ifndef LINESELECTIONWIDGET_H
#define LINESELECTIONWIDGET_H

#include <QWidget>
#include "ftp/ftpclient.h"


namespace Ui
{
class LineSelectionWidget;
}

class LineSelectionWidget : public QWidget
{
    Q_OBJECT

    QWidget* _parentWidget;

public:
    explicit LineSelectionWidget(QWidget* parent = 0);
    ~LineSelectionWidget();

    void popup();

protected:
    bool event(QEvent* e);

private slots:
    void on_okLineButton_released();
    void on_addLineButton_released();
    void on_editLineButton_released();
    void on_deleteLineButton_released();
    void onDeleteDialogExited(bool status);
    void on_closeButton_released();

signals:
    void lineSelected(QString line, bool isControlSection);
    void lineDeleted(QString line);

private:
    Ui::LineSelectionWidget* ui;
    void insertControlSectionInLineListWidget();
    void translateFirstItemLineListWidget();
    void addNewLine();
    void editLine();
    void deleteLine();
};

#endif  // LINESELECTIONWIDGET_H
