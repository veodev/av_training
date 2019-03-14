#ifndef SPINBOXBASE_H
#define SPINBOXBASE_H

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>

namespace Ui
{
class SpinBoxBase;
}

class SpinBoxBase : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString caption READ caption WRITE setCaption)
    Q_PROPERTY(bool enableCaption READ isCaptionEnabled WRITE enableCaption)

public:
    explicit SpinBoxBase(QWidget* parent = 0);
    virtual ~SpinBoxBase();

    void setCaption(const QString& caption);
    QString caption() const;

    void enableCaption(bool enable = true);
    bool isCaptionEnabled() const;

    void setCaptionFontSize(int value);
    void setValueFontSize(int value);

    void setColor(const QColor& color);

    Ui::SpinBoxBase* ui;

protected:
    virtual void displayValue() = 0;
    virtual void nextValue() = 0;
    virtual void prevValue() = 0;
    virtual bool isMinimumReached();
    virtual bool isMaximumReached();
    void mousePressEvent(QMouseEvent* e);

protected:
    QString _suffix;
    QTimer _timer;

    unsigned char _timerInterval;

private:
    void timerStop();

private slots:
    void onLeft();
    void onRight();
    void on_leftButton_pressed();
    void on_leftButton_released();
    void on_rightButton_pressed();
    void on_rightButton_released();
};

#endif  // SPINBOXBASE_H
