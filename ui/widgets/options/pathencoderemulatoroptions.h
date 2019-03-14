#ifndef PATHENCODEREMULATOROPTIONS_H
#define PATHENCODEREMULATOROPTIONS_H

#include <QWidget>

namespace Ui
{
class PathEncoderEmulatorOptions;
}

class PathEncoderEmulatorOptions : public QWidget
{
    Q_OBJECT

public:
    explicit PathEncoderEmulatorOptions(QWidget* parent = 0);
    ~PathEncoderEmulatorOptions();

private slots:
    void onEnabledChanged(int index, const QString& value, const QVariant& userData);

private:
    Ui::PathEncoderEmulatorOptions* ui;
};

#endif  // PATHENCODEREMULATOROPTIONS_H
