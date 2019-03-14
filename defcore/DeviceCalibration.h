#ifndef DEVICECALIBRATION_H_
#define DEVICECALIBRATION_H_

#include "Definitions.h"

class ChannelCalibrationPatch;
class ChannelCalibrationPatchVer3;
class ChannelCalibrationPatchVer2;
class ChannelCalibrationPatchVer4;
class tSChannelCalibration;
class tSChannelCalibrationVer2;
class tSChannelCalibrationVer3;
class tSChannelCalibrationVer4;
class tSChannelCalibrationVer5;
class tCreationTime;
class cChannelsTable;
class cCriticalSection;

class cDeviceCalibration  // Настройки каналов контроля
{
public:
    // если файлов нет, сразу создается безымянная настройка
    cDeviceCalibration(const char* FileName, cChannelsTable* Table, cCriticalSection* CriticalSection);
    // если файлов нет и (toCreateTheOne) сразу создается безымянная настройка
    cDeviceCalibration(const char* FileName, cChannelsTable* Table, cCriticalSection* CriticalSection, bool toCreateTheOne);
    ~cDeviceCalibration();                                                                         // Деструктор
    bool CreateNew();                                                                              // Создать новую настройку
    bool CreateFrom(int Idx);                                                                      // Создать новую настройку из существующей настройки
    bool Delete(int Idx);                                                                          // Удалить настройку при удалении последней создается безымянная настройка
    bool Delete(int Idx, bool toCreateTheOne);                                                     // Удалить настройку при
    bool SetCurrent(int Idx);                                                                      // Установка текущей настройки
    int GetCurrent();                                                                              // Получение индекса текущей настройки, если ошибка: см. PROCRESULT
    int Count();                                                                                   // Количество настроек
    void ResetStates();                                                                            // Сброс состояний всех каналов - на "Не настроен"
    void ResetReadOnly();                                                                          // Сброс свойства ReadOnly для текущей настройки
    void SetReadOnly();                                                                            // Установка свойства ReadOnly для текущей настройки
    eGateMode GetGateMode();                                                                       // Режим работы строба - АСД, настройка Ку, настройка 2Тп
    void SetGateMode(eGateMode Mode);                                                              // Выбор режима работы строба - АСД, настройка Ку, настройка 2Тп
    char* GetName();                                                                               // Название настройки
    char* GetName(int Idx);                                                                        // Название настройки
    bool GetState(eDeviceSide Side, CID Channel, int GateIdx);                                     // Состояние канала - настроен / ненастроен ; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetSens(eDeviceSide Side, CID Channel, int GateIdx);                                       // Условная чувствительность [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetGain(eDeviceSide Side, CID Channel, int GateIdx);                                       // Аттенюатор [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetStGate(eDeviceSide Side, CID Channel, int GateIdx);                                     // Начало строб АСД [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetEdGate(eDeviceSide Side, CID Channel, int GateIdx);                                     // Конец строба АСД [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetPrismDelay(eDeviceSide Side, CID Channel);                                              // Время в призме [10 * мкс]
    int GetTVG(eDeviceSide Side, CID Channel);                                                     // ВРЧ [мкс]
    int GetMark(eDeviceSide Side, CID Channel);                                                    // Метка [мкс] < 0 Выключена
    double GetCalibrationDateTime(eDeviceSide Side, CID Channel);                                  // получить дату и время настройки канала
    int GetScheme();                                                                               // получить индекс схемы прозвучивания для текущей настройки
    int GetScheme(int Idx);                                                                        // получить индекс схемы прозвучивания по индексу
    int GetCalibrationTemperature(eDeviceSide Side, CID Channel);                                  // получить температуру (град.Цельсия) в момент настройки
    void GetSnapShotFileID(sSnapshotFileID* pFileID, eDeviceSide Side, CID Channel, int GateIdx);  // получить ID-файла со скриншотом экрана в момент настройки
                                                                                                   // создать список ID файлов скриншотов для настройки, заданной индексом
                                                                                                   // возвращает указатель (возможно NULL) на вектор
                                                                                                   // удаление списка должно производиться пользователем через вызов DeleteSnapShotFileIDList
    sSnapshotFileIDList* CreateSnapShotFileIDList(int calibrationIdx);
    // удалить ранее созданный список
    void DeleteSnapShotFileIDList(sSnapshotFileIDList* pList);
    int GetNotchStart(eDeviceSide Side, CID Channel, int GateIdx);                                // начало полки [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetNotchDur(eDeviceSide Side, CID Channel, int GateIdx);                                  // длительность полки [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    int GetNotchLevel(eDeviceSide Side, CID Channel, int GateIdx);                                // уровень ослабления в пределах полки [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
                                                                                                  //
    void SetName(char* NewVal);                                                                   // Переименование текущей настройки
    void SetNameByIndex(char* NewVal, int Index);                                                 // Переименование настройки
    void SetState(eDeviceSide Side, CID Channel, int GateIdx, bool NewVal);                       // Состояние канала - настроен / не настроен; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetSens(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                         // Условная чувствительность [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetGain(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                         // Аттенюатор [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetStGate(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                       // Начало строб АСД [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetEdGate(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                       // Конец строба АСД [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetPrismDelay(eDeviceSide Side, CID Channel, int NewVal);                                // Время в призме [10 * мкс]
    void SetTVG(eDeviceSide Side, CID Channel, int NewVal);                                       // ВРЧ [мкс]
    void SetMark(eDeviceSide Side, CID Channel, int NewVal);                                      // Метка [мкс] < 0 Выключена
    void SetCalibrationDateTime(eDeviceSide Side, CID Channel, double dateAndTime);               // установить  дату и время  настройки канала
    void SetScheme(unsigned int idx);                                                             // установить индекс схемы прозвучивания для текущей настройки если ошибка: см. PROCRESULT
    void SetCalibrationTemperature(eDeviceSide Side, CID Channel, int NewVal);                    // Записать температуру (град.Цельсия) в момент настройки
    void SetSnapShotFileID(eDeviceSide Side, CID Channel, int GateIdx, sSnapshotFileID pFileID);  // записать ID-файла со скриншотом экрана в момент настройки
    void SaveToArchFile(char* FileName);                                                          // Сохранить все настройка в файл
    void RestoryFromArchFile(char* FileName);                                                     // Восстановить настройки из файла
    void SetNotchStart(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                   // Задать начало полки [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetNotchDur(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                     // Задать длительность полки [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    void SetNotchLevel(eDeviceSide Side, CID Channel, int GateIdx, int NewVal);                   // Задать уровень ослабления для полки [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)

protected:
    unsigned int FIndex;                     // Индекс текущей настройки
    std::vector<sChannelsCalibration> List;  // Список значений параметров
    eGateMode FMode;
    // Таблица каналов
    cChannelsTable* tbl;
    char* pFileName;        // указатель на имя фала (из конструктора)
    bool fDataChanged;      // устанавливается, если был создан рабочий файл (новый или скопирован из *.dat)
    double alterationTime;  // время последнего изменения данных класса
    void constructorProc(bool toCreateTheOne);
    int writeChannelCalibrationToFile(std::ofstream& file, sChannelCalibration* pChannelCalibration, uint32_t* pSum);
    int writeCalibrationsSetToFile(std::ofstream& file, eDeviceSide side, uint32_t* pSum);
    int readCalibrationsSetFromFile(eDeviceSide side, std::ifstream& file, unsigned int fileFormatVersion);
    int readCalibrationsFromFile(std::vector<sChannelCalibration>* pCalibrations, std::ifstream& file, unsigned int ammountOfchannelCalibration, unsigned int fileFormatVersion);
    int getCalibrationsFromFile(uint8_t** destBufferAddress, std::ifstream& file, unsigned int fileFormatVersion);
    void setChannelCalibrationToDefaultData(eDeviceSide Side, unsigned int channelIdx);  // устанавливает значения полей из описателя канала

    int validateFileMainData(std::ifstream& inputFile, double* pDataCreationTime, bool fNotToClose);
    int validateFileMainData(std::ifstream& inputFile, double* pDataCreationTime, bool fNotToClose, unsigned int* pFileFormatVer);

    int fWrite(const void* pBuffer, uint32_t size, std::ofstream& file, uint32_t* pSum);  // записывает блок данных  в файл, в *pSum обновляется контрольная сумма
    int fRead(void* pBuffer, uint32_t blocks, uint32_t size, std::ifstream& file);
    int saveMainDataToFile(const std::string& FileName);             //  записывает в файл заголовок и основной блок
    int saveMainDataToFile(std::ofstream& file);                     // Сохранить данные в файл
    int getMainDataFromFile(std::ifstream& file, bool fNotToClose);  // открывает файл и заполняет класс данными из основного блока
    int updateDataFromPatch(std::ifstream& file);
    uint32_t countUsedChannels(int calibrationsIndex, eDeviceSide side);

    int readPatchFromFile(uint8_t* pbuf, std::ifstream& file);
    int getNextPatchTime(double* pPatchTime, std::ifstream& fileStream);
    bool createNewAction();
    bool createFromAction(unsigned int Idx);
    bool deleteAction(unsigned int Idx, bool toCreateTheOne);
    bool setNameAction(unsigned int idx, char* newName);
    bool defineReadOnlyAction(unsigned int calibrationIndex, bool onState);
    void channelCalibrationPatchApply(ChannelCalibrationPatch* p);
    void channelCalibrationPatchApply(ChannelCalibrationPatchVer3* p);
    void channelCalibrationPatchApply(ChannelCalibrationPatchVer2* p);
    void channelCalibrationPatchApply(ChannelCalibrationPatchVer4* p);

    // TODO: make this functions static
    void convertCalibrationFormat(tSChannelCalibration* pDest, sChannelCalibration* pCalibration);
    void convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibration* pSource);
    void convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer2* pSource);
    void convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer3* pSource);
    void convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer4* pSource);
    void convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer5* pSource);

    int writeCreateNewActionPatch();
    int writeCreateFromActionPatch(unsigned int calibrationIndex);
    int writeDeleteActionPatch(unsigned int calibrationIndex);
    int writeDeleteActionPatch(unsigned int calibrationIndex, bool toCreateTheOne);
    int writeNameActionPatch(unsigned int calibrationIndex, char* NewVal);
    int writePatch(unsigned int patchSign, unsigned int calibrationIndex);
    int writePatch(unsigned int patchSign, unsigned int calibrationIndex, unsigned int value);
    int writeSetCurrentPatch(unsigned int Idx);
    int writeSetSchemePatch(unsigned int calibrationIndex, unsigned int scheme);

    int writeChannelCalibrationPatch(unsigned int calibrationIndex, eDeviceSide side, sChannelCalibration* pCalibration);
    int writeDefineReadOnlyActionPatch(unsigned int calibrationIndex, bool onState);

    uint32_t readLE32U(volatile uint8_t* pmem);
    void writeLE32U(volatile uint8_t* pmem, uint32_t val);
    uint32_t sumcalc(uint8_t* blockStartAddr, uint32_t blockSize, uint32_t initalSumValue);

    void createWorkFile();

    void getTime(tCreationTime* pTime);
    bool setCurrentAction(unsigned int idx);
    bool setSchemeAction(unsigned int calibrationIndex, unsigned int scheme);

    void saveDatFile(const std::string& fileName, unsigned int fileFormatVer);
    bool saveOldFormatFile(const std::string& fileName, unsigned int fileFormatVer);

    void copyFile(const std::string& pDestFileName, const std::string& pSourceFileName);

    cCriticalSection* criticalSection;
    std::string getWorkFileNameString();
    std::string getStoreFileNameString();
    std::string extentFileNameString(const std::string& source, const std::string& stringToAdd);
};

#endif /* DEVICECALIBRATION_H_ */
