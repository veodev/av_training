#include "mscanpage.h"
#include "ui_mscanpage.h"

#include "channelscontroller.h"
#include "debug.h"
#include "core.h"

MScanPage::MScanPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MScanPage)
    , _scheme(-1)
    , _isEntered(false)
{
    ui->setupUi(this);
    setScheme(-1);
    ASSERT(connect(&Core::instance(), &Core::doSchemeChanged, this, &MScanPage::onSchemeChanged));
}

MScanPage::~MScanPage()
{
    stopDrawing();
    delete ui;
}

void MScanPage::onSchemeChanged(int index)
{
    ui->mScanPlot->resetAllPoints();
    setScheme(index);
}

void MScanPage::startDrawing()
{
    stopDrawing();

    Core& core = Core::instance();
    ASSERT(connect(&core, &Core::doBScan2Data, this, &MScanPage::onBScan2Data, Qt::UniqueConnection));
    ASSERT(connect(&core, &Core::doMScan2Data, this, &MScanPage::onBScan2Data, Qt::UniqueConnection));
    ASSERT(connect(&core, &Core::doAlarmData, this, &MScanPage::onAlarmData, Qt::UniqueConnection));
}

void MScanPage::stopDrawing()
{
    Core& core = Core::instance();
    disconnect(&core, &Core::doBScan2Data, this, &MScanPage::onBScan2Data);
    disconnect(&core, &Core::doMScan2Data, this, &MScanPage::onBScan2Data);
    disconnect(&core, &Core::doAlarmData, this, &MScanPage::onAlarmData);
}

void MScanPage::setDotSize(int value)
{
    ui->mScanPlot->setRadiusOfPoint(value);
}

void MScanPage::setVisible(bool visible)
{
    if (visible == true) {
        _isEntered = true;
        startDrawing();
    }
    else {
        _isEntered = false;
        stopDrawing();
    }

    QWidget::setVisible(visible);
}

void MScanPage::onBScan2Data(QSharedPointer<tDEV_BScan2Head> head)
{
    if (!isVisible()) {
        return;
    }
    Q_ASSERT(head.data() != 0);

    int evaluationGateLevel = Core::evaluationGateLevel();
    ChannelsController* channelsController = Core::instance().channelsController();
    for (const tDEV_BScan2HeadItem& item : head.data()->Items) {
        eDeviceSide side = item.Side;
        int cid = item.Channel;
        const QModelIndex& firstGateIndex = channelsController->modelIndexByCidSideAndGateIndex(cid, side, 0);
        const QModelIndex& secondGateIndex = channelsController->modelIndexByCidSideAndGateIndex(cid, side, 1);
        if (firstGateIndex.isValid() == false) {
            continue;
        }
        QAbstractItemModel* model = const_cast<QAbstractItemModel*>(firstGateIndex.model());
        Q_ASSERT(model);

        int delayMultiplyFirst = model->data(firstGateIndex, DelayMultiplyRole).toInt();
        Q_ASSERT(delayMultiplyFirst != 0);

        int startGateFirst = model->data(firstGateIndex, StartGateRole).toInt();
        int endGateFirst = model->data(firstGateIndex, EndGateRole).toInt();

        for (const auto& sign : item.Signals) {
            int delay = sign.Delay / delayMultiplyFirst;

            QModelIndex currentIndex = firstGateIndex;
            if (delay < startGateFirst || delay > endGateFirst) {
                if (secondGateIndex.isValid() == false) {
                    continue;
                }
                int startGateSecond = model->data(secondGateIndex, StartGateRole).toInt();
                int endGateSecond = model->data(secondGateIndex, EndGateRole).toInt();
                if (delay < startGateSecond || delay > endGateSecond) {
                    continue;
                }
                currentIndex = secondGateIndex;
            }

            int amplitude = Core::dbToAmp(sign.Ampl[AmplDBIdx_int] & 0x0f);
            if (amplitude < evaluationGateLevel) {
                continue;
            }

            InspectMethod inspectMethod = static_cast<InspectMethod>(model->data(currentIndex, MethodRole).toInt());
            int enterAngle = model->data(currentIndex, AngleRole).toInt();
            bool isRise = false;
            int strokeDuration = model->data(currentIndex, StrokeDurationRole).toInt();
            qreal position = delay / static_cast<float>(strokeDuration);
            QString id = createId(model, currentIndex, side);
            if (inspectMethod != MirrorShadow) {
                Direction direction = static_cast<Direction>(model->data(currentIndex, DirectionRole).toInt());
                QString idPostfix = "";
                if (enterAngle == 58) {
                    position *= 2;
                    if (position >= 1) {
                        position = 1.0 - (position - 1);
                        idPostfix = "2";
                        if (direction == ForwardDirection) {
                            direction = BackwardDirection;
                        }
                        else if (direction == BackwardDirection) {
                            direction = ForwardDirection;
                        }
                    }
                }

#if defined TARGET_AVICON31
                if ((side == dsLeft && direction == BackwardDirection) || (side == dsRight && direction != BackwardDirection)) {
#else
                if (direction == BackwardDirection) {
#endif
                    isRise = true;
                    position = 1.0 - position;
                }
                else {
                    isRise = false;
                }

                id += idPostfix;
            }
            else {
                if (enterAngle == 55) {
                    eControlZone zone = static_cast<eControlZone>(model->data(currentIndex, ZoneRole).toInt());
                    if (zone == czHeadWork) {
                        isRise = true;
                        position = 1.0 - position;
                    }
                    else if (zone == czHeadUnWork) {
                        isRise = false;
                    }
                }
            }
            ui->mScanPlot->drawPoint(id, position, isRise);
        }
    }
    ui->mScanPlot->updateViewport();
}

void MScanPage::onAlarmData(QSharedPointer<tDEV_AlarmHead> head)
{
    if (!isVisible()) {
        return;
    }
    Q_ASSERT(head.data() != nullptr);

    ChannelsController* channelsController = Core::instance().channelsController();

    for (const tDEV_AlarmHeadItem& item : head.data()->Items) {
        for (unsigned char i = 1; i <= 2; ++i) {
            if (item.State[i]) {
                const QModelIndex& index = channelsController->modelIndexByCidSideAndGateIndex(item.Channel, item.Side, i - 1);
                if (index.isValid()) {
                    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
                    Q_ASSERT(model != nullptr);
                    InspectMethod inspectMethod = static_cast<InspectMethod>(model->data(index, MethodRole).toInt());
                    if (inspectMethod == MirrorShadow) {
                        const QString& id = createId(model, index, item.Side) + "Probe";
                        ui->mScanPlot->drawCross(id);
                    }
                }
            }
        }
    }
    ui->mScanPlot->updateViewport();
}

void MScanPage::setScheme(int index)
{
    if (_scheme == index) {
        return;
    }
    _scheme = index;

    if (_isEntered == true) {
        stopDrawing();
    }
    ui->mScanPlot->resetAllPoints();

    switch (index) {
    case 0:
        ui->mScanPlot->setScheme(QString(":/scheme1.svg"));
        break;
    case 1:
        ui->mScanPlot->setScheme(QString(":/scheme2.svg"));
        break;
    case 2:
        ui->mScanPlot->setScheme(QString(":/scheme3.svg"));
        break;
    default:
        ui->mScanPlot->setScheme(QString(":/scheme_not_found.svg"));
        break;
    }

    if (_isEntered == true) {
        startDrawing();
    }
}

QString MScanPage::createId(QAbstractItemModel* model, const QModelIndex& index, eDeviceSide side)
{
    const QString& channelName = model->data(index, ChannelNameRole).toString();

    if (side == dsLeft) {
        return (channelName + "L");
    }
    else if (side == dsRight) {
        return (channelName + "R");
    }

    return channelName;
}
