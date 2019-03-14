#ifndef SCANTAPEACOUSTICCONTACTITEM_H
#define SCANTAPEACOUSTICCONTACTITEM_H

#include <QWidget>
#include <QTimer>

#include "scanlateralpanel.h"
#include "scantapeacousticcontactview.h"

namespace Ui
{
class ScanTapeAcousticContactItem;
}

class ScanTapeAcousticContactItem : public QWidget
{
    Q_OBJECT

public:
    explicit ScanTapeAcousticContactItem(QWidget* parent = 0);
    ~ScanTapeAcousticContactItem();

    void showBottomWidget();
    void hideBottomWidget();

    void hideBottomWidget(bool isHide);
    void setSide(LateralPanelType side);
    void setAcousticContactState(AcousticContactSegment segment, bool isAcousticContact);

    void updateState(bool isAcousticContact);
    bool prevState() const;

private:
    Ui::ScanTapeAcousticContactItem* ui;
    bool _prevState;
    LateralPanelType _side;
};

#endif  // SCANTAPEACOUSTICCONTACTITEM_H
