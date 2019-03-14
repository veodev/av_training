/*
 * ChannelTable.cpp
 *
 *  Created on: 06.05.2012
 *      Author: Denis
 */

#include "ChannelsTable.h"
#include "Definitions.h"
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>

#include "ChannelsIds.h"

cChannelsTable::cChannelsTable()  // Конструктор
{
    SkipChangeGatePermission = false;

    List.reserve(128);

    sChannelDescription tmp;
    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////////////////////////////////////
    // Каналы дефектоскопных тележек                                //
    //////////////////////////////////////////////////////////////////
    tmp.id = N0EMS;      // 0x01
    tmp.Name = "N0EMS";  // Нет направления; 0 град; Эхо + ЗТМ
    tmp.Title = "0° Эхо+ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAll;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imMirrorShadow;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmSeparate;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 57;
    tmp.RecommendedAlarmGate[1].gStart = 57;
    tmp.RecommendedAlarmGate[1].gEnd = 62;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    tmp.cdGateMinLen[0] = 30;  // мкс
    tmp.cdGateMinLen[1] = 8;   // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = true;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = N0EMS2;      // 0x02
    tmp.Name = "N0EMS2";  // Нет направления; 0 град; Эхо + ЗТМ
    tmp.Title = "0° Эхо+ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAll;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imMirrorShadow;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 70;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmSeparate;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 57;
    tmp.RecommendedAlarmGate[1].gStart = 57;
    tmp.RecommendedAlarmGate[1].gEnd = 62;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    tmp.cdGateMinLen[0] = 30;  // мкс
    tmp.cdGateMinLen[1] = 8;   // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = true;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58EB;      // 0x03
    tmp.Name = "F58EB";  // Наезжающий; 58/34 град; Эхо; Обе грани
    tmp.Title = "58° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadBoth;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58EW;      // 0x04
    tmp.Name = "F58EW";  // Наезжающий; 58/34 град; Эхо; Рабочая грань
    tmp.Title = "58° Р-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58EU;      // 0x05
    tmp.Name = "F58EU";  // Наезжающий; 58/34 град; Эхо; Нерабочая грань
    tmp.Title = "58° Н-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58ELW;      // 0x06
    tmp.Name = "F58ELW";  // Наезжающий; 58/34 град; Эхо; ?; Рабочая грань
    tmp.Title = "58° Р-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadLeftWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58ELU;      // 0x07
    tmp.Name = "F58ELU";  // Наезжающий; 58/34 град; Эхо; ?; Нерабочая грань
    tmp.Title = "58° Н-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadLeftUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58EB;      // 0x08
    tmp.Name = "B58EB";  // Отъезжающий; 58/34 град; Эхо; Обе грани
    tmp.Title = "58° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadBoth;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58EW;      // 0x09
    tmp.Name = "B58EW";  // Отъезжающий; 58/34 град; Эхо; Рабочая грань
    tmp.Title = "58° Р-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58EU;      // 0x0A
    tmp.Name = "B58EU";  // Отъезжающий; 58/34 град; Эхо; Нерабочая грань
    tmp.Title = "58° Н-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58ELW;      // 0x0B
    tmp.Name = "B58ELW";  // Отъезжающий; 58/34 град; Эхо; ?; Рабочая грань
    tmp.Title = "58° Р-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadLeftWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58ELU;      // 0x0C
    tmp.Name = "B58ELU";  // Отъезжающий; 58/34 град; Эхо; ?; Нерабочая грань
    tmp.Title = "58° Н-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadLeftUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58MB;      // 0x0D
    tmp.Name = "F58MB";  // Наезжающий; 58/34 град; Зеркальный; Обе грани
    tmp.Title = "58° З-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadBoth;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58MW;      // 0x0E
    tmp.Name = "F58MW";  // Наезжающий; 58/34 град; Зеркальный; Рабочая грань
    tmp.Title = "58° ЗР-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58MU;      // 0x0F
    tmp.Name = "F58MU";  // Наезжающий; 58/34 град; Зеркальный; Нерабочая грань
    tmp.Title = "58° ЗН-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58MLW;      // 0x10
    tmp.Name = "F58MLW";  // Наезжающий; 58/34 град; Зеркальный; ?; Рабочая грань
    tmp.Title = "58° ЗР-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadLeftWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F58MLU;      // 0x11
    tmp.Name = "F58MLU";  // Наезжающий; 58/34 град; Зеркальный; Нерабочая грань
    tmp.Title = "58° ЗН-Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadLeftUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58MB;      // 0x12
    tmp.Name = "B58MB";  // Отъезжающий; 58/34 град; Зеркальный; Обе грани
    tmp.Title = "58° З-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadBoth;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58MW;      // 0x13
    tmp.Name = "B58MW";  // Отъезжающий; 58/34 град; Зеркальный; Рабочая грань
    tmp.Title = "58° ЗР-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58MU;      // 0x14
    tmp.Name = "B58MU";  // Отъезжающий; 58/34 град; Зеркальный; Нерабочая грань
    tmp.Title = "58° ЗН-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 180;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58MLW;      // 0x15
    tmp.Name = "B58MLW";  // Отъезжающий; 58/34 град; Зеркальный; ?; Рабочая грань
    tmp.Title = "58° ЗР-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadLeftWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B58MLU;      // 0x16
    tmp.Name = "B58MLU";  // Отъезжающий; 58/34 град; Зеркальный; ?; Нерабочая грань
    tmp.Title = "58° ЗН-О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadLeftUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 25;
    tmp.RecommendedAlarmGate[0].gEnd = 135;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F70E;      // 0x17
    tmp.Name = "F70E";  // Наезжающий; 70 град; Эхо;
    tmp.Title = "70° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 91;
    tmp.cdStrokeDuration = 100;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 85;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B70E;      // 0x18
    tmp.Name = "B70E";  // Отъезжающий; 70 град; Эхо;
    tmp.Title = "70° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 91;
    tmp.cdStrokeDuration = 100;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 85;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F42E;      // 0x19
    tmp.Name = "F42E";  // Наезжающий; 42 град; Эхо;
    tmp.Title = "42° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 42;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B42E;      // 0x1A
    tmp.Name = "B42E";  // Отъезжающий; 42 град; Эхо;
    tmp.Title = "42° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 42;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F45E;      // 0x1B
    tmp.Name = "F45E";  // Наезжающий; 45 град; Эхо;
    tmp.Title = "45° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 200;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B45E;      // 0x1C
    tmp.Name = "B45E";  // Отъезжающий; 45 град; Эхо;
    tmp.Title = "45° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 200;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F22E;      // 0xA1
    tmp.Name = "F22E";  // Наезжающий; 22 град; Эхо;
    tmp.Title = "22° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAll;
    tmp.cdEnterAngle = 22;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 68;
    tmp.cdStrokeDuration = 145;
    tmp.cdAScanScale = 7;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 75;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B22E;      // 0xA2
    tmp.Name = "B22E";  // Отъезжающий; 22 град; Эхо;
    tmp.Title = "22° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAll;
    tmp.cdEnterAngle = 22;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 68;
    tmp.cdStrokeDuration = 145;
    tmp.cdAScanScale = 7;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 75;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F42EE;      // 0x19
    tmp.Name = "F42EE";  // Наезжающий; 42 град; Эхо;
    tmp.Title = "42° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 42;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 200;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 9;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B42EE;      // 0x1A
    tmp.Name = "B42EE";  // Отъезжающий; 42 град; Эхо;
    tmp.Title = "42° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 42;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 200;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 9;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);


    resetChannelDescriptionToDefauls(tmp);
    ///////////////////////////////////////////////////////////
    // Каналы ручного контроля                               //
    ///////////////////////////////////////////////////////////
    tmp.id = H0E;  // 0x1D
    tmp.Name = "H0E";
    tmp.Title = "0° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    //    tmp.cdMethod[1] = imMirrorShadow;
    tmp.cdAlarm[0] = aaByPresence;
    //    tmp.cdAlarm[1] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 57;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H0MS;  // 0x1E
    tmp.Name = "H0MS";
    tmp.Title = "0° ЗТМ";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 57;
    tmp.RecommendedAlarmGate[0].gEnd = 62;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.cdGateMinLen[0] = 8;  // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H45;  // 0x1F
    tmp.Name = "H45";
    tmp.Title = "45° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 184;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 20;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 180;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H50;  // 0x20
    tmp.Name = "H50";
    tmp.Title = "50° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 50;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 184;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 9;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 18;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 200;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H58;  // 0x21
    tmp.Name = "H58";
    tmp.Title = "58° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 138;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 7;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 12;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 158;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H65;  // 0x22
    tmp.Name = "H65";
    tmp.Title = "65° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 138;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 5;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 18;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 110;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H70;  // 0x23
    tmp.Name = "H70";
    tmp.Title = "70° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 91;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 134;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    ///////////////////////////////////////////////////////////
    //                                                       //
    ///////////////////////////////////////////////////////////
    tmp.id = LH0E;  // 0x24
    tmp.Name = "LH0E";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 38;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = LH0MS;  // 0x25
    tmp.Name = "LH0MS";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 38;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = LH45;  // 0x26
    tmp.Name = "LH45";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 184;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 11;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = LH50;  // 0x27
    tmp.Name = "LH50";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 50;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 184;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 11;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = LH58;  // 0x28
    tmp.Name = "LH58";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 138;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 10;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = LH65;  // 0x29
    tmp.Name = "LH65";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 138;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 10;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = LH70;  // 0x2A
    tmp.Name = "LH70";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 91;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 10;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////////
    tmp.id = RH0E;  // 0x2B
    tmp.Name = "RH0E";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 38;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = RH0MS;  // 0x2C
    tmp.Name = "RH0MS";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 38;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = RH45;  // 0x2D
    tmp.Name = "RH45";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 184;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 11;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = RH50;  // 0x2E
    tmp.Name = "RH50";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 50;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 184;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 11;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = RH58;  // 0x2F
    tmp.Name = "RH58";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 138;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 10;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = RH65;  // 0x30
    tmp.Name = "RH65";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 10;
    tmp.cdBScanGate.gEnd = 138;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 10;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = RH70;  // 0x31
    tmp.Name = "RH70";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 91;
    tmp.cdStrokeDuration = 232;
    tmp.cdAScanScale = 10;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    ///////////////////////////////////////////////////////////
    // ?                                                     //
    ///////////////////////////////////////////////////////////
    tmp.id = F70LPE;      // 0x32
    tmp.Name = "F70LPE";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czLeftPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 130;
    tmp.cdStrokeDuration = 150;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F70RPE;      // 0x33
    tmp.Name = "F70RPE";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czRightPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 130;
    tmp.cdStrokeDuration = 150;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B70LPM;      // 0x34
    tmp.Name = "B70LPM";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czLeftPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 45;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = S70LPE;      // 0x35
    tmp.Name = "S70LPE";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czLeftPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 90;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 90;
    tmp.cdStrokeDuration = 105;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = S70RPE;      // 0x36
    tmp.Name = "S70RPE";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czRightPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 90;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 90;
    tmp.cdStrokeDuration = 105;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B70LPE;      // 0x37;
    tmp.Name = "B70LPE";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czLeftPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 45;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 130;
    tmp.cdStrokeDuration = 150;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B70RPE;      // 0x38
    tmp.Name = "B70RPE";  // ?
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czRightPen;
    tmp.cdEnterAngle = 70;
    tmp.cdRotateAngle = 45;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 130;
    tmp.cdStrokeDuration = 150;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = N55MSLW;      // 0x39
    tmp.Name = "N55MSLW";  //  55/90 град; ЗТМ; ?; Рабочая грань
    tmp.Title = "55° Р";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHeadLeftWork;
    tmp.cdEnterAngle = 55;
    tmp.cdRotateAngle = 90;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 60;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 30;
    tmp.RecommendedAlarmGate[0].gEnd = 40;
    tmp.DefaultPrismDelay = 300;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    //    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[0] = 5;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = true;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = N55MSLU;      // 0x3A
    tmp.Name = "N55MSLU";  //  55/90 град; ЗТМ; ?; Нерабочая грань
    tmp.Title = "55° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHeadLeftUnWork;
    tmp.cdEnterAngle = 55;
    tmp.cdRotateAngle = 90;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 60;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 30;
    tmp.RecommendedAlarmGate[0].gEnd = 40;
    tmp.DefaultPrismDelay = 300;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    //    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[0] = 5;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F65E_WP;   // 0xA9
    tmp.Name = "F65E";  //Наезжающий; 65 град; ЭХО;
    tmp.Title = "65° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 65;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 48;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F65EW_WP;   // 0xAA
    tmp.Name = "F65EW";  //Наезжающий; 65 град; ЭХО; Рабочая грань
    tmp.Title = "65° НР";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadAndWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 65;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 48;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F65EU_WP;   // 0xAB
    tmp.Name = "F65EU";  //Наезжающий; 65 град; ЭХО; Нерабочая грань
    tmp.Title = "65° НН";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadAndUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 65;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 48;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B65E_WP;   // 0xAC
    tmp.Name = "B65E";  //Отъезжающий; 65 град; ЭХО;
    tmp.Title = "65° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 65;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 48;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B65EW_WP;   // 0xAD
    tmp.Name = "B65EW";  //Отъезжающий; 65 град; ЭХО; Рабочая грань
    tmp.Title = "65° ОР";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadAndWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 65;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 48;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B65EU_WP;   // 0xAE
    tmp.Name = "B65EU";  //Отъезжающий; 65 град; ЭХО; Нерабочая грань
    tmp.Title = "65° ОН";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadAndUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 65;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 48;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = F45E_WP;   // 0x3B
    tmp.Name = "F45E";  // Наезжающий; 45 град; Эхо;
    tmp.Title = "45° Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = true;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = B45E_WP;   // 0x3C
    tmp.Name = "B45E";  // Отъезжающий; 45 град; Эхо;
    tmp.Title = "45° О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czWebAndBase;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmByDelta;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedSens[1] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 6;
    tmp.RecommendedAlarmGate[0].gEnd = 130;
    tmp.RecommendedAlarmGate[1].gStart = 130;
    tmp.RecommendedAlarmGate[1].gEnd = 160;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    //    tmp.cdGateMinLen[0] = 30; // мкс
    //    tmp.cdGateMinLen[1] = 8; // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 20;
    tmp.cdUseNotch = true;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    tmp.id = N0EMS_WP;   // 0x3D
    tmp.Name = "N0EMS";  // Нет направления; 0 град; Эхо + ЗТМ
    tmp.Title = "0° Эхо+ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAll;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imMirrorShadow;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 70;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmSeparate;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedSens[1] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 57;
    tmp.RecommendedAlarmGate[1].gStart = 57;
    tmp.RecommendedAlarmGate[1].gEnd = 62;
    tmp.DefaultPrismDelay = 900;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 30;
    tmp.cdGateMinLen[0] = 30;  // мкс
    tmp.cdGateMinLen[1] = 8;   // мкс
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = true;
    tmp.LockStGate[1] = true;
    tmp.LockEdGate[1] = true;
    tmp.cdUseNotch = true;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    /* Свободные номера каналов


    */
    ///////////////////////////////////////////////////////////
    // Ручник 0° ЭХО 5 МГц                                   //
    ///////////////////////////////////////////////////////////

    tmp.id = H0E_5MGz;  // 0x3E
    tmp.Name = "H0E";
    tmp.Title = "0° ЭХО (5 МГц)";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    //    tmp.cdMethod[1] = imMirrorShadow;
    tmp.cdAlarm[0] = aaByPresence;
    //    tmp.cdAlarm[1] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.WorkFrequency = wf5MHz;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

///////////////////////////////////////////////////////////
// Каналы Автокон-С                                      //
///////////////////////////////////////////////////////////
//////////////////////////////////
//  Колесный преобразователь 1  //
//////////////////////////////////
#ifdef AUTOCON_CHANNELS
    // КП-1 - Канал: 58 Н1; Номер пластины - 10
    tmp.id = 0x3F;
    tmp.Name = "WP1F581";
    tmp.Title = "КП1 58 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.cdGateSpace = 0;
    tmp.cdTuningToRailType = false;           // Необходимость настройки канала на тип рельса
    tmp.RecommendedSens[0] = 14;              // Рекомендованное значение Ку
    tmp.RecommendedAlarmGate[0].gStart = 30;  // Рекомендованное значение строба АСД
    tmp.RecommendedAlarmGate[0].gEnd = 135;   // Рекомендованное значение строба АСД   // FIXME - RecommendedAlarmGate[0] is set twice
    tmp.DefaultGain = 40;                     // Значение АТТ по умолчанию
    tmp.DefaultTVG = 5;                       // Значение ВРЧ по умолчанию
    tmp.DefaultPrismDelay = 60;               // Значение 2Тп по умолчанию
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 58 Н2; Номер пластины - 8
    tmp.id = 0x40;
    tmp.Name = "WP1F582";
    tmp.Title = "КП1 58 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 65 Н1; Номер пластины - 11
    tmp.id = 0x41;
    tmp.Name = "WP1F651";
    tmp.Title = "КП1 65 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 65 Н2; Номер пластины - 12
    tmp.id = 0x42;
    tmp.Name = "WP1F652";
    tmp.Title = "КП1 65 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 65 Н3; Номер пластины - 13
    tmp.id = 0x43;
    tmp.Name = "WP1F653";
    tmp.Title = "КП1 65 Н3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 45 Н; Номер пластины - 9
    tmp.id = 0x44;
    tmp.Name = "WP1F45";
    tmp.Title = "КП1 45 Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 45 НЗ; Номер пластины - 9
    tmp.id = 0x45;
    tmp.Name = "WP1F45M";
    tmp.Title = "КП1 45 НЗрк";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 0 ЭХО; Номер пластины - 7
    tmp.id = 0x46;
    tmp.Name = "WP1N0EM";  // FIXME - max size of tmp.name - 8, length of "WP1N0EMS" is 9 (str + \0)
    tmp.Title = "КП1 0 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 70;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 45 О; Номер пластины - 5
    tmp.id = 0x47;
    tmp.Name = "WP1B45";
    tmp.Title = "КП1 45 О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 45 ОЗ; Номер пластины - 5
    tmp.id = 0x48;
    tmp.Name = "WP1B45M";
    tmp.Title = "КП1 45 ОЗрк";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 65 О1; Номер пластины - 1
    tmp.id = 0x49;
    tmp.Name = "WP1B651";
    tmp.Title = "КП1 65 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 65 О2; Номер пластины - 2
    tmp.id = 0x4A;
    tmp.Name = "WP1B652";
    tmp.Title = "КП1 65 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 65 О3; Номер пластины - 3
    tmp.id = 0x4B;
    tmp.Name = "WP1B653";
    tmp.Title = "КП1 65 О3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 58 O1; Номер пластины - 6
    tmp.id = 0x4C;
    tmp.Name = "WP1B581";
    tmp.Title = "КП1 58 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-1 - Канал: 58 O2; Номер пластины - 4
    tmp.id = 0x4D;
    tmp.Name = "WP1B582";
    tmp.Title = "КП1 58 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 140;
    tmp.cdStrokeDuration = 140;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 2  //
    //////////////////////////////////
    // КП-2 - Канал: 65 Н; Номер пластины - 7
    tmp.id = 0x4E;
    tmp.Name = "WP2F65";
    tmp.Title = "КП2 65 Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 120;
    tmp.cdStrokeDuration = 120;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 65 НЗ; Номер пластины - 7
    tmp.id = 0x4F;
    tmp.Name = "WP2F65M";
    tmp.Title = "КП2 65 НЗрк";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 60;
    tmp.cdStrokeDuration = 60;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 45 Н2/0; Номер пластины - 5
    tmp.id = 0x50;
    tmp.Name = "WP2F451";
    tmp.Title = "КП2 45 Н2/0";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 45 Н2/4; Номер пластины - 6
    tmp.id = 0x51;
    tmp.Name = "WP2F452";
    tmp.Title = "КП2 45 Н2/4";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 0 ЗТМ; Номер пластины - 4
    tmp.id = 0x52;
    tmp.Name = "WP2N0";
    tmp.Title = "КП2 0 ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 35;
    tmp.cdStrokeDuration = 35;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 45 О2/0; Номер пластины - 2
    tmp.id = 0x53;
    tmp.Name = "WP2F451";
    tmp.Title = "КП2 45 О2/0";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 45 О2/4; Номер пластины - 3
    tmp.id = 0x54;
    tmp.Name = "WP2F452";
    tmp.Title = "КП2 45 О2/4";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 65 О; Номер пластины - 1
    tmp.id = 0x55;
    tmp.Name = "WP2B65";
    tmp.Title = "КП2 65 0";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 120;
    tmp.cdStrokeDuration = 120;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-2 - Канал: 65 ОЗ; Номер пластины - 1
    tmp.id = 0x56;
    tmp.Name = "WP2B65M";
    tmp.Title = "КП2 65 ОЗрк";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 60;
    tmp.cdStrokeDuration = 60;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 3  //
    //////////////////////////////////
    // КП-3 - Канал: 65 Н; Номер пластины - 1
    tmp.id = 0x57;
    tmp.Name = "WP3F65";
    tmp.Title = "КП3 65 Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 120;
    tmp.cdStrokeDuration = 120;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-3 - Канал: 45 Н1/0; Номер пластины - 2
    tmp.id = 0x58;
    tmp.Name = "WP3F451";
    tmp.Title = "КП3 45 Н1/0";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-3 - Канал: 45 Н1/3; Номер пластины - 3
    tmp.id = 0x59;
    tmp.Name = "WP3F452";
    tmp.Title = "КП3 45 Н1/3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-3 - Канал: 0 ЗТМ; Номер пластины - 4
    tmp.id = 0x5A;
    tmp.Name = "WP3N0";
    tmp.Title = "КП3 0 ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 35;
    tmp.cdStrokeDuration = 35;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-3 - Канал: 45 О1/0; Номер пластины - 5
    tmp.id = 0x5B;
    tmp.Name = "WP3B451";
    tmp.Title = "КП3 45 О1/0";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-3 - Канал: 45 О1/3; Номер пластины - 6
    tmp.id = 0x5C;
    tmp.Name = "WP3B452";
    tmp.Title = "КП3 45 О1/3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-3 - Канал: 65 О; Номер пластины - 7
    tmp.id = 0x5D;
    tmp.Name = "WP3B65";
    tmp.Title = "КП3 65 О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 120;
    tmp.cdStrokeDuration = 120;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 4  //
    //////////////////////////////////
    // КП-4 - Канал: 58 Н6/4; Номер пластины - 5
    tmp.id = 0x5E;
    tmp.Name = "WP4F581";
    tmp.Title = "КП4 58 Н6/4";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 58 Н6/8; Номер пластины - 4
    tmp.id = 0x5F;
    tmp.Name = "WP4F582";
    tmp.Title = "КП4 58 Н6/8";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 65 Н1; Номер пластины - 1
    tmp.id = 0x60;
    tmp.Name = "WP4F651";
    tmp.Title = "КП4 65 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 50;
    tmp.cdStrokeDuration = 50;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 65 Н2; Номер пластины - 2
    tmp.id = 0x61;
    tmp.Name = "WP4F652";
    tmp.Title = "КП4 65 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 50;
    tmp.cdStrokeDuration = 50;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 65 Н3; Номер пластины - 3
    tmp.id = 0x62;
    tmp.Name = "WP4F653";
    tmp.Title = "КП4 65 Н3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 50;
    tmp.cdStrokeDuration = 50;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 0 ЗТМ; Номер пластины - 6
    tmp.id = 0x63;
    tmp.Name = "WP4N0MS";
    tmp.Title = "КП4 0 ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 20;
    tmp.cdStrokeDuration = 20;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 65 О1; Номер пластины - 9
    tmp.id = 0x64;
    tmp.Name = "WP4B651";
    tmp.Title = "КП4 65 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 50;
    tmp.cdStrokeDuration = 50;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 65 О2; Номер пластины - 10
    tmp.id = 0x65;
    tmp.Name = "WP4B652";
    tmp.Title = "КП4 65 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 50;
    tmp.cdStrokeDuration = 50;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 65 О3; Номер пластины - 11
    tmp.id = 0x66;
    tmp.Name = "WP4B653";
    tmp.Title = "КП4 65 О3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 50;
    tmp.cdStrokeDuration = 50;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 58 O6/4; Номер пластины - 8
    tmp.id = 0x67;
    tmp.Name = "WP4B581";
    tmp.Title = "КП4 58 О6/4";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-4 - Канал: 58 О6/8; Номер пластины - 7
    tmp.id = 0x68;
    tmp.Name = "WP4B582";
    tmp.Title = "КП4 58 О6/8";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 5  //
    //////////////////////////////////
    // КП-5 - Канал: 58 Н5/3; Номер пластины - 8
    tmp.id = 0x69;
    tmp.Name = "WP5B581";
    tmp.Title = "КП5 58 Н5/3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 58 Н5/7; Номер пластины - 7
    tmp.id = 0x6A;
    tmp.Name = "WP5F582";
    tmp.Title = "КП5 58 Н5/7";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 65 Н1; Номер пластины - 9
    tmp.id = 0x6B;
    tmp.Name = "WP5F651";
    tmp.Title = "КП5 65 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 40;
    tmp.cdStrokeDuration = 40;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 65 Н2; Номер пластины - 10
    tmp.id = 0x6C;
    tmp.Name = "WP5F652";
    tmp.Title = "КП5 65 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 40;
    tmp.cdStrokeDuration = 40;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 65 Н3; Номер пластины - 11
    tmp.id = 0x6D;
    tmp.Name = "WP5F653";
    tmp.Title = "КП5 65 Н3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 40;
    tmp.cdStrokeDuration = 40;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 0 ЗТМ; Номер пластины - 6
    tmp.id = 0x6E;
    tmp.Name = "WP5N0MS";
    tmp.Title = "КП5 0 ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 20;
    tmp.cdStrokeDuration = 20;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 65 О1; Номер пластины - 1
    tmp.id = 0x6F;
    tmp.Name = "WP5B651";
    tmp.Title = "КП5 65 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 40;
    tmp.cdStrokeDuration = 40;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 65 О2; Номер пластины - 2
    tmp.id = 0x70;
    tmp.Name = "WP5B652";
    tmp.Title = "КП5 65 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 40;
    tmp.cdStrokeDuration = 40;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 65 О3; Номер пластины - 3
    tmp.id = 0x71;
    tmp.Name = "WP5B653";
    tmp.Title = "КП5 65 О3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 40;
    tmp.cdStrokeDuration = 40;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 58 O5/3; Номер пластины - 5
    tmp.id = 0x72;
    tmp.Name = "WP5B581";
    tmp.Title = "КП5 58 О5/3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-5 - Канал: 58 O5/7; Номер пластины - 4
    tmp.id = 0x73;
    tmp.Name = "WP5B582";
    tmp.Title = "КП5 58 О5/7";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 6  //
    //////////////////////////////////
    // КП-6 - Канал: 58 Н7; Номер пластины - 7
    tmp.id = 0x74;
    tmp.Name = "WP6B581";
    tmp.Title = "КП6 58 Н7";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 58 Н8; Номер пластины - 8
    tmp.id = 0x75;
    tmp.Name = "WP6F582";
    tmp.Title = "КП6 58 Н8";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 65 Н1; Номер пластины - 9
    tmp.id = 0x76;
    tmp.Name = "WP6F651";
    tmp.Title = "КП6 65 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 65 Н2; Номер пластины - 10
    tmp.id = 0x77;
    tmp.Name = "WP6F652";
    tmp.Title = "КП6 65 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 65 Н3; Номер пластины - 11
    tmp.id = 0x78;
    tmp.Name = "WP6F653";
    tmp.Title = "КП6 65 Н3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 0 ЗТМ; Номер пластины - 6
    tmp.id = 0x79;
    tmp.Name = "WP6N0MS";
    tmp.Title = "КП6 0 ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 20;
    tmp.cdStrokeDuration = 20;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 65 О1; Номер пластины - 1
    tmp.id = 0x7A;
    tmp.Name = "WP6B651";
    tmp.Title = "КП6 65 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 65 О2; Номер пластины - 2
    tmp.id = 0x7B;
    tmp.Name = "WP6B652";
    tmp.Title = "КП6 65 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 65 О3; Номер пластины - 3
    tmp.id = 0x7C;
    tmp.Name = "WP6B653";
    tmp.Title = "КП6 65 О3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 58 O7; Номер пластины - 4
    tmp.id = 0x7D;
    tmp.Name = "WP6B581";
    tmp.Title = "КП6 58 О7";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-6 - Канал: 58 O8; Номер пластины - 5
    tmp.id = 0x7E;
    tmp.Name = "WP6B582";
    tmp.Title = "КП6 58 О8";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 7  //
    //////////////////////////////////
    // КП-7 - Канал: 58 Н7; Номер пластины - 5
    tmp.id = 0x7F;
    tmp.Name = "WP7F581";
    tmp.Title = "КП7 58 Н7";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 58 Н8; Номер пластины - 4
    tmp.id = 0x80;
    tmp.Name = "WP7F582";
    tmp.Title = "КП7 58 Н8";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 65 Н1; Номер пластины - 1
    tmp.id = 0x81;
    tmp.Name = "WP7F651";
    tmp.Title = "КП7 65 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 65 Н2; Номер пластины - 2
    tmp.id = 0x82;
    tmp.Name = "WP7F652";
    tmp.Title = "КП7 65 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 65 Н3; Номер пластины - 3
    tmp.id = 0x83;
    tmp.Name = "WP7F653";
    tmp.Title = "КП7 65 Н3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 0 ЗТМ; Номер пластины - 6
    tmp.id = 0x84;
    tmp.Name = "WP7N0MS";
    tmp.Title = "КП7 0 ЗТМ";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 20;
    tmp.cdStrokeDuration = 20;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdGateMinLen[1] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 65 О1; Номер пластины - 9
    tmp.id = 0x85;
    tmp.Name = "WP7B651";
    tmp.Title = "КП7 65 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 65 О2; Номер пластины - 10
    tmp.id = 0x86;
    tmp.Name = "WP7B652";
    tmp.Title = "КП7 65 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 65 О3; Номер пластины - 11
    tmp.id = 0x87;
    tmp.Name = "WP7B653";
    tmp.Title = "КП7 65 О3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 55;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 58 О8; Номер пластины - 7
    tmp.id = 0x88;
    tmp.Name = "WP7B581";
    tmp.Title = "КП7 58 О8";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-7 - Канал: 58 О7; Номер пластины - 8
    tmp.id = 0x89;
    tmp.Name = "WP7B582";
    tmp.Title = "КП7 58 О7";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 45;
    tmp.cdStrokeDuration = 45;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    //////////////////////////////////
    //  Колесный преобразователь 8  //
    //////////////////////////////////
    // КП-8 - Канал: 58 Н7/5; Номер пластины - 8
    tmp.id = 0x8A;
    tmp.Name = "WP8F581";
    tmp.Title = "КП8 58 Н7/5";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 58 Н8/6; Номер пластины - 10
    tmp.id = 0x8B;
    tmp.Name = "WP8F582";
    tmp.Title = "КП8 58 Н8/6";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 65 Н1; Номер пластины - 11
    tmp.id = 0x8C;
    tmp.Name = "WP8F651";
    tmp.Title = "КП8 65 Н1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 75;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 65 Н2; Номер пластины - 12
    tmp.id = 0x8D;
    tmp.Name = "WP8F652";
    tmp.Title = "КП8 65 Н2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 75;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 65 Н3; Номер пластины - 13
    tmp.id = 0x8E;
    tmp.Name = "WP8F653";
    tmp.Title = "КП8 65 Н3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 75;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 45 Н; Номер пластины - 9
    tmp.id = 0x8F;
    tmp.Name = "WP8F45";
    tmp.Title = "КП8 45 Н";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomIn;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 0 ЭХО; Номер пластины - 7
    tmp.id = 0x90;
    tmp.Name = "WP8N0EM";  // FIXME - max size of tmp.name - 8, length of "WP1N0EMS" is 9 (str + \0)
    tmp.Title = "КП8 0 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 2;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imEcho;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdAlarm[1] = aaByPresence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 70;
    tmp.cdStrokeDuration = 70;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.cdGateMinLen[1] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 45 О; Номер пластины - 5
    tmp.id = 0x91;
    tmp.Name = "WP8B45 ";
    tmp.Title = "КП8 45 О";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirror;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 175;
    tmp.cdStrokeDuration = 175;
    tmp.cdAScanScale = 8;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 65 О1; Номер пластины - 1
    tmp.id = 0x92;
    tmp.Name = "WP8B651";
    tmp.Title = "КП8 65 О1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 75;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 65 О2; Номер пластины - 2
    tmp.id = 0x93;
    tmp.Name = "WP8B652";
    tmp.Title = "КП8 65 О2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 75;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 65 О3; Номер пластины - 3
    tmp.id = 0x94;
    tmp.Name = "WP8B653";
    tmp.Title = "КП8 65 О3";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 65;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 75;
    tmp.cdAScanScale = 4;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 58 O7/5; Номер пластины - 4
    tmp.id = 0x95;
    tmp.Name = "WP8B581";
    tmp.Title = "КП8 58 О7/5";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    // КП-8 - Канал: 58 O8/6; Номер пластины - 6
    tmp.id = 0x96;
    tmp.Name = "WP8B582";
    tmp.Title = "КП8 58 О8/6";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdZoomOut;
    tmp.cdZone = czHeadUnWork;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 80;
    tmp.cdStrokeDuration = 80;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);
#endif
    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);

    tmp.id = 0x97;
    tmp.Name = "L1 GR0";
    tmp.Title = "L1 GR0";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAll;
    tmp.cdEnterAngle = 58;
    tmp.cdRotateAngle = 34;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 160;
    tmp.cdStrokeDuration = 180;
    tmp.cdAScanScale = 6;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    tmp.id = 0x98;
    tmp.Name = "L1 GR1";
    tmp.Title = "L1 GR1";
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    tmp.id = 0x99;
    tmp.Name = "L1 GR2";
    tmp.Title = "L1 GR2";
    List.push_back(tmp);

    tmp.id = 0x9A;
    tmp.Name = "L1 GR3";
    tmp.Title = "L1 GR3";
    List.push_back(tmp);

    tmp.id = 0x9B;
    tmp.Name = "L1 GR4";
    tmp.Title = "L1 GR4";
    List.push_back(tmp);

    tmp.id = 0x9C;
    tmp.Name = "L1 GR5";
    tmp.Title = "L1 GR5";
    List.push_back(tmp);

    tmp.id = 0x9D;
    tmp.Name = "L1 GR6";
    tmp.Title = "L1 GR6";
    List.push_back(tmp);

    tmp.id = 0xA7;
    tmp.Name = "L1 GR7";
    tmp.Title = "L1 GR7";
    List.push_back(tmp);

    ///

    tmp.id = 0x9E;
    tmp.Name = "L2 GR0";
    tmp.Title = "L2 GR0";
    List.push_back(tmp);

    tmp.id = 0x9F;
    tmp.Name = "L2 GR1";
    tmp.Title = "L2 GR1";
    List.push_back(tmp);

    tmp.id = 0xA0;
    tmp.Name = "L2 GR2";
    tmp.Title = "L2 GR2";
    List.push_back(tmp);

    tmp.id = 0xA5;
    tmp.Name = "L2 GR3";
    tmp.Title = "L2 GR3";
    List.push_back(tmp);

    tmp.id = 0xA6;
    tmp.Name = "L2 GR4";
    tmp.Title = "L2 GR4";
    List.push_back(tmp);

    tmp.id = 0xA3;
    tmp.Name = "L2 GR5";
    tmp.Title = "L2 GR5";
    List.push_back(tmp);

    tmp.id = 0xA4;
    tmp.Name = "L2 GR6";
    tmp.Title = "L2 GR6";
    List.push_back(tmp);

    tmp.id = 0xA8;
    tmp.Name = "L2 GR7";
    tmp.Title = "L2 GR7";
    List.push_back(tmp);

    // ------------------------------------------------------------
    // Сканер головки
    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xAF;
    tmp.Name = "0S";
    tmp.Title = "0 ТЕН";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 10;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB0;
    tmp.Name = "+45S";
    tmp.Title = "+45 ТЕН";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 20;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB1;
    tmp.Name = "-45S";
    tmp.Title = "-45 ТЕН";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = -45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 20;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------
    // Сканер головки - Каналы для проверки сканера
    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB2;
    tmp.Name = "A0E";
    tmp.Title = "A 0 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 10;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB3;
    tmp.Name = "B0E";
    tmp.Title = "B 0 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 10;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB4;
    tmp.Name = "A+45E";
    tmp.Title = "A +45 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 20;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB5;
    tmp.Name = "B+45E";
    tmp.Title = "B +45 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = 45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 20;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB6;
    tmp.Name = "A-45E";
    tmp.Title = "A -45 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = -45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 20;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    // ------------------------------------------------------------

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = 0xB7;
    tmp.Name = "B-45E";
    tmp.Title = "B -45 ЭХО";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czHead;
    tmp.cdEnterAngle = -45;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 6;
    tmp.cdBScanGate.gEnd = 55;
    tmp.cdStrokeDuration = 250;
    tmp.cdAScanScale = 2;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedAlarmGate[0].gStart = 20;
    tmp.RecommendedAlarmGate[0].gEnd = 45;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 10;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    tmp.DefaultPrismDelay = 100;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    List.push_back(tmp);

    ///////////////////////////////////////////////////////////
    // Ручник 0° ЗТМ 5 МГц                                   //
    ///////////////////////////////////////////////////////////

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H0MS_5MGz;  // 0xB8
    tmp.Name = "H0MS";
    tmp.Title = "0° ЗТМ (5 МГц)";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.WorkFrequency = wf5MHz;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);


    ///////////////////////////////////////////////////////////
    // Тестовые каналы                                       //
    ///////////////////////////////////////////////////////////

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = TEST_1;  // 0xB9
    tmp.Name = "TEST_1";
    tmp.Title = "TEST_1";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 255;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 11;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.WorkFrequency = wf2_5MHz;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    tmp.id = TEST_2;  // 0xBA
    tmp.Name = "TEST_2";
    tmp.Title = "TEST_2";
    tmp.cdType = ctCompInsp;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByPresence;
    tmp.cdBScanGate.gStart = 0;
    tmp.cdBScanGate.gEnd = 255;
    tmp.cdStrokeDuration = 255;
    tmp.cdAScanScale = 11;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 1;
    tmp.RecommendedSens[0] = 14;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.WorkFrequency = wf2_5MHz;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    // ------------------------------------------------------------
    // ------------------------------------------------------------

    ///////////////////////////////////////////////////////////
    // Каналы ручного контроля 0° для UIC 60                 //
    ///////////////////////////////////////////////////////////
    tmp.id = H0E_UIC60;  // 0xBB
    tmp.Name = "H0E_U60";
    tmp.Title = "0° ЭХО";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imEcho;
    //    tmp.cdMethod[1] = imMirrorShadow;
    tmp.cdAlarm[0] = aaByPresence;
    //    tmp.cdAlarm[1] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 16;
    tmp.RecommendedAlarmGate[0].gStart = 2;
    tmp.RecommendedAlarmGate[0].gEnd = 55;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 30;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

    resetChannelDescriptionToDefauls(tmp);
    tmp.id = H0MS_UIC60;  // 0xBC
    tmp.Name = "H0MS_U6";
    tmp.Title = "0° ЗТМ";
    tmp.cdType = ctHandScan;
    tmp.cdDir = cdNone;
    tmp.cdZone = czAny;
    tmp.cdEnterAngle = 0;
    tmp.cdRotateAngle = 0;
    tmp.cdGateCount = 1;
    tmp.cdMethod[0] = imMirrorShadow;
    tmp.cdMethod[1] = imNotSet;
    tmp.cdAlarm[0] = aaByAbsence;
    tmp.cdBScanGate.gStart = 3;
    tmp.cdBScanGate.gEnd = 75;
    tmp.cdStrokeDuration = 200;
    tmp.cdAScanScale = 3;
    tmp.cdTuningMode = tmNotSet;
    tmp.cdBScanDelayMultiply = 3;
    tmp.RecommendedSens[0] = 14;
    tmp.RecommendedAlarmGate[0].gStart = 55;
    tmp.RecommendedAlarmGate[0].gEnd = 60;
    tmp.DefaultPrismDelay = 45;
    tmp.DefaultTVG = 5;
    tmp.DefaultGain = 40;
    tmp.LockStGate[0] = false;
    tmp.LockEdGate[0] = false;
    tmp.LockStGate[1] = false;
    tmp.LockEdGate[1] = false;
    tmp.cdGateMinLen[0] = 8;
    tmp.cdUseNotch = false;
    tmp.cdInvertedBScan = false;
    List.push_back(tmp);

//    List.shrink_to_fit();

    MaxCID = 0;

    // Расчет длительности развертки
    size_t listSize = List.size();
    for (size_t i = 0; i < listSize; ++i) {
        List[i].cdAScanDuration = std::min(255, static_cast<int>(std::ceil(23.2f * List[i].cdAScanScale)));
        if (static_cast<int>(List[i].id) > MaxCID) {
            MaxCID = static_cast<int>(List[i].id);
        }
    }

    CIDToIndex.resize(static_cast<unsigned int>(MaxCID + 1));

    for (std::vector<unsigned int>::iterator it = CIDToIndex.begin(); it != CIDToIndex.end(); ++it) {
        it.operator*() = 0xFFFFFFFF;
    }

    std::sort(List.begin(), List.end());

    for (size_t i = 0; i < listSize; ++i) {
        CIDToIndex[static_cast<unsigned int>(List[i].id)] = i;
    }
}

cChannelsTable::~cChannelsTable()  // Деструктор
{
    List.clear();
}

int cChannelsTable::StrobeCount(CID id) const
{
    sChannelDescription ChanDesc;
    ItemByCID(id, &ChanDesc);
    return ChanDesc.cdGateCount;
}

size_t cChannelsTable::Count() const  // Количество каналов
{
    return List.size();
}

bool cChannelsTable::ItemByCID(CID id, sChannelDescription* ChanDesc) const  // Получение описания канала по числову идентификатору
{
    DEFCORE_ASSERT(ChanDesc != NULL);
    unsigned int index;
    if (IndexByCID(&index, id)) {
        *ChanDesc = List[index];
        return true;
    }
    return false;
}

bool cChannelsTable::isHandScan(CID id) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        return ChanDesc.cdType == ctHandScan;
    }

    return false;
}

int cChannelsTable::RayMMToDelay(CID id, int MM) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        if (ChanDesc.cdEnterAngle <= 20) {
            float Vt = 5.9f;
            return static_cast<int>(std::floor(2.0f * MM / Vt));
        }

        float Vn = 3.26f;
        return static_cast<int>(std::floor(2.0f * MM / Vn));
    }

    return 0;
}

int cChannelsTable::DelayToRayMM(CID id, int Delay) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        if (ChanDesc.cdEnterAngle <= 20) {
            float Vt = 5.9f;
            return static_cast<int>(std::floor(Vt * Delay / 2.0f));
        }

        float Vn = 3.26f;
        return static_cast<int>(std::floor(Vn * Delay / 2.0f));
    }
    return 0;  // FIXME: I don't known what does it has to return
}

int cChannelsTable::DelayToHeiMM(CID id, int Delay) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        if (ChanDesc.cdEnterAngle <= 20) {
            float Vt = 5.9f;
            return static_cast<int>(std::floor(Vt * Delay * std::cos(static_cast<float>(ChanDesc.cdEnterAngle)) / 2.0f));
        }

        float Vn = 3.26f;
        return static_cast<int>(std::floor(Vn * Delay * std::cos(static_cast<float>(ChanDesc.cdEnterAngle)) / 2.0f));
    }
    return 0;  // FIXME: I don't known what does it has to return
}

int cChannelsTable::DelayToRayMM(CID id, int Delay, int RH) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        int result;
        if (ChanDesc.cdEnterAngle <= 20) {
            float Vt = 5.9f;
            result = static_cast<int>(std::floor(Vt * Delay / 2.0f));
            int j = 0;
            while ((result - RH) >= 0) {
                ++j;
                result -= RH;
            }
            if (j == 1 || j == 3) {
                result = result * -1;
            }
            return result;
        }

        float Vn = 3.26f;
        result = static_cast<int>(std::floor(Vn * Delay / 2.0f));
        int j = 0;
        while ((result - RH) >= 0) {
            ++j;
            result -= RH;
        }
        if (j == 1 || j == 3) {
            result = result * -1;
        }
        return result;
    }
    return 0;  // FIXME: I don't known what does it has to return
}

int cChannelsTable::DelayToHeiMM(CID id, int Delay, int RH) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        int result;
        if (ChanDesc.cdEnterAngle <= 20) {
            float Vt = 5.9f;
            result = static_cast<int>(std::floor(Vt * Delay * std::cos(static_cast<float>(ChanDesc.cdEnterAngle)) / 2.0f));
            int j = 0;
            while ((result - RH) >= 0) {
                ++j;
                result -= RH;
            }
            if (j == 1 || j == 3) {
                result = result * -1;
            }
            return result;
        }

        float Vn = 3.26f;
        result = static_cast<int>(std::floor(Vn * Delay * std::cos(static_cast<float>(ChanDesc.cdEnterAngle)) / 2.0f));
        int j = 0;
        while ((result - RH) >= 0) {
            ++j;
            result -= RH;
        }
        if (j == 1 || j == 3) {
            result = result * -1;
        }
        return result;
    }
    return 0;  // FIXME: I don't known what does it has to return
}

int cChannelsTable::GetGateCount(CID id) const
{
    sChannelDescription ChanDesc;
    if (ItemByCID(id, &ChanDesc)) {
        return ChanDesc.cdGateCount;
    }

    return 1;
}

float cChannelsTable::DepthToDelay(CID id, int Depth) const  // Пересчет задержки отражателя [мм] в глубину
{
    sChannelDescription ChanDesc;
    ItemByCID(id, &ChanDesc);
    if (ChanDesc.cdEnterAngle != 0) {
        return static_cast<float>(((2.0f * (static_cast<float>(Depth) / std::cos(static_cast<float>(ChanDesc.cdEnterAngle) * 3.1415926535f / 180.0f))) / 3.26f));
    }

    return (2.0f * static_cast<float>(Depth) / 5.9f);
}

// Получение описания канала по индексу
bool cChannelsTable::CIDByIndex(CID* pId, unsigned int idx) const
{
    DEFCORE_ASSERT(pId != NULL);
    if ((idx < List.size())) {
        *pId = List[idx].id;
        return true;
    }
    return false;
}


void cChannelsTable::resetChannelDescriptionToDefauls(sChannelDescription& obj)
{
    obj.RecommendedSens[0] = 12;              // Рекомендованное значение Ку
    obj.RecommendedSens[1] = 12;              // Рекомендованное значение Ку
    obj.RecommendedAlarmGate[0].gStart = 10;  // Рекомендованное значение строба АСД
    obj.RecommendedAlarmGate[0].gEnd = 50;
    obj.RecommendedAlarmGate[1].gStart = 10;  // Рекомендованное значение строба АСД
    obj.RecommendedAlarmGate[1].gEnd = 50;    // Рекомендованное значение строба АСД
    obj.DefaultGain = 40;                     // Значение АТТ по умолчанию
    obj.DefaultTVG = 5;                       // Значение ВРЧ по умолчанию
    obj.DefaultPrismDelay = 45;               // Значение 2Тп по умолчанию
    obj.cdAlarm[0] = aaByPresence;
    obj.cdAlarm[1] = aaByPresence;
    obj.WorkFrequency = wf2_5MHz;  // Частота 2.5 МГц
}

// Получение индекса в списке для описания канала по числову идентификатору
bool cChannelsTable::IndexByCID(unsigned int* pIdx, CID id) const
{
    DEFCORE_ASSERT(pIdx != NULL);
    if (static_cast<unsigned int>(id) < CIDToIndex.size()) {
        *pIdx = CIDToIndex[static_cast<unsigned int>(id)];
        DEFCORE_ASSERT(*pIdx != 0xFFFFFFFF);
        return true;
    }
    return false;
}

bool cChannelsTable::GetChannelBScanDelayMultiply(int* Multiply, CID Channel) const
{
    DEFCORE_ASSERT(Multiply != NULL);
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        *Multiply = List[pIdx].cdBScanDelayMultiply;
        return true;
    }
    return false;
}

bool cChannelsTable::GetChannelEnterAngle(int* Angle, CID Channel) const
{
    DEFCORE_ASSERT(Angle != NULL);
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        *Angle = List[pIdx].cdEnterAngle;
        return true;
    }
    return false;
}

int cChannelsTable::GetChannelGateMinLen(CID Channel, int GateIndex) const
{
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        return List[pIdx].cdGateMinLen[GateIndex];
    }
    return 0;
}

bool cChannelsTable::GetPermissionToChangeGate(CID Channel, int GateIndex, bool Start) const
{
    if (SkipChangeGatePermission) {
        return true;
    }

    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        if (Start) {
            return !List[pIdx].LockStGate[GateIndex - 1];
        }

        return !List[pIdx].LockEdGate[GateIndex - 1];
    }
    return false;
}

bool cChannelsTable::GetChannelTitle(CID Channel, std::string& title)
{
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        title = List[pIdx].Title;
        return true;
    }
    return false;
}

bool cChannelsTable::SetChannelTitle(CID Channel, const std::string& NewTitle)
{
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        List[pIdx].Title = NewTitle;
        return true;
    }
    return false;
}


bool cChannelsTable::ItemByName(const std::string& name, sChannelDescription* ChanDesc) const  // Получение описания канала по текстовому идентификатору
{
    for (std::vector<sChannelDescription>::const_iterator it = List.begin(); it != List.end(); ++it) {
        const sChannelDescription& cur = it.operator*();
        if (cur.Name == name) {
            *ChanDesc = cur;
            return true;
        }
    }
    return false;
}

bool cChannelsTable::CIDbyName(const std::string& name, CID* ID) const
{
    for (std::vector<sChannelDescription>::const_iterator it = List.begin(); it != List.end(); ++it) {
        const sChannelDescription& cur = it.operator*();
        if (cur.Name == name) {
            *ID = cur.id;
            return true;
        }
    }
    *ID = 0;
    return false;
}

bool cChannelsTable::GetNamebyCID(CID id, std::string& name) const
{
    unsigned int index;
    if (IndexByCID(&index, id)) {
        name = List[index].Name;
        return true;
    }
    return false;
}

bool cChannelsTable::GetAnotherChannelGateIndex(CID Channel, int GateIndex, int* ResAnotherIndex) const
{
    if (GetGateCount(Channel) == 2) {
        if (GateIndex == 1) {
            *ResAnotherIndex = 2;
        }
        else {
            *ResAnotherIndex = 1;
        }
        return true;
    }
    return false;
}

eAlarmAlgorithm cChannelsTable::GetAlarmAlgorithm(CID Channel, int GateIndex) const  // Алгоритм работы АСД - GateIndex = 0, 1;
{
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        return List[pIdx].cdAlarm[GateIndex];
    }
    return aaNone;
}

eChannelDir cChannelsTable::GetChannelDir(CID Channel) const
{
    unsigned int pIdx;
    if (IndexByCID(&pIdx, Channel)) {
        return List[pIdx].cdDir;
    }
    return cdNone;
}

int cChannelsTable::GetMaxCID() const
{
    return MaxCID;
}
