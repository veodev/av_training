#ifndef BOLTJOINTOPTIONS_H
#define BOLTJOINTOPTIONS_H

#include <QWidget>

namespace Ui
{
class BoltJointOptions;
}

class BoltJointOptions : public QWidget
{
    Q_OBJECT

public:
    explicit BoltJointOptions(QWidget* parent = 0);
    ~BoltJointOptions();

protected:
    void retranslateUi();
    bool event(QEvent* e);
private slots:
    void onAdditiveSensChanged(qreal value);
    void onIncreaseTracks(int index, const QString& value, const QVariant& userData);
signals:
    void additiveSensChanged(int value);
    void increaseTracks(bool value);

private:
    Ui::BoltJointOptions* ui;
};

#endif  // BOLTJOINTOPTIONS_H
