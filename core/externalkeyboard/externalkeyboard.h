#ifndef EXTERNALKEYBOARD_H
#define EXTERNALKEYBOARD_H

#include <QObject>
#include <QTimer>
#include <QList>

class GpioInput;

class ExternalKeyboard : public QObject
{
    Q_OBJECT
public:
    explicit ExternalKeyboard(QObject* parent = 0);
    ~ExternalKeyboard();

signals:
    void toggled(bool isPressed, int keyCode);

public slots:
    void tick();

private:
    QTimer _timer;

    struct GpioKey
    {
        GpioInput* gpio;
        bool isPressedPrev;
        int keyCode;
        ~GpioKey();
    };
    std::vector<GpioKey*> _keys;
};

#endif  // EXTERNALKEYBOARD_H
