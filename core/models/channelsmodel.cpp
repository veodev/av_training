#include "channelsmodel.h"

ChannelsModel::ChannelsModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ChannelsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return static_cast<int>(_data.size());
}

QVariant ChannelsModel::data(const QModelIndex& index, int role) const
{
    size_t currentRow = static_cast<size_t>(index.row());
    if (index.isValid() == false || currentRow >= _data.size()) {
        return QVariant();
    }
    switch (role) {
    case IsAlarmedRole:
        return _data[currentRow].isAlarmed;
    case SideRole:
        return _data[currentRow].side;
    case GateCountRole:
        return _data[currentRow].gateCount;
    case AlarmToneRole:
        return _data[currentRow].alarmTone;
    case IsPlayAlarmToneRole:
        return _data[currentRow].isPlayAlarmTone;
    case ChannelIdRole:
        return _data[currentRow].channelId;
    case ChannelTypeRole:
        return _data[currentRow].channelType;
    case ZoneRole:
        return _data[currentRow].zone;
    case SensRole:
        return _data[currentRow].sens;
    case ChannelNameRole:
        return _data[currentRow].channelName;
    case ChannelTitleRole:
        return _data[currentRow].channelTitle;
    case Qt::DisplayRole:
        return _data[currentRow].sens;
    case GainRole:
        return _data[currentRow].gain;
    case TvgRole:
        return _data[currentRow].tvg;
    case TimeDelayRole:
        return _data[currentRow].prismDelay;
    case StartGateRole:
        return _data[currentRow].startGate;
    case EndGateRole:
        return _data[currentRow].endGate;
    case StrokeDurationRole:
        return _data[currentRow].strokeDuration;
    case AngleRole:
        return _data[currentRow].angle;
    case MarkRole:
        return _data[currentRow].mark;
    case MethodRole:
        return _data[currentRow].method;
    case NoteRole:
        return _data[currentRow].note;
    case RecommendedSensRole:
        return _data[currentRow].recommendedSens;
    case GateIndexRole:
        return _data[currentRow].gateIndex;
    case DirectionRole:
        return _data[currentRow].direction;
    case StartBscangateRole:
        return _data[currentRow].startBscanGate;
    case EndBscangateRole:
        return _data[currentRow].endBscanGate;
    case DefaultStartGateRole:
        return _data[currentRow].defaultStartGate;
    case DefaultEndGateRole:
        return _data[currentRow].defaultEndGate;
    case DefaultGainRole:
        return _data[currentRow].defaultGain;
    case DelayMultiplyRole:
        return _data[currentRow].delayMultiply;
    case AScanScaleRole:
        return _data[currentRow].aScanScale;
    case IsStartGateLocked:
        return _data[currentRow].isStartGateLocked;
    case IsEndGateLocked:
        return _data[currentRow].isEndGateLocked;
    case SensLevelsRole:
        return _data[currentRow].isSensOutRange;
    case FrequencyChannelRole:
        return _data[currentRow].frequency;
    case CalibrationTemperatureRole:
        return _data[currentRow].calibrationTemperature;
    case UseNotchRole:
        return _data[currentRow].isNotchEnable;
    case StartNotchRole:
        return _data[currentRow].startNotch;
    case SizeNotchRole:
        return _data[currentRow].sizeNotch;
    case LevelNotchRole:
        return _data[currentRow].levelNotch;
    case IsCopySlaveChannelRole:
        return _data[currentRow].isCopySlaveChannel;
    case BScanColorIndexRole:
        return _data[currentRow].bscanColorIndex;
    case BScanNormalColorRole:
        return _data[currentRow].bscanColor;
    case BScanSpecialColorRole:
        return _data[currentRow].bscanSpecialColor;
    case BScanIndividualColorRole:
        return _data[currentRow].bscanIndividualColor;
    default:
        break;
    }

    return QVariant();
}

bool ChannelsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    size_t currentRow = static_cast<size_t>(index.row());
    if (index.isValid() == false || currentRow >= _data.size()) {
        return false;
    }

    switch (role) {
    case IsAlarmedRole:
        _data[currentRow].isAlarmed = value.toBool();
        break;
    case ChannelTypeRole:
        _data[currentRow].channelType = static_cast<ChannelType>(value.toInt());
        break;
    case ZoneRole:
        _data[currentRow].zone = value.toInt();
        break;
    case SensRole:
        _data[currentRow].sens = value.toInt();
        break;
    case AlarmToneRole:
        _data[currentRow].alarmTone = value.toInt();
        break;
    case IsPlayAlarmToneRole:
        _data[currentRow].isPlayAlarmTone = value.toBool();
        break;
    case ChannelNameRole:
        _data[currentRow].channelName = value.toString();
        break;
    case ChannelTitleRole:
        _data[currentRow].channelTitle = value.toString();
        break;
    case ChannelIdRole:
        _data[currentRow].channelId = value.toInt();
        break;
    case GainRole:
        _data[currentRow].gain = value.toInt();
        break;
    case TvgRole:
        _data[currentRow].tvg = value.toFloat();
        break;
    case TimeDelayRole:
        _data[currentRow].prismDelay = value.toFloat();
        break;
    case StartGateRole:
        _data[currentRow].startGate = value.toFloat();
        break;
    case EndGateRole:
        _data[currentRow].endGate = value.toFloat();
        break;
    case StrokeDurationRole:
        _data[currentRow].strokeDuration = value.toInt();
        break;
    case SideRole:
        _data[currentRow].side = static_cast<DeviceSide>(value.toInt());
        break;
    case AngleRole:
        _data[currentRow].angle = value.toInt();
        break;
    case MarkRole:
        _data[currentRow].mark = value.toFloat();
        break;
    case MethodRole:
        _data[currentRow].method = static_cast<eInspectionMethod>(value.toInt());
        break;
    case NoteRole:
        _data[currentRow].note = value.toString();
        break;
    case RecommendedSensRole:
        _data[currentRow].recommendedSens = value.toInt();
        break;
    case GateIndexRole:
        _data[currentRow].gateIndex = value.toInt();
        break;
    case DirectionRole:
        _data[currentRow].direction = static_cast<Direction>(value.toInt());
        break;
    case GateCountRole:
        _data[currentRow].gateCount = value.toInt();
        break;
    case StartBscangateRole:
        _data[currentRow].startBscanGate = value.toInt();
        break;
    case EndBscangateRole:
        _data[currentRow].endBscanGate = value.toInt();
        break;
    case DefaultStartGateRole:
        _data[currentRow].defaultStartGate = value.toInt();
        break;
    case DefaultEndGateRole:
        _data[currentRow].defaultEndGate = value.toInt();
        break;
    case DefaultGainRole:
        _data[currentRow].defaultGain = value.toInt();
        break;
    case DelayMultiplyRole:
        _data[currentRow].delayMultiply = value.toInt();
        break;
    case AScanScaleRole:
        _data[currentRow].aScanScale = value.toInt();
        break;
    case IsStartGateLocked:
        _data[currentRow].isStartGateLocked = value.toBool();
        break;
    case IsEndGateLocked:
        _data[currentRow].isEndGateLocked = value.toBool();
        break;
    case SensLevelsRole:
        _data[currentRow].isSensOutRange = value.toInt();
        break;
    case FrequencyChannelRole:
        _data[currentRow].frequency = static_cast<eWorkFrequency>(value.toInt());
        break;
    case CalibrationTemperatureRole:
        _data[currentRow].calibrationTemperature = value.toInt();
        break;
    case UseNotchRole:
        _data[currentRow].isNotchEnable = value.toBool();
        break;
    case StartNotchRole:
        _data[currentRow].startNotch = value.toInt();
        break;
    case SizeNotchRole:
        _data[currentRow].sizeNotch = value.toInt();
        break;
    case LevelNotchRole:
        _data[currentRow].levelNotch = value.toInt();
        break;
    case IsCopySlaveChannelRole:
        _data[currentRow].isCopySlaveChannel = value.toBool();
        break;
    case BScanColorIndexRole:
        _data[currentRow].bscanColorIndex = value.toUInt();
        break;
    case BScanNormalColorRole:
        _data[currentRow].bscanColor = value.toUInt();
        break;
    case BScanSpecialColorRole:
        _data[currentRow].bscanSpecialColor = value.toUInt();
        break;
    case BScanIndividualColorRole:
        _data[currentRow].bscanIndividualColor = value.toUInt();
        break;
    default:
        return false;
    }
    emit dataChanged(index, index, QVector<int>() << role);

    return true;
}

bool ChannelsModel::insertRows(int position, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(), position, position + count - 1);
    for (int i = position; i < position + count; ++i) {
        _data.insert(_data.begin() + i, Item());
    }
    endInsertRows();

    return true;
}

void ChannelsModel::reset()
{
    beginResetModel();
    _data.clear();
    endResetModel();
}
