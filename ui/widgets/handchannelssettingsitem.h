#ifndef HANDCHANNELSSETTINGSITEM_H
#define HANDCHANNELSSETTINGSITEM_H

#include <QWidget>
#include "modeltypes.h"

namespace Ui
{
class HandChannelsSettingsItem;
}

class HandChannelsSettingsItem : public QWidget
{
    Q_OBJECT

public:
    explicit HandChannelsSettingsItem(QWidget* parent = nullptr);
    ~HandChannelsSettingsItem();

    friend class HandChannelsSettingsView;

    void setAngle(int angle);
    void setFrequency(double frequency);
    void setMethod(InspectMethod method);
    void setTimeDelay(double timeDelay);
    void setSensAndRecommendedSens(int sens, int recommendedSens);
    void setGain(int gain);
    void setTvg(int tvg);
    void setGate(int startGate, int endGate);
    void setItemBackgroundColor(int itemIndex);

private:
    Ui::HandChannelsSettingsItem* ui;
};

#endif  // HANDCHANNELSSETTINGSITEM_H
