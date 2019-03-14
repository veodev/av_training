#ifndef TABLESETTINGSITEM_H
#define TABLESETTINGSITEM_H

#include <QWidget>
#include <QtGui>
#include <QtWidgets>

namespace Ui
{
class TableSettingsItem;
}

class TableSettingsItem : public QWidget
{
    Q_OBJECT

public:
    explicit TableSettingsItem(bool isBothRail = true, QWidget* parent = 0);
    ~TableSettingsItem();

    friend class TableSettingsView;

    void setChannelName(const QString& channelName);
    void setChannelNameSingle(const QString& channelName);

    void setLeftTimeDelay(double timeDelay);
    void setRightTimeDelay(double timeDelay);
    void setTimeDelay(double timeDelay);

    void setLeftSensAndRecommendedSens(int sens, int recommendedSens);
    void setRightSensAndRecommendedSens(int sens, int recommendedSens);
    void setSensAndRecommendedSens(int sens, int recommendedSens);

    void setLeftGain(int gain);
    void setRightGain(int gain);
    void setGain(int gain);

    void setLeftTvg(int tvg);
    void setRightTvg(int tvg);
    void setTvg(int tvg);

    void setLeftGate(int startGate, int endGate);
    void setRightGate(int startGate, int endGate);
    void setGate(int startGate, int endGate);

    void setItemBackgroundColor(int itemIndex);

private:
    Ui::TableSettingsItem* ui;
};

#endif  // TABLESETTINGSITEM_H
