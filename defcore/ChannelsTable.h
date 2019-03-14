/*
 * ChannelTable.h
 *
 *  Created on: 06.05.2012
 *      Author: Denis
 */

#ifndef CHANNELTABLE_H
#define CHANNELTABLE_H

#include <vector>
#include <cstring>
#include "Definitions.h"


class cChannelsTable  // Таблица каналов
{
public:
    // элементы в этой секции доступны из любой части программы
    cChannelsTable();   // Конструктор
    ~cChannelsTable();  // Деструктор

    int GetGateCount(CID id) const;
    int StrobeCount(CID id) const;
    size_t Count() const;  // Количество каналов

    bool ItemByCID(CID id, sChannelDescription* ChanDesc) const;  // Получение описания канала по числову идентификатору

    // Отображение на В-развертки ?
    // Метод для перевода задержки [мкс] в глубину [мм]
    // Метод для перевода глубины [мм] в задержку [мкс]
    bool isHandScan(CID id) const;

    int RayMMToDelay(CID id, int MM) const;

    int DelayToRayMM(CID id, int Delay) const;
    int DelayToHeiMM(CID id, int Delay) const;

    int DelayToRayMM(CID id, int Delay, int RH) const;
    int DelayToHeiMM(CID id, int Delay, int RH) const;

    float DepthToDelay(CID id, int Depth) const;        // Пересчет задержки отражателя [мм] в глубину
    bool IndexByCID(unsigned int* pIdx, CID id) const;  // Получение индекса в списке для описания канала по числову идентификатору                               // Получить индекс канала в масиве по CID
    bool CIDByIndex(CID* pId, unsigned int idx) const;  // Получение описания канала по индексу
    bool GetChannelBScanDelayMultiply(int* Multiply, CID Channel) const;                      // Получить множитель задержек В-развертки для канала
    bool GetChannelEnterAngle(int* Angle, CID Channel) const;                                 // Получить угол ввода УЗК канала
    int GetChannelGateMinLen(CID Channel, int GateIndex) const;                               // Получить допустимую минимальную длинну строба: GateIndex = 0, 1;
    bool GetAnotherChannelGateIndex(CID Channel, int GateIndex, int* ResAnotherIndex) const;  // Получить (если возможно) номер второго строба, отличный от заданного
    bool GetPermissionToChangeGate(CID Channel, int GateIndex, bool Start) const;
    eAlarmAlgorithm GetAlarmAlgorithm(CID Channel, int GateIndex) const;  // Алгоритм работы АСД - GateIndex = 0, 1;
    eChannelDir GetChannelDir(CID Channel) const;
    bool GetChannelTitle(CID Channel, std::string& title);
    bool SetChannelTitle(CID Channel, const std::string& NewTitle);
    bool ItemByName(const std::string& name, sChannelDescription* ChanDesc) const;
    bool CIDbyName(const std::string& name, CID* ID) const;
    bool GetNamebyCID(CID id, std::string& name) const;
    int GetMaxCID() const;  // Получение максимального CID

    bool SkipChangeGatePermission;  // Переменна позволяет менять сробы как хочешь
protected:
    int MaxCID;

private:
    void resetChannelDescriptionToDefauls(sChannelDescription& obj);

private:
    std::vector<sChannelDescription> List;  // Список каналов
    std::vector<unsigned int> CIDToIndex;   // Список связи: Идентификатор канала - индекс в массиве описаний каналов
};

#endif /* CHANNELTABLE_H */
