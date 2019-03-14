#ifndef EventManagerH
#define EventManagerH

enum EventDataType
{
    edAScanMeas = 0x01,     // [UMU, Device - 1 ptr] А-развертка, точное значение амплитуды и задержки
    edAScanData = 0x02,     // [UMU, Device - 2 ptr] Отсчеты А-развертки
    edTVGData = 0x03,       // [UMU, Device - 2 ptr] Отсчеты ВРЧ
    edAlarmData = 0x04,     // [UMU, Device - 1 ptr] Данные АСД
    edBScan2Data = 0x05,    // [UMU, Device - 1 ptr] Данные В-развертки (тип 2)
    edMScan2Data = 0x06,    // [UMU, Device - 1 ptr] Данные М-развертки (тип 2)
    edMSensor = 0x07,       // [UMU, Device - 2 ptr] Данные датчика металла
                            //    edBScanData  = 0x08,      // Данные В-развертки (тип 1)
                            //    edMScanData  = 0x09,      // Данные М-развертки (тип 1)
    edPathStepData = 0x0A,  // [UMU, Device - 1 ptr] Данные датчика пути

    edSignalPacket = 0x0B,     // [Device - 1 ptr] Пачка сигналов прошедшая фильтр
    edSignalSpacing = 0x0C,    // [Device - 1 ptr] Разрыв линии сигналов
    edDefect53_1 = 0x0D,       // [Device - 1 ptr] Дефект 53.1
    edBottomSignalAmpl = 0x0E, // [Device - 1 ptr] Амплитуда донного сигнала
//    edScanData = 0x0F,       // [Device - 1 ptr] Данные В/М-развертки - второй пакет данных, на него ссылается edBScan2Data/edMScan2Data
    edDeviceSpeed = 0x10,      // [Device - 1 uint] Скорость

    edA15ScanerPathStepData = 0x11  // [UMU, Device - 1 ptr] Данные датчика пути

    //
};

class cEventManager
{
public:
    int Tag;  // Для отладки

    virtual ~cEventManager() {}
    virtual bool CreateNewEvent(unsigned long Id) = 0;
    virtual bool WriteEventData(void* Ptr, unsigned int Size) = 0;
    virtual void Post(void) = 0;
    virtual bool WaitForEvent(void) = 0;
    virtual int EventDataSize(void) = 0;
    virtual bool ReadEventData(void* Ptr, unsigned int Size, unsigned long* Read) = 0;
    virtual bool PeekEventData(void* Ptr, unsigned int Size, unsigned long* lpBytesRead, unsigned long* lpTotalBytesAvail, unsigned long* lpBytesLeftThisMessage) = 0;
    virtual void WriteHeadBodyAndPost(unsigned long Id, void* headptr, unsigned int headsize, void* bodyptr, unsigned int bodysize) = 0;
    virtual void WriteHeadAndPost(unsigned long Id, void* headptr, unsigned int headsize) = 0;
};

#endif
