#ifndef MODELTYPES_H
#define MODELTYPES_H

enum ChannelType
{
    UnknownChannel,
    HandChannel,
    ScanChannel
};

enum DeviceSide
{
    NoneDeviceSide = 0,
    LeftDeviceSide = 1,
    RightDeviceSide = 2
};

enum InspectMethod
{
    NotSet = -1,
    Echo = 0,
    MirrorShadow,
    Mirror,
    Shadow
};

enum ChannelFrequency
{
    f2_5MHz = 0,
    f5MHz = 1
};

#endif  // MODELTYPES_H
