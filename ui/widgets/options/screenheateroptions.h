#ifndef SCREENHEATEROPTIONS_H
#define SCREENHEATEROPTIONS_H

#include <QWidget>

namespace Ui
{
class ScreenHeaterOptions;
}

class ScreenHeaterOptions : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenHeaterOptions(QWidget* parent = 0);
    ~ScreenHeaterOptions();

public slots:
    void onScreenTemperatureChanged(qreal value);

private slots:
    void onScreenHeaterStatusChanged(int index, const QString& value, const QVariant& userData);
    void onScreenHeaterTempChanged(qreal value);

private:
    Ui::ScreenHeaterOptions* ui;
};

#endif  // SCREENHEATEROPTIONS_H
