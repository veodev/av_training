#ifndef EKASUIOPTIONS_H
#define EKASUIOPTIONS_H

#include <QWidget>

namespace Ui
{
class EKASUIOptions;
}

class EKASUIOptions : public QWidget
{
    Q_OBJECT

public:
    explicit EKASUIOptions(QWidget* parent = 0);
    ~EKASUIOptions();
    void enableSettingsEditing(bool enable);

private:
    void setCarId();


private slots:
    void onrr_currentIndexChanged(const QString& arg1);
    void onpred_currentIndexChanged(const QString& arg1);

    void on_updateDBButton_released();
    void onEKASUIModeChanged(int index, const QString& value, const QVariant& userData);

    void on_IntegrateMode_valueChanged();
    void on_carIdPushButton_released();
public slots:
    void setVisible(bool visible);
signals:
    void ekasuiChanged();

private:
    bool _inited;
    Ui::EKASUIOptions* ui;
    QString getFlashPath();
    void init();
};

#endif  // EKASUIOPTIONS_H
