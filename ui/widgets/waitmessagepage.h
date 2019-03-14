#ifndef WAITMESSAGEPAGE_H
#define WAITMESSAGEPAGE_H

#include <QWidget>
#include <QTimer>

namespace Ui
{
class WaitMessagePage;
}

class WaitMessagePage : public QWidget
{
    Q_OBJECT

public:
    explicit WaitMessagePage(QWidget* parent = 0);
    ~WaitMessagePage();

    void setText(const QString& text);
    void startProcessTimer(int msec);
    void stopProcessTimer();
    void setClicked(bool enableClick);

signals:
    void timeIsOut();
    void clickWidget();

protected:
    bool event(QEvent* e);
    void mousePressEvent(QMouseEvent* e);

private slots:
    void onStepTimerTimeout();
    void onProcessTimerTimeout();

private:
    Ui::WaitMessagePage* ui;
    QTimer* _processTimer;
    QTimer* _stepTimer;
    int _index;
    bool _enableClick;
    QString _message;
};

#endif  // WAITMESSAGEPAGE_H
