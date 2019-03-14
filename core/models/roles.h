#ifndef ROLES_H
#define ROLES_H

enum Roles
{
    ChannelTypeRole = Qt::UserRole + 1,
    AngleRole,
    AngleNoteRole,
    NoteRole,
    SensRole,
    IsAlarmedRole,
    AlarmToneRole,
    IsPlayAlarmToneRole,
    ChannelNameRole,
    ChannelTitleRole,
    FakedChannelRole,
    ChannelIdRole,
    GainRole,
    TvgRole,
    StartGateRole,
    EndGateRole,
    StrokeDurationRole,
    TimeDelayRole,
    SideRole,
    MarkRole,
    MethodRole,
    ZoneRole,
    RecommendedSensRole,
    GateIndexRole,
    DirectionRole,
    GateCountRole,
    StartBscangateRole,
    EndBscangateRole,
    DefaultStartGateRole,
    DefaultEndGateRole,
    DefaultGainRole,
    DelayMultiplyRole,
    AScanScaleRole,
    IsStartGateLocked,
    IsEndGateLocked,
    VisibleRole,
    ShowSeparatorsRole,
    HideSeparatorsRole,
    TapeConformityRole,
    IsTapeVisibleInBoltJointModeRole,
    SensLevelsRole,
    FrequencyChannelRole,
    CalibrationTemperatureRole,
    UseNotchRole,
    StartNotchRole,
    SizeNotchRole,
    LevelNotchRole,
    IsCopySlaveChannelRole,
    BScanColorIndexRole,
    BScanNormalColorRole,
    BScanSpecialColorRole,
    BScanIndividualColorRole,
    BScanForwardColorRole,
    BScanBackwardColorRole
};

#endif  // ROLES_H
