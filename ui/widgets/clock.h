#ifndef DATETIME_H
#define DATETIME_H

#include <QLCDNumber>


class Clock : public QLCDNumber
{
    Q_OBJECT

public:
    explicit Clock(QWidget* parent = nullptr);
    virtual ~Clock();
private slots:
    void update();

private:
    bool _even;
    QTimer* _timer;
};

#endif  // DATETIME_H
