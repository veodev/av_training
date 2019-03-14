#ifndef SENSAUTORESET_H
#define SENSAUTORESET_H

#include <QWidget>

namespace Ui
{
class SensAutoReset;
}

class SensAutoReset : public QWidget
{
    Q_OBJECT

public:
    explicit SensAutoReset(QWidget* parent = 0);
    ~SensAutoReset();

protected:
    bool event(QEvent* e);

private:
    void retranslateUi();

public slots:
    void onAutoSensResetEnabledChanged(int index, const QString& value, const QVariant& userData);
    void onAutoSensResetDelayChanged(qreal value);

private:
    Ui::SensAutoReset* ui;
};

#endif  // SENSAUTORESET_H
