//При открытии файла предыдущей версии он сохраняется под именем <имя>_version_<старый_номер версии>_ддммгг_ччммcc_мс

#include "DeviceCalibration.h"
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <ctime>

#include <fstream>
#include <iostream>

#include "ChannelsTable.h"
#include "CriticalSection.h"

#define sign1 1
#define sign2 2
#define sign3 4

#include "DeviceCalibration_definitions.h"

// если файлов нет, сразу создается безымянная настройка
cDeviceCalibration::cDeviceCalibration(const char* FileName, cChannelsTable* Table, cCriticalSection* CriticalSection)
    : FMode(gmSearch)
{
    criticalSection = CriticalSection;
    fDataChanged = false;
    tbl = Table;
    pFileName = strdup(FileName);
    constructorProc(true);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
// если файлов нет и (toCreateTheOne) сразу создается безымянная настройка
cDeviceCalibration::cDeviceCalibration(const char* FileName, cChannelsTable* Table, cCriticalSection* CriticalSection, bool toCreateTheOne)
    : FMode(gmSearch)
{
    criticalSection = CriticalSection;
    fDataChanged = false;
    tbl = Table;
    pFileName = strdup(FileName);
    constructorProc(toCreateTheOne);
    std::cerr << "Calibration| Calibration file name(create): Filename = " << FileName << " pFileName = " << pFileName << std::endl;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
cDeviceCalibration::~cDeviceCalibration()
{
    std::string workFileName = getWorkFileNameString();
    std::cerr << "Calibration| Calibration file name(destructor): " << workFileName << std::endl;
    std::ifstream inputFile(workFileName.c_str(), std::ios_base::in | std::ios_base::binary);
    int res = validateFileMainData(inputFile, NULL, false);
    std::cerr << "Calibration| Calibration validated: " << res << std::endl;

    assert(res != ERROR_FILE_NOTFOUND);
    assert(res != ERROR_BAD_FILEVERSION);
    assert(res != ERROR_FILE_READ_FAILED);
    assert(res != ERROR_FILE_WRITE_FAILED);
    assert(res != ERROR_ACTION_FAILED);
    assert(res != ERROR_ARCH_MISMATCH);
    assert(res != ERROR_BAD_SUM);
    assert(res != ERROR_BAD_DATA);
    assert(res != ERROR_MEMALLOC_FAILED);

    std::ifstream file(pFileName, std::ios_base::in | std::ios_base::binary);

    bool exists = false;
    if (file.good()) {
        exists = true;
        file.close();
        std::cerr << "Calibration| file:" << pFileName << " exists!" << std::endl;
    }
    else {
        std::cerr << "Calibration| file:" << pFileName << " does not exist!" << std::endl;
    }

    if ((fDataChanged) || (!exists)) {  // если нет файла *.dat что-то мы меняли, поэтому
                                        // перезаписываем или создаем файл .dat
        std::cerr << "Calibration| create new file:" << pFileName << " fDataChanged:" << fDataChanged << std::endl;
        std::string storeFileName = getStoreFileNameString();
        if (!storeFileName.empty()) {
            rename(pFileName, storeFileName.c_str());

            res = saveMainDataToFile(std::string(pFileName));
            if (res == ERROR_NO) {
                remove(storeFileName.c_str());  // "~1.dat"
                std::string newWorkFileName = getWorkFileNameString();
                remove(newWorkFileName.c_str());  //  "~.dat"
            }
            else {
                std::cerr << "===== ERROR:" << __FUNCTION__ << " saveMainDataToFile res:" << res << std::endl;
            }
        }
    }
    else {
        if (fDataChanged == false) {
            std::string newWorkFileName = getWorkFileNameString();
            std::cerr << "Calibration| removing work file:" << newWorkFileName << std::endl;
            remove(newWorkFileName.c_str());  //  "~.dat"
        }
    }
    size_t size = List.size();
    for (size_t i = 0; i < size; ++i) {
        (List[i]).List[dsNone].clear();
        ((List[i]).List[dsLeft]).clear();
        ((List[i]).List[dsRight]).clear();
    }
    List.clear();

    free(pFileName);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

void cDeviceCalibration::constructorProc(bool toCreateTheOne)
{
    int fFile = 0;
    int fWorkFile = 0;
    int fStoreFile = 0;

    bool fRepeat = false;
    int res = 0;

    std::ifstream fileStream(pFileName, std::ios_base::in | std::ios_base::binary);
    if (fileStream.good()) {
        fFile = sign1;
        fileStream.close();
    }

    std::ifstream workfileStream(getWorkFileNameString().c_str(), std::ios_base::in | std::ios_base::binary);
    if (workfileStream.good()) {
        fWorkFile = sign2;
        workfileStream.close();
    }

    std::ifstream storeStream(getStoreFileNameString().c_str(), std::ios_base::in | std::ios_base::binary);
    if (storeStream.good()) {
        fStoreFile = sign3;
        storeStream.close();
    }

    do {
        fRepeat = false;
        switch (fFile | fWorkFile | fStoreFile) {
        case 0: {
            tCreationTime t;
            getTime(&t);                                                           // время создания файла
            memcpy(&alterationTime, reinterpret_cast<char*>(&t), sizeof(double));  // ни одного из файлов нет
            std::ofstream workfileStream(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary);
            if (workfileStream.good()) {
                saveMainDataToFile(workfileStream);
                workfileStream.flush();
                workfileStream.close();
            }
            if (toCreateTheOne) {
                CreateNew();
            }
            fDataChanged = true;
            break;
        }
        //
        case sign1: {
            double datTime;
            unsigned int fileFormatVer;
            int res;
            // проверка корректности файла *.dat
            std::ifstream fileStreamValidation(pFileName, std::ios_base::in | std::ios_base::binary);
            res = validateFileMainData(fileStreamValidation, &datTime, false, &fileFormatVer);
            if (res != ERROR_NO) {
                if (res == ERROR_BAD_FILEVERSION) {
                    saveDatFile(pFileName, fileFormatVer);
                }
                fFile = 0;
                res = remove(pFileName);
                if (res == ERROR_NO) fRepeat = true;
            }
            else {
                std::ifstream fileStreamRead(pFileName, std::ios_base::in | std::ios_base::binary);
                getMainDataFromFile(fileStreamRead, false);
                if (saveOldFormatFile(pFileName, fileFormatVer) == true) {
                    remove(pFileName);  // удалить, т.к. при завершении ПО
                                        // без вызова деструктора так и будем плодить сохраняемые файлы
                    saveMainDataToFile(std::string(pFileName));
                }
                alterationTime = datTime;
                createWorkFile();
            }
            break;
        }
        case sign2: {
            double datTime;
            unsigned int fileFormatVer;
            std::ifstream workFileValidationStream(getWorkFileNameString().c_str(), std::ios_base::in | std::ios_base::binary);
            if (validateFileMainData(workFileValidationStream, &datTime, false, &fileFormatVer) == ERROR_NO) {
                std::ifstream workFileStreamRead(getWorkFileNameString().c_str(), std::ios_base::in | std::ios_base::binary);
                if (workFileStreamRead.good()) {
                    if (getMainDataFromFile(workFileStreamRead, true) == ERROR_NO) {
                        do {
                            res = updateDataFromPatch(workFileStreamRead);
                        } while (res == ERROR_NO);
                        workFileStreamRead.close();
                    }
                }
                alterationTime = datTime;
                if (saveOldFormatFile(getWorkFileNameString(), fileFormatVer) == true) {
                    remove(getWorkFileNameString().c_str());
                    saveMainDataToFile(std::string(pFileName));
                }
            }
            else {
                fWorkFile = 0;
                remove(getWorkFileNameString().c_str());
                fRepeat = true;
            }
            break;
        }
        case (sign1 | sign2): {
            int res1, res2;
            double datTime;
            double patchTime = 0;
            unsigned int fileFormatVer;

            std::ifstream fileValidationStream(pFileName, std::ios_base::in | std::ios_base::binary);
            std::ifstream workfileValidationStream(getWorkFileNameString().c_str(), std::ios_base::in | std::ios_base::binary);
            res1 = validateFileMainData(fileValidationStream, &datTime, false, &fileFormatVer);
            res2 = validateFileMainData(workfileValidationStream, NULL, true);
            if ((res1 == ERROR_NO) && (res2 == ERROR_NO)) {
                do {
                    res = getNextPatchTime(&patchTime, workfileValidationStream);
                } while (res == ERROR_NO);
                workfileValidationStream.close();
                if (datTime >= patchTime) {
                    fWorkFile = 0;
                    remove(getWorkFileNameString().c_str());
                }
                else {
                    fFile = 0;
                    remove(pFileName);
                }
            }
            else {
                if (workfileValidationStream.good()) {
                    workfileValidationStream.close();
                }
                if (res1 != ERROR_NO) {
                    if (res1 == ERROR_BAD_FILEVERSION) {
                        saveDatFile((char*) pFileName, fileFormatVer);
                    }
                    fFile = 0;
                    remove(pFileName);
                }
                if (res2 != ERROR_NO) {
                    fWorkFile = 0;
                    remove(getWorkFileNameString().c_str());
                }
            }
            fRepeat = true;
            break;
        }
        case sign3: {
            std::string storeName = getStoreFileNameString();
            rename(storeName.c_str(), pFileName);
            fFile = sign1;
            fStoreFile = 0;
            fRepeat = true;
            break;
        }
        case (sign1 | sign3):
        case (sign1 | sign2 | sign3): {
            std::string storeName = getStoreFileNameString();
            remove(storeName.c_str());
            fStoreFile = 0;
            fRepeat = true;
            break;
        }
        }
    } while (fRepeat);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::CreateNew()  // Создать новую настройку
{
    bool res;
    criticalSection->Enter();
    res = createNewAction();
    if (res) {
        writeCreateNewActionPatch();
    }
    criticalSection->Release();
    return res;
}

bool cDeviceCalibration::CreateFrom(int Idx)  // Создать новую настройку из существующей настройки
{
    DEFCORE_ASSERT(Idx >= 0);
    bool res;
    criticalSection->Enter();
    res = createFromAction(Idx);
    if (res) {
        writeCreateFromActionPatch(Idx);
    }
    criticalSection->Release();
    return res;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
// toCreateTheOne == true - создать новую настройку, если удалялась последняя
bool cDeviceCalibration::Delete(int Idx, bool toCreateTheOne)  // Удалить настройку
{
    DEFCORE_ASSERT(Idx >= 0);
    bool res;
    criticalSection->Enter();
    res = deleteAction(Idx, toCreateTheOne);
    if (res) {
        writeDeleteActionPatch(Idx);
    }
    criticalSection->Release();
    return res;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::Delete(int Idx)  // Удалить настройку
{
    DEFCORE_ASSERT(Idx >= 0);
    Delete(Idx, true);
    return true;  // TODO: added return
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::SaveToArchFile(char* FileName)  // Сохранить все настройка в файле
{
    UNUSED(FileName);
}

void cDeviceCalibration::RestoryFromArchFile(char* FileName)  // Восстановить настройки из файла
{
    UNUSED(FileName);
}

int cDeviceCalibration::Count()  // Количество настроек
{
    int res;
    criticalSection->Enter();
    res = List.size();
    criticalSection->Release();
    return res;
}

bool cDeviceCalibration::SetCurrent(int Idx)  // Установка текущей настройки
{
    DEFCORE_ASSERT(Idx >= 0);
    criticalSection->Enter();
    if (setCurrentAction(Idx)) {
        writeSetCurrentPatch(Idx);
        criticalSection->Release();
        return true;
    }
    criticalSection->Release();
    return false;
}

int cDeviceCalibration::GetCurrent()  // получить индекс текущей настройки
{
    int res;
    criticalSection->Enter();
    if (List.size() > 0)
        res = static_cast<int>(FIndex);
    else
        res = ERROR_ACTION_FAILED;
    criticalSection->Release();
    return res;
}

void cDeviceCalibration::ResetStates()  // Сброс состояний всех каналов - на ненастроен
{
    criticalSection->Enter();
    if (FIndex < static_cast<unsigned int>(List.size())) {
        size_t size = tbl->Count();
        for (size_t ii = 0; ii < size; ++ii) {
            ((List[FIndex]).List[dsNone][ii]).Calibrated[0] = false;
            ((List[FIndex]).List[dsNone][ii]).Calibrated[1] = false;
            ((List[FIndex]).List[dsLeft][ii]).Calibrated[0] = false;
            ((List[FIndex]).List[dsLeft][ii]).Calibrated[1] = false;
            ((List[FIndex]).List[dsRight][ii]).Calibrated[0] = false;
            ((List[FIndex]).List[dsRight][ii]).Calibrated[1] = false;
        }
    }
    criticalSection->Release();
}
//
void cDeviceCalibration::ResetReadOnly()  // Сброс свойства ReadOnly для текущей настройки
{
    criticalSection->Enter();
    if (defineReadOnlyAction(FIndex, false)) writeDefineReadOnlyActionPatch(FIndex, false);
    criticalSection->Release();
}
//
void cDeviceCalibration::SetReadOnly()  // Установка свойства ReadOnly для текущей настройки
{
    criticalSection->Enter();
    if (defineReadOnlyAction(FIndex, true)) writeDefineReadOnlyActionPatch(FIndex, true);
    criticalSection->Release();
}
//
char* cDeviceCalibration::GetName()  // Название настройки
{
    char* res;
    criticalSection->Enter();
    if (List.size() > 0)
        res = (char*) &List[FIndex].Name;
    else
        res = NULL;
    criticalSection->Release();
    return res;
}

char* cDeviceCalibration::GetName(int Idx)
{
    DEFCORE_ASSERT(Idx >= 0);
    char* res;
    criticalSection->Enter();
    if (Idx >= 0 && Idx < static_cast<int>(List.size()))
        res = (char*) &List[Idx].Name;
    else
        res = NULL;
    criticalSection->Release();
    return res;
}

bool cDeviceCalibration::GetState(eDeviceSide Side, CID Channel, int GateIdx)  // Состояние канала - настроен / ненастроен
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        bool res = List[FIndex].List[Side][cidx].Calibrated[GateIdx];
        criticalSection->Release();
        return res;
    }
    return false;
}

int cDeviceCalibration::GetSens(eDeviceSide Side, CID Channel, int GateIdx)  // Условная чувствительность [дБ]
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        int sens = List[FIndex].List[Side][cidx].Sens[GateIdx];
        criticalSection->Release();
        return sens;
    }
    return 0;
}

int cDeviceCalibration::GetGain(eDeviceSide Side, CID Channel, int GateIdx)  // Аттенюатор [дБ]
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        int gain = List[FIndex].List[Side][cidx].Gain[GateIdx];
        criticalSection->Release();
        return gain;
    }
    return 0;
}

eGateMode cDeviceCalibration::GetGateMode()
{
    return FMode;
}
//
double cDeviceCalibration::GetCalibrationDateTime(eDeviceSide Side, CID Channel)  // получить дату и время настройки канала
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        double res = List[FIndex].List[Side][cidx].DataTime;
        criticalSection->Release();
        return res;
    }
    return 0.0;
}

int cDeviceCalibration::GetScheme()  // получить индекс схемы прозвучивания для текущей настройки
{
    return GetScheme(FIndex);
}


int cDeviceCalibration::GetScheme(int Idx)
{
    DEFCORE_ASSERT(Idx >= 0);
    int res;
    criticalSection->Enter();
    if (List.size() > 0) {
        res = static_cast<int>(List[Idx].scheme);
    }
    else
        res = ERROR_ACTION_FAILED;
    criticalSection->Release();
    return res;
}

//
void cDeviceCalibration::SetGateMode(eGateMode Mode)
{
    FMode = Mode;
}

int cDeviceCalibration::GetStGate(eDeviceSide Side, CID Channel, int GateIdx)  // Начало строб АСД [мкс]
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_HIGHMAX;
    unsigned int cidx;
    int res = 0;

    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        switch (FMode) {
        case gmSearch: {
            res = List[FIndex].List[Side][cidx].StGate[GateIdx];
            break;
        }
        case gmSensCalibration: {
            res = List[FIndex].List[Side][cidx].StGate[GateIdx + 2];
            break;
        }
        case gmPrismDelayCalibration: {
            res = List[FIndex].List[Side][cidx].StGate[GateIdx + 4];
            break;
        }
        }
        criticalSection->Release();
        return res;
    }
    return 0;
}

int cDeviceCalibration::GetEdGate(eDeviceSide Side, CID Channel, int GateIdx)  // Конец строба АСД [мкс]
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_HIGHMAX;
    unsigned int cidx;
    int res = 0;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        switch (FMode) {
        case gmSearch: {
            res = List[FIndex].List[Side][cidx].EdGate[GateIdx];
            break;
        }
        case gmSensCalibration: {
            res = List[FIndex].List[Side][cidx].EdGate[GateIdx + 2];
            break;
        }
        case gmPrismDelayCalibration: {
            res = List[FIndex].List[Side][cidx].EdGate[GateIdx + 4];
            break;
        }
        }
        criticalSection->Release();
        return res;
    }
    return 0;
}

int cDeviceCalibration::GetPrismDelay(eDeviceSide Side, CID Channel)  // Время в призме [10 * мкс]
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        int res = List[FIndex].List[Side][cidx].PrismDelay;
        criticalSection->Release();
        return res;
    }
    return 0;
}

int cDeviceCalibration::GetTVG(eDeviceSide Side, CID Channel)  // ВРЧ [мкс]
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        int tvg = List[FIndex].List[Side][cidx].TVG;
        criticalSection->Release();
        return tvg;
    }
    return 0;
}

int cDeviceCalibration::GetMark(eDeviceSide Side, CID Channel)  // Начало метки [мкс] < 0 Выключенна
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        int res = List[FIndex].List[Side][cidx].Mark;
        criticalSection->Release();
        return res;
    }
    return 0;
}
//---------------------------------------------------------------------------------
int cDeviceCalibration::GetCalibrationTemperature(eDeviceSide Side, CID Channel)
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        int res = List[FIndex].List[Side][cidx].CalibrationTemperatureValue;
        criticalSection->Release();
        return res;
    }
    return cTemperatureValueUnknown;
}
//---------------------------------------------------------------------------------
void cDeviceCalibration::GetSnapShotFileID(sSnapshotFileID* pFileID, eDeviceSide Side, CID Channel, int GateIdx)
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    assert(pFileID != NULL);

    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        *pFileID = List[FIndex].List[Side][cidx].SnapshotFileID[GateIdx & 1];
        criticalSection->Release();
    }
}
//---------------------------------------------------------------------------------
// создать список ID файлов скриншотов для настройки, заданной индексом
// возвращает указатель (возможно NULL) на вектор
// удаление списка должно производиться пользователем через вызов DeleteSnapShotFileIDList
sSnapshotFileIDList* cDeviceCalibration::CreateSnapShotFileIDList(int calibrationIdx)
{
    sSnapshotFileIDList* pList;
    std::vector<sChannelCalibration>* pChannelList;
    int jj;
    criticalSection->Enter();
    if ((List.size() == 0) || static_cast<unsigned long>(calibrationIdx) >= List.size()) {
        criticalSection->Release();
        return NULL;
    }
    pList = new sSnapshotFileIDList;
    assert(pList != NULL);
    for (unsigned char ii = 0; ii < 3; ++ii) {
        switch (ii) {
        case 2: {
            pChannelList = &List[static_cast<unsigned long>(calibrationIdx)].List[2];
            break;
        }
        case 1: {
            pChannelList = &List[static_cast<unsigned long>(calibrationIdx)].List[1];
            break;
        }
        default:
            pChannelList = &List[static_cast<unsigned long>(calibrationIdx)].List[0];
            break;
        }
        for (jj = 0; jj < static_cast<int>(List[static_cast<unsigned long>(calibrationIdx)].List[ii].size()); ++jj) {
            if ((*pChannelList)[static_cast<unsigned long>(jj)].used == true) {
                if ((*pChannelList)[static_cast<unsigned long>(jj)].SnapshotFileID[0] != cSnapshortFileIdUnknown) {
                    unsigned long index;
                    index = (*pList).size();
                    (*pList).resize(index + 1);
                    (*pList)[index] = (*pChannelList)[jj].SnapshotFileID[0];
                }
                if ((*pChannelList)[jj].SnapshotFileID[1] != cSnapshortFileIdUnknown) {
                    unsigned long index;
                    index = (*pList).size();
                    (*pList).resize(index + 1);
                    (*pList)[index] = (*pChannelList)[jj].SnapshotFileID[1];
                }
            }
        }
    }
    criticalSection->Release();
    return pList;
}
//---------------------------------------------------------------------------------
// удалить ранее созданный список
void cDeviceCalibration::DeleteSnapShotFileIDList(sSnapshotFileIDList* pList)
{
    DEFCORE_ASSERT(pList != NULL);
    criticalSection->Enter();
    if (pList) {
        (*pList).clear();
        delete pList;
    }
    criticalSection->Release();
}
//---------------------------------------------------------------------------------
#define GetNotchParameter(ParameterName, Side, Channel, GateIdx)               \
    {                                                                          \
        unsigned int cidx;                                                     \
        VERIFY_GATE_IDX_LOWMAX;                                                \
        if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {              \
            criticalSection->Enter();                                          \
            setChannelCalibrationToDefaultData(Side, cidx);                    \
            int result = List[FIndex].List[Side][cidx].ParameterName[GateIdx]; \
            criticalSection->Release();                                        \
            return result;                                                     \
        }                                                                      \
        return 0;                                                              \
    }
//---------------------------------------------------------------------------------
int cDeviceCalibration::GetNotchStart(eDeviceSide Side, CID Channel, int GateIdx)
{
    GetNotchParameter(NotchStart, Side, Channel, GateIdx);
}
//---------------------------------------------------------------------------------
int cDeviceCalibration::GetNotchDur(eDeviceSide Side, CID Channel, int GateIdx)
{
    GetNotchParameter(NotchDur, Side, Channel, GateIdx);
}
//---------------------------------------------------------------------------------
int cDeviceCalibration::GetNotchLevel(eDeviceSide Side, CID Channel, int GateIdx)
{
    GetNotchParameter(NotchLevel, Side, Channel, GateIdx);
}
//---------------------------------------------------------------------------------
void cDeviceCalibration::SetCalibrationDateTime(eDeviceSide Side, CID Channel, double DateAndTime)  // установить дату и время настройки канала
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].DataTime = DateAndTime;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetName(char* NewVal)  // переименование текущей настройки
{
    SetNameByIndex(NewVal, FIndex);
}

void cDeviceCalibration::SetNameByIndex(char* NewVal, int Index)
{
    DEFCORE_ASSERT(NewVal != NULL);
    criticalSection->Enter();
    if (Index >= 0 && Index < static_cast<int>(List.size()) && List[Index].ReadOnly == false) {
        if (setNameAction(Index, NewVal)) {
            writeNameActionPatch(Index, NewVal);
        }
    }
    criticalSection->Release();
}

void cDeviceCalibration::SetState(eDeviceSide Side, CID Channel, int GateIdx, bool NewVal)  // Состояние канала - настроен / не настроен
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].Calibrated[GateIdx] = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetSens(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)  // Условная чувствительность [дБ]
{
    VERIFY_CID_IDX;
    VERIFY_NEW_VALUE_SENS;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].Sens[GateIdx] = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetGain(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)  // Аттенюатор [дБ]
{
    VERIFY_CID_IDX;
    VERIFY_NEW_VALUE_GAIN;
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].Gain[GateIdx] = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetStGate(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)  // Начало строб АСД [мкс]
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_HIGHMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            switch (FMode) {
            case gmSearch: {
                List[FIndex].List[Side][cidx].StGate[GateIdx] = NewVal;
                break;
            }
            case gmSensCalibration: {
                List[FIndex].List[Side][cidx].StGate[GateIdx + 2] = NewVal;
                break;
            }
            case gmPrismDelayCalibration: {
                List[FIndex].List[Side][cidx].StGate[GateIdx + 4] = NewVal;
                break;
            }
            }
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetEdGate(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)  // Конец строба АСД [мкс]
{
    VERIFY_CID_IDX;
    VERIFY_GATE_IDX_HIGHMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            switch (FMode) {
            case gmSearch: {
                List[FIndex].List[Side][cidx].EdGate[GateIdx] = NewVal;
                break;
            }
            case gmSensCalibration: {
                List[FIndex].List[Side][cidx].EdGate[GateIdx + 2] = NewVal;
                break;
            }
            case gmPrismDelayCalibration: {
                List[FIndex].List[Side][cidx].EdGate[GateIdx + 4] = NewVal;
                break;
            }
            }
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetPrismDelay(eDeviceSide Side, CID Channel, int NewVal)  // Время в призме [10 * мкс]
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].PrismDelay = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetTVG(eDeviceSide Side, CID Channel, int NewVal)  // ВРЧ [мкс]
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].TVG = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}

void cDeviceCalibration::SetMark(eDeviceSide Side, CID Channel, int NewVal)  // Начало метки [мкс] < 0 Выключенна
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].Mark = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::SetScheme(unsigned int idx)  // установить индекс схемы прозвучивания для текущей настройки
{
    criticalSection->Enter();
    if ((List[FIndex]).ReadOnly == false) {
        if (setSchemeAction(FIndex, idx)) writeSetSchemePatch(FIndex, idx);
    }
    criticalSection->Release();
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// Записать температуру (град.Цельсия) в момент настройки
void cDeviceCalibration::SetCalibrationTemperature(eDeviceSide Side, CID Channel, int NewVal)
{
    VERIFY_CID_IDX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].CalibrationTemperatureValue = NewVal;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// записать ID-файла со скриншотом экрана в момент настройки
void cDeviceCalibration::SetSnapShotFileID(eDeviceSide Side, CID Channel, int GateIdx, sSnapshotFileID pFileID)
{
    VERIFY_GATE_IDX_LOWMAX;
    unsigned int cidx;
    if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {
        criticalSection->Enter();
        setChannelCalibrationToDefaultData(Side, cidx);
        if ((List[FIndex]).ReadOnly == false) {
            List[FIndex].List[Side][cidx].SnapshotFileID[GateIdx & 1] = pFileID;
            writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]);
        }
        criticalSection->Release();
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
#define SetNotchParameter(ParameterName, Side, Channel, GateIdx, NewVal)                    \
    {                                                                                       \
        unsigned int cidx;                                                                  \
        VERIFY_GATE_IDX_LOWMAX;                                                             \
        if ((tbl) && (tbl->IndexByCID(&cidx, Channel) == true)) {                           \
            criticalSection->Enter();                                                       \
            setChannelCalibrationToDefaultData(Side, cidx);                                 \
            if ((List[FIndex]).ReadOnly == false) {                                         \
                List[FIndex].List[Side][cidx].ParameterName[GateIdx] = NewVal;              \
                writeChannelCalibrationPatch(FIndex, Side, &List[FIndex].List[Side][cidx]); \
                criticalSection->Release();                                                 \
            }                                                                               \
        }                                                                                   \
    }
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::SetNotchStart(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)
{
    VERIFY_CID_IDX;
    SetNotchParameter(NotchStart, Side, Channel, GateIdx, NewVal);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::SetNotchDur(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)
{
    VERIFY_CID_IDX;
    SetNotchParameter(NotchDur, Side, Channel, GateIdx, NewVal);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::SetNotchLevel(eDeviceSide Side, CID Channel, int GateIdx, int NewVal)
{
    VERIFY_CID_IDX;
    SetNotchParameter(NotchLevel, Side, Channel, GateIdx, NewVal);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
uint32_t cDeviceCalibration::readLE32U(volatile uint8_t* pmem)
{
    DEFCORE_ASSERT(pmem != NULL);
    uint32_t val = 0;

    ((uint8_t*) &val)[0] = pmem[0];
    ((uint8_t*) &val)[1] = pmem[1];
    ((uint8_t*) &val)[2] = pmem[2];
    ((uint8_t*) &val)[3] = pmem[3];
    return (val);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::writeLE32U(volatile uint8_t* pmem, uint32_t val)
{
    DEFCORE_ASSERT(pmem != NULL);
    pmem[0] = ((uint8_t*) &val)[0];
    pmem[1] = ((uint8_t*) &val)[1];
    pmem[2] = ((uint8_t*) &val)[2];
    pmem[3] = ((uint8_t*) &val)[3];
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// читает из файла данные о каналах, относящихся к side, для всех настроек
int cDeviceCalibration::readCalibrationsSetFromFile(eDeviceSide side, std::ifstream& file, unsigned int fileFormatVersion)
{
    uint32_t ii = 0, a1 = 0, b = 0, c = 0;
    int res = ERROR_FILE_READ_FAILED;

    // читаем количество настроек, в которых есть каналы side
    if (file.good()) {
        if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {
            if (a1) {
                for (ii = 0; ii < a1; ++ii) {
                    if (fRead(&b, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {      // индекс настройки
                        if (fRead(&c, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {  // прочитано количество каналов
                            if (readCalibrationsFromFile(&((List[ii]).List[side]), file, c, fileFormatVersion) != 0) break;
                        }
                    }
                }
                if (ii == a1) {
                    res = ERROR_NO;
                }
            }
            else
                res = ERROR_NO;
        }
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " bad file!" << std::endl;
    }
    return res;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// читает ammountOfchannelCalibration настроек каналов и помещает их в список настроек каналов соответствующих eDeviceSide
int cDeviceCalibration::readCalibrationsFromFile(std::vector<sChannelCalibration>* pCalibration, std::ifstream& file, unsigned int ammountOfChannelCalibration, unsigned int fileFormatVersion)
{
    DEFCORE_ASSERT(pCalibration != NULL);
    tSChannelCalibration tmp;
    int res = ERROR_NO;
    unsigned int ii = 0;
    uint32_t a = 0;
    tSChannelCalibrationVer2 oldTmp2;
    tSChannelCalibrationVer3 oldTmp3;
    tSChannelCalibrationVer4 oldTmp4;
    tSChannelCalibrationVer5 oldTmp5;
    unsigned int structureSize = 0;
    unsigned char* pTmp = NULL;
    bool result = false;

    switch (fileFormatVersion) {
    case 1:
    case 2:
        structureSize = sizeof(tSChannelCalibrationVer2);
        pTmp = (unsigned char*) &oldTmp2;
        break;
    case 3:
        structureSize = sizeof(tSChannelCalibrationVer3);
        pTmp = (unsigned char*) &oldTmp3;
        break;
    case 4:
        structureSize = sizeof(tSChannelCalibrationVer4);
        pTmp = (unsigned char*) &oldTmp4;
        break;
    case 5:
        structureSize = sizeof(tSChannelCalibrationVer5);
        pTmp = (unsigned char*) &oldTmp5;
        break;
    default:
        structureSize = sizeof(tSChannelCalibration);
        pTmp = (unsigned char*) &tmp;
    }
    //
    for (ii = 0; ii < ammountOfChannelCalibration; ++ii)
        if (fRead(pTmp, 1, structureSize, file) == structureSize) {
            switch (fileFormatVersion) {
            case 1:
            case 2:
                result = tbl->IndexByCID(&a, oldTmp2.ID);
                convertCalibrationFormat(&(*pCalibration)[a], &oldTmp2);
                break;
            case 3:
                result = tbl->IndexByCID(&a, oldTmp3.ID);
                convertCalibrationFormat(&(*pCalibration)[a], &oldTmp3);
                break;
            case 4:
                result = tbl->IndexByCID(&a, oldTmp4.ID);
                convertCalibrationFormat(&(*pCalibration)[a], &oldTmp4);
                break;
            case 5:
                result = tbl->IndexByCID(&a, oldTmp5.ID);
                convertCalibrationFormat(&(*pCalibration)[a], &oldTmp5);
                break;
            default:
                result = tbl->IndexByCID(&a, tmp.ID);
                convertCalibrationFormat(&(*pCalibration)[a], &tmp);
            }

            if (result == true)
                ((*pCalibration)[a]).used = true;
            else {
                res = ERROR_BAD_DATA;
                break;
            }
        }
        else {
            res = ERROR_FILE_READ_FAILED;
            break;
        }
    return res;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// записывает данные о каналах, относящихся к side, для всех настроек
int cDeviceCalibration::writeCalibrationsSetToFile(std::ofstream& file, eDeviceSide side, uint32_t* pSum)
{
    DEFCORE_ASSERT(pSum != NULL);
    uint32_t ii = 0, jj = 0, kk = 0, a1 = 0;
    int res = ERROR_FILE_WRITE_FAILED;
    // пишем число настроек, в которых есть каналы side
    a1 = List.size();
    for (ii = 0, jj = 0; ii < a1; ++ii) {
        if (countUsedChannels(ii, side)) jj++;
    }
    if (fWrite(&jj, sizeof(uint32_t), file, pSum) == 0) {
        if (jj) {
            for (ii = 0; ii < a1; ++ii) {
                kk = countUsedChannels(ii, side);
                if (kk) {
                    if (fWrite(&ii, sizeof(uint32_t), file, pSum) == 0)  // индекс настройки
                    {
                        if (fWrite(&kk, sizeof(uint32_t), file, pSum) == 0)  // число настроек каналов
                        {
                            for (kk = 0; kk < tbl->Count(); kk++) {
                                if (((List[ii]).List[side][kk]).used)
                                    if (writeChannelCalibrationToFile(file, &(List[ii]).List[side][kk], pSum) != 0) break;
                            }
                            if (kk != static_cast<uint32_t>(tbl->Count())) break;  // TODO: consider revising, old line: [ if (kk != (uint32_t)((cChannelsTable*)tbl)->Count()) break; ]
                        }
                    }
                }
            }
            if (ii == a1) res = ERROR_NO;
        }
        else {
            res = ERROR_NO;
        }
    }
    return res;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::convertCalibrationFormat(tSChannelCalibration* pDest, sChannelCalibration* pCalibration)
{
    DEFCORE_ASSERT(pDest != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    pDest->ID = (uint32_t) pCalibration->ID;
    pDest->TVG = (uint32_t) pCalibration->TVG;
    pDest->PrismDelay = (uint32_t) pCalibration->PrismDelay;
    for (unsigned char ii = 0; ii <= maxGateLowIndex; ++ii) {
        pDest->Sens[ii] = (uint32_t) pCalibration->Sens[ii];
        pDest->Gain[ii] = (uint32_t) pCalibration->Gain[ii];
        if (pCalibration->Calibrated[ii])
            pDest->Calibrated[ii] = 1;
        else
            pDest->Calibrated[ii] = 0;
        pDest->NotchStart[ii] = (uint32_t) pCalibration->NotchStart[ii];
        pDest->NotchDur[ii] = (uint32_t) pCalibration->NotchDur[ii];
        pDest->NotchLevel[ii] = (uint32_t) pCalibration->NotchLevel[ii];
    }
    for (unsigned char ii = 0; ii <= maxGateHighIndex; ++ii) {
        pDest->StGate[ii] = (uint32_t) pCalibration->StGate[ii];
        pDest->EdGate[ii] = (uint32_t) pCalibration->EdGate[ii];
    }
    pDest->Mark = (uint32_t) pCalibration->Mark;
    memset(&pDest->DataTime, 0, sizeof(tCreationTime));
    memcpy(&pDest->DataTime, &pCalibration->DataTime, sizeof(double));
    pDest->CalibrationTemperatureValue = pCalibration->CalibrationTemperatureValue;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pDest->SnapshotFileID[ii] = pCalibration->SnapshotFileID[ii];
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibration* pSource)
{
    DEFCORE_ASSERT(pSource != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    pCalibration->ID = (int) pSource->ID;
    pCalibration->TVG = (int) pSource->TVG;
    pCalibration->PrismDelay = (int) pSource->PrismDelay;
    for (unsigned char ii = 0; ii <= maxGateLowIndex; ++ii) {
        pCalibration->Sens[ii] = (int) pSource->Sens[ii];
        pCalibration->Gain[ii] = (int) pSource->Gain[ii];
        if (pSource->Calibrated[ii])
            pCalibration->Calibrated[ii] = true;
        else
            pCalibration->Calibrated[ii] = false;
        pCalibration->NotchStart[ii] = (int) pSource->NotchStart[ii];
        pCalibration->NotchDur[ii] = (int) pSource->NotchDur[ii];
        pCalibration->NotchLevel[ii] = (int) pSource->NotchLevel[ii];
    }
    for (unsigned char ii = 0; ii <= maxGateHighIndex; ++ii) {
        pCalibration->StGate[ii] = (int) pSource->StGate[ii];
        pCalibration->EdGate[ii] = (int) pSource->EdGate[ii];
    }
    pCalibration->Mark = (int) pSource->Mark;
    memcpy(&pCalibration->DataTime, &pSource->DataTime, sizeof(double));
    pCalibration->CalibrationTemperatureValue = pSource->CalibrationTemperatureValue;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pCalibration->SnapshotFileID[ii] = pSource->SnapshotFileID[ii];
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer5* pSource)
{
    DEFCORE_ASSERT(pSource != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    pCalibration->ID = (int) pSource->ID;
    pCalibration->TVG = (int) pSource->TVG;
    pCalibration->PrismDelay = (int) pSource->PrismDelay;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pCalibration->Sens[ii] = (int) pSource->Sens[ii];
        pCalibration->Gain[ii] = (int) pSource->Gain[ii];
        if (pSource->Calibrated[ii])
            pCalibration->Calibrated[ii] = true;
        else
            pCalibration->Calibrated[ii] = false;
    }
    for (unsigned char ii = 0; ii < 6; ++ii) {
        pCalibration->StGate[ii] = (int) pSource->StGate[ii];
        pCalibration->EdGate[ii] = (int) pSource->EdGate[ii];
    }
    pCalibration->Mark = (int) pSource->Mark;
    memcpy(&pCalibration->DataTime, &pSource->DataTime, sizeof(double));
    pCalibration->CalibrationTemperatureValue = pSource->CalibrationTemperatureValue;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pCalibration->SnapshotFileID[ii] = pSource->SnapshotFileID[ii];
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer4* pSource)
{
    DEFCORE_ASSERT(pSource != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    pCalibration->ID = (int) pSource->ID;
    pCalibration->TVG = (int) pSource->TVG;
    pCalibration->PrismDelay = (int) pSource->PrismDelay;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pCalibration->Sens[ii] = (int) pSource->Sens[ii];
        pCalibration->Gain[ii] = (int) pSource->Gain[ii];
        if (pSource->Calibrated[ii])
            pCalibration->Calibrated[ii] = true;
        else
            pCalibration->Calibrated[ii] = false;
    }
    for (unsigned char ii = 0; ii < 6; ++ii) {
        pCalibration->StGate[ii] = (int) pSource->StGate[ii];
        pCalibration->EdGate[ii] = (int) pSource->EdGate[ii];
    }
    pCalibration->Mark = (int) pSource->Mark;
    memcpy(&pCalibration->DataTime, &pSource->DataTime, sizeof(double));
}

//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer3* pSource)
{
    DEFCORE_ASSERT(pSource != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    pCalibration->ID = (int) pSource->ID;
    pCalibration->TVG = (int) pSource->TVG;
    pCalibration->PrismDelay = (int) pSource->PrismDelay;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pCalibration->Sens[ii] = (int) pSource->Sens[ii];
        pCalibration->Gain[ii] = (int) pSource->Gain[ii];
        if (pSource->Calibrated[ii])
            pCalibration->Calibrated[ii] = true;
        else
            pCalibration->Calibrated[ii] = false;
    }
    for (unsigned char ii = 0; ii < 5; ++ii) {
        pCalibration->StGate[ii] = (int) pSource->StGate[ii];
        pCalibration->EdGate[ii] = (int) pSource->EdGate[ii];
    }
    pCalibration->Mark = (int) pSource->Mark;
    memcpy(&pCalibration->DataTime, &pSource->DataTime, sizeof(double));
}
//-----------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::convertCalibrationFormat(sChannelCalibration* pCalibration, tSChannelCalibrationVer2* pSource)
{
    DEFCORE_ASSERT(pSource != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    pCalibration->ID = (int) pSource->ID;
    pCalibration->TVG = (int) pSource->TVG;
    pCalibration->PrismDelay = (int) pSource->PrismDelay;
    for (unsigned char ii = 0; ii < 2; ++ii) {
        pCalibration->Sens[ii] = (int) pSource->Sens[ii];
        pCalibration->Gain[ii] = (int) pSource->Gain[ii];
        if (pSource->Calibrated[ii])
            pCalibration->Calibrated[ii] = true;
        else
            pCalibration->Calibrated[ii] = false;
    }
    for (unsigned char ii = 0; ii < 4; ++ii) {
        pCalibration->StGate[ii] = (int) pSource->StGate[ii];
        pCalibration->EdGate[ii] = (int) pSource->EdGate[ii];
    }
    pCalibration->Mark = (int) pSource->Mark;
    memcpy(&pCalibration->DataTime, &pSource->DataTime, sizeof(double));
}
//----------------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::writeChannelCalibrationToFile(std::ofstream& file, sChannelCalibration* pCalibration, uint32_t* pSum)
{
    DEFCORE_ASSERT(pSum != NULL);
    DEFCORE_ASSERT(pCalibration != NULL);
    tSChannelCalibration tmp;
    memset(&tmp, 0, sizeof(tmp));
    convertCalibrationFormat(&tmp, pCalibration);  // TODO: some fields were not copied
    return fWrite((char*) &tmp, sizeof(tSChannelCalibration), file, pSum);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::setChannelCalibrationToDefaultData(eDeviceSide Side, unsigned int channelIdx)  // устанавливает значения полей из описателя канала
{
    sChannelDescription chanDesc;
    std::vector<sChannelCalibration>* pCalibration = &((List[FIndex]).List[Side]);
    sChannelCalibration& currentCalibration = (*pCalibration)[channelIdx];

    if (currentCalibration.used == false) {
        CID Channel;
        if ((tbl->CIDByIndex(&Channel, channelIdx) == true) && (tbl->ItemByCID(Channel, &chanDesc) == true)) {
            currentCalibration.ID = Channel;
            currentCalibration.Gain[0] = currentCalibration.Gain[1] = chanDesc.DefaultGain;  // Значение АТТ по умолчанию
            currentCalibration.PrismDelay = chanDesc.DefaultPrismDelay;                      // Значение 2Тп по умолчанию
            currentCalibration.TVG = chanDesc.DefaultTVG;                                    // Значение ВРЧ по умолчанию
            currentCalibration.Calibrated[0] = currentCalibration.Calibrated[1] = false;
            currentCalibration.StGate[0] = chanDesc.RecommendedAlarmGate[0].gStart;
            currentCalibration.EdGate[0] = chanDesc.RecommendedAlarmGate[0].gEnd;
            currentCalibration.StGate[1] = chanDesc.RecommendedAlarmGate[1].gStart;
            currentCalibration.EdGate[1] = chanDesc.RecommendedAlarmGate[1].gEnd;
            for (int ii = 0; ii <= maxGateLowIndex; ++ii) {
                currentCalibration.Sens[ii] = chanDesc.RecommendedSens[ii];
                currentCalibration.SnapshotFileID[ii] = cSnapshortFileIdUnknown;
            }
            currentCalibration.CalibrationTemperatureValue = cTemperatureValueUnknown;
            currentCalibration.NotchStart[0] = currentCalibration.NotchStart[1] = 0;
            currentCalibration.NotchDur[0] = currentCalibration.NotchDur[1] = 0;
            currentCalibration.NotchLevel[0] = currentCalibration.NotchLevel[1] = 0;
            currentCalibration.used = true;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
//   >0 - буфер был выделен и блок данных считан, указатель в файле соответственно передвинулся
//  в случае ошибки, если  память под буфер (еще )не была веделена, то *destBufferAddress = NULL
int cDeviceCalibration::getCalibrationsFromFile(uint8_t** destBufferAddress, std::ifstream& file, unsigned int fileFormatVersion)
{
    uint32_t a[2];
    int res = ERROR_FILE_READ_FAILED;
    uint8_t* p = NULL;
    unsigned int structureSize = 0;

    if (destBufferAddress) {
        *destBufferAddress = NULL;
    }


    if (fRead(a, 1, sizeof(uint32_t) * 2, file) == sizeof(uint32_t) * 2) {
        // a[0]   - индекс имени настройки
        // a[1]   - количество каналов
        switch (fileFormatVersion) {
        case 1:
        case 2:
            structureSize = sizeof(tSChannelCalibrationVer2);
            break;
        case 3:
            structureSize = sizeof(tSChannelCalibrationVer3);
            break;
        case 4:
            structureSize = sizeof(tSChannelCalibrationVer4);
            break;
        case 5:
            structureSize = sizeof(tSChannelCalibrationVer5);
            break;
        default:
            structureSize = sizeof(tSChannelCalibration);
        }
        p = (uint8_t*) malloc(2 * sizeof(uint32_t) + a[1] * structureSize);
        if (p) {
            memcpy(p, a, sizeof(uint32_t) * 2);
            uint8_t* poffs = p;
            poffs += sizeof(uint32_t) * 2;
            if (fRead(poffs, 1, a[1] * structureSize, file) == a[1] * structureSize) res = 2 * sizeof(uint32_t) + a[1] * structureSize;
            if (destBufferAddress) *destBufferAddress = p;
        }
        else {
            res = ERROR_MEMALLOC_FAILED;
        }
    }
    return res;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
uint32_t cDeviceCalibration::sumcalc(uint8_t* blockStartAddr, uint32_t blockSize, uint32_t initalSumValue)
{
    assert(blockStartAddr != NULL);
    uint32_t sum = initalSumValue;
    for (uint32_t ii = 0; ii < blockSize; ++ii) {
        uint32_t a = sum;
        sum += blockStartAddr[ii];
        if (sum < a) sum++;
    }
    return static_cast<uint32_t>(sum);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
//  открывает файл и считает контрольную сумму файла
//  возвращает коды ошибок, в т.ч.:
// ERROR_FILE_NOTFOUND если невозможно открыть файл
// ERROR_ARCH_MISMATCH - представления типов данных double и integer в файле не соответствуют используемой архитектуре
// ERROR_BAD_DATA - не совпадает сигнатура основного блока
// при завершении без ошибок в *pDataCreationTime возвращает время создания файла; если
// установлен флаг fNotToClose, файл не закрывается, а в *fHandle записывается дескриптор

int cDeviceCalibration::validateFileMainData(std::ifstream& inputFile, double* pDataCreationTime, bool fNotToClose)
{
    return validateFileMainData(inputFile, pDataCreationTime, fNotToClose, NULL);
}
//----------------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::validateFileMainData(std::ifstream& file, double* pDataCreationTime, bool fNotToClose, unsigned int* pFileFormatVersion)
{
    uint32_t a1 = 0, ii = 0, jj = 0;
    int res = ERROR_FILE_NOTFOUND;
    uint32_t sum = 0;
    double t = 0.0;
    uint32_t fileFormat = 0;

#ifdef DEFCORE_OS_LINUX
    sync();
#endif
    file.seekg(0);
    if (file.good()) {
        res = ERROR_FILE_READ_FAILED;
        if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {  // версия формата файла
            if (pFileFormatVersion) {
                *pFileFormatVersion = static_cast<unsigned int>(a1);
            }
            if ((a1 != 0) && (a1 <= currentFileFormatVersion)) {
                fileFormat = a1;
                sum = sumcalc((uint8_t*) &a1, sizeof(uint32_t), sum);
                if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {
                    if (a1 == sizeof(int)) {
                        sum = sumcalc((uint8_t*) &a1, sizeof(uint32_t), sum);
                        if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {
                            if (a1 == sizeof(double)) {
                                sum = sumcalc((uint8_t*) &a1, sizeof(uint32_t), sum);
                                if (fRead(&t, 1, sizeof(double), file) == sizeof(double)) {
                                    sum = sumcalc((uint8_t*) &t, sizeof(double), sum);
                                    if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t))  // сигнатура основного блока
                                    {
                                        if (a1 != (uint32_t) mainDataSignature) {
                                            res = ERROR_BAD_DATA;
                                            std::cerr << "===== ERROR:" << __FUNCTION__ << " bad data!" << std::endl;
                                        }
                                        else {
                                            sum = sumcalc((uint8_t*) &a1, sizeof(uint32_t), sum);
                                            // чтение блока свойств настроек
                                            if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {  // число настроек
                                                sum = sumcalc((uint8_t*) &a1, sizeof(uint32_t), sum);

                                                if (a1) {
                                                    if (fileFormat > 1) {
                                                        uint32_t tmp;
                                                        tCalibrationsProperty2 a;
                                                        ii = 0;
                                                        if (fRead(&tmp, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {  // индекс текущей настройки
                                                            sum = sumcalc((uint8_t*) &tmp, sizeof(uint32_t), sum);
                                                            for (ii = 0; ii < a1; ++ii) {
                                                                if (fRead(&a, 1, sizeof(tCalibrationsProperty2), file) == sizeof(tCalibrationsProperty2)) {
                                                                    sum = sumcalc((uint8_t*) &a, sizeof(tCalibrationsProperty2), sum);
                                                                }
                                                                else
                                                                    break;
                                                            }
                                                        }
                                                    }
                                                    else
                                                        for (ii = 0; ii < a1; ++ii) {  //  формат файла версии 1
                                                            tCalibrationsProperty a;
                                                            if (fRead(&a, 1, sizeof(tCalibrationsProperty), file) == sizeof(tCalibrationsProperty)) {
                                                                sum = sumcalc((uint8_t*) &a, sizeof(tCalibrationsProperty), sum);
                                                            }
                                                            else
                                                                break;
                                                        }
                                                }
                                                //
                                                if ((a1) && (ii == a1)) {
                                                    // последовательно читаем данные для каналов "без стороны",левой стороны, правой
                                                    for (jj = 0; jj < 3; jj++) {
                                                        uint32_t a2;
                                                        if (fRead(&a2, 1, sizeof(uint32_t), file) == sizeof(uint32_t))  // число настроек
                                                        {
                                                            sum = sumcalc((uint8_t*) &a2, sizeof(uint32_t), sum);
                                                            for (ii = 0; ii < a2; ++ii) {
                                                                uint8_t* pBuffer;
                                                                int result = getCalibrationsFromFile(&pBuffer, file, fileFormat);
                                                                if (result > 0) sum = sumcalc(pBuffer, result, sum);
                                                                if (pBuffer) free(pBuffer);
                                                            }
                                                            //										   if (result < (int)ERROR_NO) break;
                                                        }
                                                        if (ii != a2) break;
                                                    }
                                                }
                                            }
                                            if ((a1 == 0) || ((a1) && (jj == 3))) {
                                                if (fRead(&a1, 1, sizeof(uint32_t), file) == sizeof(uint32_t))  // контрольная сумма
                                                {
                                                    if (a1 == ~sum) {
                                                        res = ERROR_NO;
                                                    }
                                                    else {
                                                        std::cerr << "===== ERROR:" << __FUNCTION__ << " bad crc!" << std::endl;
                                                        res = ERROR_BAD_SUM;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            std::cerr << "===== ERROR:" << __FUNCTION__ << " arch mismatch 1!" << std::endl;
                            res = ERROR_ARCH_MISMATCH;
                        }
                    }
                }
                else {
                    std::cerr << "===== ERROR:" << __FUNCTION__ << " arch mismatch 2!" << std::endl;
                    res = ERROR_ARCH_MISMATCH;
                }
            }
            else {
                std::cerr << "===== ERROR:" << __FUNCTION__ << " bad fileversion!" << std::endl;
                res = ERROR_BAD_FILEVERSION;
            }
        }
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }
    if ((res == ERROR_NO) && (pDataCreationTime)) {
        *pDataCreationTime = t;
    }

    if ((res == ERROR_NO) && (fNotToClose)) {
    }
    else {
        file.close();
    }

#ifdef DEFCORE_OS_LINUX
    sync();
#endif
    return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// считает число используемых в настройке каналов, относящихся к стороне sideSign
//
uint32_t cDeviceCalibration::countUsedChannels(int calibrationsIndex, eDeviceSide side)
{
    uint32_t ii, jj;
    for (ii = 0, jj = 0; ii < (tbl)->Count(); ++ii)
    //	for(ii=0, jj=0; ii<tmp;++ii)
    {
        if (((List[calibrationsIndex]).List[side][ii]).used) jj++;
    }
    return jj;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// записывает блок данных  в файл, в *pSum обновляется контрольная сумма
int cDeviceCalibration::fWrite(const void* pBuffer, uint32_t size, std::ofstream& file, uint32_t* pSum)
{
    assert(pSum != NULL);
    assert(pBuffer != NULL);
    *pSum = sumcalc((uint8_t*) pBuffer, size, *pSum);
    file.write(reinterpret_cast<const char*>(pBuffer), size);
    if (file.good()) {
        return 0;
    }
    else {
        return -1;
    }
}

int cDeviceCalibration::fRead(void* pBuffer, uint32_t blocks, uint32_t size, std::ifstream& file)
{
    DEFCORE_ASSERT(pBuffer);
    file.read(reinterpret_cast<char*>(pBuffer), blocks * size);
    if (file.good()) {
        return blocks * size;
    }
    else {
        return 0;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------
// перезаписывает в файл заголовок и основной блок
int cDeviceCalibration::saveMainDataToFile(const std::string& fileName)  // Сохранить данные в файл
{
    assert(!fileName.empty());

    int res = ERROR_FILE_NOTFOUND;

#ifdef DEFCORE_OS_LINUX
    sync();
#endif

    std::ofstream outfile(fileName.c_str(), std::ios_base::out | std::ios_base::binary);
    if (outfile.good()) {
        res = saveMainDataToFile(outfile);
        outfile.flush();
        outfile.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }

#ifdef DEFCORE_OS_LINUX
    sync();
#endif

    return res;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// записывает в файл заголовок и основной блок
int cDeviceCalibration::saveMainDataToFile(std::ofstream& file)  // Сохранить данные в файл
{
    uint32_t a1 = 0, sum = 0, ii = 0;
    int res = ERROR_FILE_WRITE_FAILED;
    sum = 0;

#ifdef DEFCORE_OS_LINUX
    sync();
#endif
    a1 = currentFileFormatVersion;

    if (file.good()) {
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            a1 = sizeof(int);
            if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                a1 = sizeof(double);
                if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                    if (fWrite(&alterationTime, sizeof(double), file, &sum) == 0)  // время создания файла
                    {
                        // сигнатура основного блока
                        a1 = mainDataSignature;
                        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                            // записываем основной блок
                            a1 = List.size();
                            if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                                int result = 0;

                                if (a1 != 0) {  //индекс текущей настройки - в версии файла > 1
                                    uint32_t tmp = FIndex;
                                    result = fWrite(&tmp, sizeof(uint32_t), file, &sum);
                                }
                                if (result == 0) {
                                    tCalibrationsProperty2 a;
                                    for (ii = 0; ii < a1; ++ii) {  // пишем  свойства настроек
                                        memset(&a, 0, sizeof(tCalibrationsProperty2));
                                        strcpy(a.Name, List[ii].Name);
                                        a.scheme = List[ii].scheme;
                                        if (List[ii].ReadOnly) a.ReadOnly = 1;
                                        if (fWrite(&a, sizeof(tCalibrationsProperty2), file, &sum) != 0) break;
                                    }
                                }
                                if ((a1) && (ii == a1)) {
                                    if ((writeCalibrationsSetToFile(file, dsNone, &sum) == 0) && (writeCalibrationsSetToFile(file, dsLeft, &sum) == 0) && (writeCalibrationsSetToFile(file, dsRight, &sum) == 0)) res = ERROR_NO;
                                }

                                if ((a1 == 0) || ((a1) && (res == ERROR_NO))) {
                                    sum = ~sum;
                                    file.write(reinterpret_cast<const char*>(&sum), sizeof(int32_t));
                                    res = ERROR_NO;
                                }
                            }
                        }
                    }
                }
            }
        }
        file.flush();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }

#ifdef DEFCORE_OS_LINUX
    sync();
#endif
    return res;
}

//--------------------------------------------------------------------------------------------------------------------------------------
// открывает файл и заполняет класс данными из основного блока
// при завершении без ошибок если
// установлен флаг fNotToClose, файл не закрывается, а в *fHandle записывается дескриптор

int cDeviceCalibration::getMainDataFromFile(std::ifstream& file, bool fNotToClose)
{
    struct datFileHeader header;
    memset(&header, 0x00, sizeof(datFileHeader));
    uint32_t a1 = 0, ii = 0;
    int res = ERROR_FILE_NOTFOUND;

#ifdef DEFCORE_OS_LINUX
    sync();
#endif

    file.seekg(0);
    if (file.good()) {
        res = ERROR_FILE_READ_FAILED;
        if (fRead(&header, 1, sizeof(header), file) == sizeof(header)) {
            int result = ERROR_NO;
            if (fRead(&a1, 1, sizeof(int32_t), file) == sizeof(int32_t)) {
                if (fRead(&a1, 1, sizeof(int32_t), file) == sizeof(int32_t)) {  // число настроек

                    for (ii = 0; ii < a1; ++ii) createNewAction();
                    if (a1) {
                        uint32_t tmp;
                        setCurrentAction(0);
                        if (header.fileVersion > 1) {
                            if (fRead(&tmp, 1, sizeof(int32_t), file) == sizeof(int32_t)) {
                                setCurrentAction(tmp);
                            }
                        }
                        else {                    // формат версии  1
                            fDataChanged = true;  //
                            setCurrentAction(0);
                        }
                        if (result == ERROR_NO)  // TODO: condition always true
                        {
                            for (ii = 0; ii < a1; ++ii)
                                if (header.fileVersion > 1) {
                                    tCalibrationsProperty2 a;
                                    if (fRead(&a, 1, sizeof(tCalibrationsProperty2), file) == sizeof(tCalibrationsProperty2)) {
                                        strcpy((List[ii]).Name, a.Name);
                                        List[ii].scheme = a.scheme;
                                        if (a.ReadOnly) {
                                            (List[ii]).ReadOnly = true;
                                        }
                                        else {
                                            (List[ii]).ReadOnly = false;
                                        }
                                    }
                                }
                                else {  // формат файла версии 1
                                    tCalibrationsProperty a;
                                    if (fRead(&a, 1, sizeof(tCalibrationsProperty), file) == sizeof(tCalibrationsProperty)) {
                                        strcpy((List[ii]).Name, a.Name);
                                        if (a.ReadOnly) {
                                            (List[ii]).ReadOnly = true;
                                        }
                                        else {
                                            (List[ii]).ReadOnly = false;
                                        }
                                    }
                                }
                            if ((readCalibrationsSetFromFile(dsNone, file, header.fileVersion) == 0) && (readCalibrationsSetFromFile(dsLeft, file, header.fileVersion) == 0) && (readCalibrationsSetFromFile(dsRight, file, header.fileVersion) == 0)) res = ERROR_NO;
                        }
                    }
                    else {
                        setCurrentAction(0);
                        res = ERROR_NO;
                    }
                    if ((res == ERROR_NO) && (fRead(&a1, 1, sizeof(int32_t), file) == sizeof(int32_t))) {
                        res = ERROR_NO;
                    }  // контрольная сумма
                }
            }
        }
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }

    if ((res == ERROR_NO) && (fNotToClose)) {
    }
    else {
        file.close();
    }

#ifdef DEFCORE_OS_LINUX
    sync();
#endif

    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::updateDataFromPatch(std::ifstream& file)
{
    PATCH patch;
    int res = readPatchFromFile((uint8_t*) &patch, file);

    if (res == ERROR_NO) {
        ChannelCalibrationPatch* pp;
        pp = (ChannelCalibrationPatch*) &patch.patch;
        memcpy(&alterationTime, &pp->creationTime, sizeof(double));
        switch (patch.sign) {
        case channelCalibrationPatchSignVer5: {
            channelCalibrationPatchApply(pp);
            break;
        }
        case channelCalibrationPatchSignVer4: {
            ChannelCalibrationPatchVer4* old4Pp;
            old4Pp = (ChannelCalibrationPatchVer4*) &patch.patch;
            channelCalibrationPatchApply(old4Pp);
            break;
        }
        case channelCalibrationPatchSignVer3: {
            ChannelCalibrationPatchVer3* old3Pp;
            old3Pp = (ChannelCalibrationPatchVer3*) &patch.patch;
            channelCalibrationPatchApply(old3Pp);
            break;
        }
        case channelCalibrationPatchSignVer2: {
            ChannelCalibrationPatchVer2* oldPp;
            oldPp = (ChannelCalibrationPatchVer2*) &patch.patch;
            channelCalibrationPatchApply(oldPp);
            break;
        }
        //
        case namePatchSign: {
            NamePatch* p = (NamePatch*) &patch.patch;
            setNameAction(p->calibrationIndex, p->newCalibrationName);
            break;
        }
        //
        case createNewPatchSign: {
            createNewAction();
            break;
        }
        //
        case createFromPatchSign: {
            createFromAction(patch.createFromPatch.calibrationIndex);
            break;
        }
        case deletePatchSign: {
            deleteAction(patch.deletePatch.calibrationIndex, true);
            break;
        }
        case delete2PatchSign: {
            deleteAction(patch.delete2Patch.calibrationIndex, (bool) patch.delete2Patch.toCreateTheOne);
            break;
        }
        case readOnlyPatchSign: {
            defineReadOnlyAction(patch.readOnlyPatch.calibrationIndex, patch.readOnlyPatch.state);
            break;
        }
        case setCurrentPatchSign: {
            setCurrentAction(patch.setCurrentPatch.calibrationIndex);
            break;
        }
        case setSchemePatchSign: {
            setSchemeAction(patch.setSchemePatch.calibrationIndex, patch.setSchemePatch.scheme);
            break;
        }
        }
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::getNextPatchTime(double* pPatchTime, std::ifstream& fileStream)
{
    PATCH patch;
    ChannelCalibrationPatch* pp;
    int res = readPatchFromFile((uint8_t*) &patch, fileStream);
    if (res == ERROR_NO) {
        pp = (ChannelCalibrationPatch*) &patch.patch;
        if (pPatchTime) {
            memcpy(pPatchTime, &pp->creationTime, sizeof(double));
        }
    }
    return res;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// читает очередной патч (с сигнатурой) в буфер
int cDeviceCalibration::readPatchFromFile(uint8_t* pbuf, std::ifstream& file)
{
    DEFCORE_ASSERT(pbuf != NULL);
    uint32_t a = 0, b = 0;
    int res = ERROR_FILE_READ_FAILED;

    if (fRead(&a, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {
        uint32_t sum = 0;
        sum = sumcalc((uint8_t*) &a, sizeof(uint32_t), sum);
        writeLE32U(pbuf, a);
        pbuf += sizeof(uint32_t);
        switch (a) {
        case channelCalibrationPatchSignVer5:
            b = sizeof(ChannelCalibrationPatch);
            break;
        case channelCalibrationPatchSignVer4:
            b = sizeof(ChannelCalibrationPatchVer4);
            break;
        case channelCalibrationPatchSignVer3:
            b = sizeof(ChannelCalibrationPatchVer3);
            break;
        case channelCalibrationPatchSignVer2:
            b = sizeof(ChannelCalibrationPatchVer2);
            break;
        case namePatchSign:
            b = sizeof(NamePatch);
            break;
        case createNewPatchSign:
            b = sizeof(CreateNewPatch);
            break;
        case createFromPatchSign:
            b = sizeof(CreateFromPatch);
            break;
        case deletePatchSign:
            b = sizeof(DeletePatch);
            break;
        case delete2PatchSign:
            b = sizeof(Delete2Patch);
            break;
        case readOnlyPatchSign:
            b = sizeof(ReadOnlyPatch);
            break;
        case setCurrentPatchSign:
            b = sizeof(SetCurrentPatch);
            break;
        case setSchemePatchSign:
            b = sizeof(SetSchemePatch);
            break;
        default:
            b = 0;
            // встретился неизвестный патч
            break;
        }
        if (b != 0) {
            if (fRead(pbuf, 1, b, file) == b) {
                sum = sumcalc(pbuf, b, sum);
                if (fRead(&b, 1, sizeof(uint32_t), file) == sizeof(uint32_t)) {  // считываем контрольную сумму
                    if (b == ~sum) {
                        res = ERROR_NO;
                    }
                    else {
                        res = ERROR_BAD_SUM;
                        std::cerr << "===== ERROR:" << __FUNCTION__ << " bad checksum!" << std::endl;
                    }
                }
            }
        }
        else {
            res = ERROR_BAD_DATA;  // неправильная сигнатура
            std::cerr << "===== ERROR:" << __FUNCTION__ << " bad data!" << std::endl;
        }
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
// создает новую настройку и делает созданную ее текущей
bool cDeviceCalibration::createNewAction()
{
    List.resize(List.size() + 1);
    setCurrentAction(List.size() - 1);
    ((List[FIndex]).List[0]).resize(tbl->Count());
    ((List[FIndex]).List[1]).resize(tbl->Count());
    ((List[FIndex]).List[2]).resize(tbl->Count());
    (List[FIndex]).Name[0] = 0;
    (List[FIndex]).ReadOnly = false;
    (List[FIndex]).scheme = 0;
    return true;
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::createFromAction(unsigned int Idx)
{
    bool res = false;
    if (Idx < List.size()) {
        createNewAction();
        List[FIndex] = List[Idx];
        res = true;
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::deleteAction(unsigned int Idx, bool toCreateTheOne)
{
    bool res = false;

    if (Idx < List.size() && ((List[Idx]).ReadOnly == false)) {
        ((List[Idx]).List[dsNone]).clear();
        ((List[Idx]).List[dsLeft]).clear();
        ((List[Idx]).List[dsRight]).clear();
        std::vector<sChannelsCalibration>::iterator it = List.begin() + Idx;
        List.erase(it);
        if (FIndex >= List.size()) {
            FIndex = List.size() - 1;
        }
        res = true;
        if (List.empty() && toCreateTheOne == true) {
            createNewAction();
        }
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::setNameAction(unsigned int Idx, char* newName)
{
    if (Idx < List.size() && ((List[Idx]).ReadOnly == false)) {
        strcpy(List[Idx].Name, newName);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::defineReadOnlyAction(unsigned int calibrationIndex, bool onState)
{
    if (List.size() > 0) {
        List[calibrationIndex].ReadOnly = onState;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::setCurrentAction(unsigned int idx)
{
    if ((List.size()) && (idx < List.size())) {
        FIndex = idx;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::setSchemeAction(unsigned int calibrationIndex, unsigned int scheme)
{
    if (List.size() > 0) {
        List[calibrationIndex].scheme = scheme;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::channelCalibrationPatchApply(ChannelCalibrationPatch* p)
{
    DEFCORE_ASSERT(p != NULL);
    unsigned int index = 0, side = 0, a = 0;

    index = p->calibrationIndex;
    side = p->side;
    if ((index < List.size()) && (side <= dsRight)) {
        if (tbl->IndexByCID(&a, p->cannelCalibration.ID)) {
            (List[index]).List[side][a].used = true;
            convertCalibrationFormat(&(List[index]).List[side][a], &p->cannelCalibration);
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::channelCalibrationPatchApply(ChannelCalibrationPatchVer4* p)
{
    DEFCORE_ASSERT(p != NULL);
    unsigned int index = 0, side = 0, a = 0;

    index = p->calibrationIndex;
    side = p->side;
    if ((index < List.size()) && (side <= dsRight)) {
        if (tbl->IndexByCID(&a, p->cannelCalibration.ID)) {
            (List[index]).List[side][a].used = true;
            convertCalibrationFormat(&(List[index]).List[side][a], &p->cannelCalibration);
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::channelCalibrationPatchApply(ChannelCalibrationPatchVer3* p)
{
    DEFCORE_ASSERT(p != NULL);
    unsigned int index = 0, side = 0, a = 0;

    index = p->calibrationIndex;
    side = p->side;
    if ((index < List.size()) && (side <= dsRight)) {
        if (tbl->IndexByCID(&a, p->cannelCalibration.ID)) {
            (List[index]).List[side][a].used = true;
            convertCalibrationFormat(&(List[index]).List[side][a], &p->cannelCalibration);
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::channelCalibrationPatchApply(ChannelCalibrationPatchVer2* p)
{
    DEFCORE_ASSERT(p != NULL);
    unsigned int index = 0, side = 0, a = 0;

    index = p->calibrationIndex;
    side = p->side;
    if ((index < List.size()) && (side <= dsRight)) {
        if (tbl->IndexByCID(&a, p->cannelCalibration.ID)) {
            (List[index]).List[side][a].used = true;
            convertCalibrationFormat(&(List[index]).List[side][a], &p->cannelCalibration);
        }
    }
}

std::string cDeviceCalibration::getWorkFileNameString()
{
    assert(pFileName);
    std::string fullFileName(pFileName);
    unsigned int dotPos = fullFileName.find_last_of(".");
    std::string baseFileName = fullFileName.substr(0, dotPos);
    baseFileName += "~.dat";

    return baseFileName;
}

std::string cDeviceCalibration::getStoreFileNameString()
{
    if (pFileName == NULL) {
        return std::string();
    }
    return extentFileNameString(pFileName, "~1");
}

std::string cDeviceCalibration::extentFileNameString(const std::string& source, const std::string& stringToAdd)
{
    unsigned int dotPos = source.find_last_of(".");
    std::string baseFileName = source.substr(0, dotPos);
    baseFileName += stringToAdd;
    baseFileName += ".dat";
    std::cerr << "Extended filename:" << baseFileName << std::endl;
    return baseFileName;
}

//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::getTime(tCreationTime* pTime)
{
    memset(pTime, 0, sizeof(tCreationTime));
    TDateTime currentTime = CurrentDateTime();
    memcpy(pTime, &currentTime, sizeof(double));
}
//---------------------------------------------------------------------------------------------------------------------------------------
// записывает патч CreateNew
int cDeviceCalibration::writeCreateNewActionPatch()
{
    uint32_t a1 = 0;
    uint32_t sum = 0;
    tCreationTime time;
    int res = ERROR_FILE_NOTFOUND;

    fDataChanged = true;

    std::ofstream file(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);


    if (file.good()) {
        res = ERROR_FILE_WRITE_FAILED;
        a1 = createNewPatchSign;
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            getTime(&time);
            if (fWrite(&time, sizeof(tCreationTime), file, &sum) == 0) {
                sum = ~sum;
                file.write(reinterpret_cast<char*>(&sum), sizeof(uint32_t));
                res = ERROR_NO;
            }
        }
        file.flush();
        file.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::writePatch(unsigned int patchSign, unsigned int calibrationIndex)
{
    UNUSED(patchSign);
    uint32_t a1 = 0;
    uint32_t sum = 0;
    tCreationTime time;
    int res = ERROR_FILE_NOTFOUND;

    fDataChanged = true;
    std::ofstream file(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

    if (file.good()) {
        res = ERROR_FILE_WRITE_FAILED;
        a1 = patchSign;
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            getTime(&time);
            if (fWrite(&time, sizeof(tCreationTime), file, &sum) == 0) {
                a1 = (uint32_t) calibrationIndex;
                if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                    sum = ~sum;
                    file.write(reinterpret_cast<char*>(&sum), sizeof(uint32_t));
                    res = ERROR_NO;
                }
            }
        }
        file.flush();
        file.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::writePatch(unsigned int patchSign, unsigned int calibrationIndex, unsigned int value)
{
    uint32_t a1 = 0;
    uint32_t sum = 0;
    tCreationTime time;
    int res = ERROR_FILE_NOTFOUND;

    fDataChanged = true;
    std::ofstream file(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);
    if (file.good()) {
        res = ERROR_FILE_WRITE_FAILED;
        a1 = patchSign;
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            getTime(&time);
            if (fWrite(&time, sizeof(tCreationTime), file, &sum) == 0) {
                a1 = (uint32_t) calibrationIndex;
                if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                    a1 = static_cast<uint32_t>(value);
                    if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                        sum = ~sum;
                        file.write(reinterpret_cast<char*>(&sum), sizeof(uint32_t));
                        res = ERROR_NO;
                    }
                }
            }
        }
        file.flush();
        file.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
// записывает патч CreateFrom
int cDeviceCalibration::writeCreateFromActionPatch(unsigned int calibrationIndex)
{
    return writePatch(createFromPatchSign, calibrationIndex);
}
//---------------------------------------------------------------------------------------------------------------------------------------------
// записывает патч Delete
int cDeviceCalibration::writeDeleteActionPatch(unsigned int calibrationIndex)
{
    return writePatch(deletePatchSign, calibrationIndex);
}
//---------------------------------------------------------------------------------------------------------------------------------------------
// записывает патч Delete2
int cDeviceCalibration::writeDeleteActionPatch(unsigned int calibrationIndex, bool toCreateTheOne)
{
    return writePatch(delete2PatchSign, calibrationIndex, (unsigned int) toCreateTheOne);
}

//---------------------------------------------------------------------------------------------------------------------------------------------
// записывает патч cannelCalibrationPatch
int cDeviceCalibration::writeChannelCalibrationPatch(unsigned int calibrationIndex, eDeviceSide side, sChannelCalibration* pCalibration)
{
    assert(pCalibration != NULL);
    uint32_t a1 = 0;
    uint32_t sum = 0;
    tCreationTime time;
    memset(&time, 0, sizeof(time));
    tSChannelCalibration tmp;
    memset(&tmp, 0, sizeof(tmp));
    int res = ERROR_FILE_NOTFOUND;

    fDataChanged = true;

    std::ofstream file(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

    if (file.good()) {
        res = ERROR_FILE_WRITE_FAILED;
        a1 = channelCalibrationPatchSignVer5;
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            getTime(&time);
            if (fWrite(&time, sizeof(tCreationTime), file, &sum) == 0) {
                a1 = (uint32_t) calibrationIndex;
                if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                    a1 = (uint32_t) side;
                    if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                        convertCalibrationFormat(&tmp, pCalibration);
                        if (fWrite(&tmp, sizeof(tSChannelCalibration), file, &sum) == 0) {
                            sum = ~sum;
                            file.write(reinterpret_cast<char*>(&sum), sizeof(uint32_t));
                            res = ERROR_NO;
                        }
                    }
                }
            }
        }
        file.flush();
        file.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " file not good!" << std::endl;
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::writeDefineReadOnlyActionPatch(unsigned int calibrationIndex, bool onState)
{
    if (onState) return writePatch(readOnlyPatchSign, calibrationIndex, 1);
    return writePatch(readOnlyPatchSign, calibrationIndex, 0);
}
//---------------------------------------------------------------------------------------------------------------------------------------
int cDeviceCalibration::writeNameActionPatch(unsigned int calibrationIndex, char* NewNameVal)
{
    uint32_t a1 = 0;
    uint32_t sum = 0;
    tCreationTime time;
    int res = ERROR_FILE_NOTFOUND;
    char newCalibrationName[256];  // неиспользуемые символы заполнены нулями

    fDataChanged = true;

    std::ofstream file(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

    if (file.good()) {
        res = ERROR_FILE_WRITE_FAILED;
        a1 = namePatchSign;
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            getTime(&time);
            if (fWrite(&time, sizeof(tCreationTime), file, &sum) == 0) {
                a1 = (uint32_t) calibrationIndex;
                if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                    memset(newCalibrationName, 0, 256);
                    strcpy(newCalibrationName, NewNameVal);
                    if (fWrite(newCalibrationName, sizeof(newCalibrationName), file, &sum) == 0) {
                        sum = ~sum;
                        file.write(reinterpret_cast<char*>(&sum), sizeof(uint32_t));
                        res = ERROR_NO;
                    }
                }
            }
        }
        file.flush();
        file.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " bad file!" << std::endl;
    }
    return res;
}
//----------------------------------------------------------------------------------------
int cDeviceCalibration::writeSetCurrentPatch(unsigned int value)
{
    uint32_t a1 = 0;
    uint32_t sum = 0;
    tCreationTime time;
    int res = ERROR_FILE_NOTFOUND;

    fDataChanged = true;

    std::ofstream file(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

    if (file.good()) {
        res = ERROR_FILE_WRITE_FAILED;
        a1 = setCurrentPatchSign;
        if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
            getTime(&time);
            if (fWrite(&time, sizeof(tCreationTime), file, &sum) == 0) {
                a1 = (uint32_t) value;
                if (fWrite(&a1, sizeof(uint32_t), file, &sum) == 0) {
                    sum = ~sum;
                    file.write(reinterpret_cast<char*>(&sum), sizeof(uint32_t));
                    res = ERROR_NO;
                }
            }
        }
        file.flush();
        file.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " bad file!" << std::endl;
    }
    return res;
}
//----------------------------------------------------------------------------------------
int cDeviceCalibration::writeSetSchemePatch(unsigned int calibrationIndex, unsigned int scheme)
{
    return writePatch(setSchemePatchSign, calibrationIndex, scheme);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::createWorkFile()
{
    std::ifstream file(pFileName, std::ios_base::in | std::ios_base::binary);
    std::ofstream workFile(getWorkFileNameString().c_str(), std::ios_base::out | std::ios_base::binary);

    size_t begin = file.tellg();
    file.seekg(0, std::ios::end);
    size_t end = file.tellg();
    file.close();
    size_t size = end - begin;

    if (size > 0) {
        unsigned char* buf = new unsigned char[size];
        memset(buf, 0, size);
        file.open(pFileName, std::ios_base::in | std::ios_base::binary);
        fRead(buf, size, 1, file);  // TODO: Handle return value
        file.close();
        if (workFile.good()) {
            workFile.write(reinterpret_cast<char*>(buf), size);
        }
        delete[] buf;
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << "file not good!" << std::endl;
        if (workFile.good()) {
            saveMainDataToFile(workFile);
        }
    }
    if (workFile.good()) {
        workFile.close();
    }
    else {
        std::cerr << "===== ERROR:" << __FUNCTION__ << " bad file!" << std::endl;
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------
#define additionLen 64

void cDeviceCalibration::saveDatFile(const std::string& fileName, unsigned int fileFormatVer)
{
    char buf[256];
    memset(buf, 0x00, sizeof(buf));
#if defined(DEFCORE_CC_BOR)

    char* pFN;
    SYSTEMTIME st;
    GetSystemTime(&st);  // gets current time
    if (snprintf(buf, 256, "_version_%d_%2d%2d%2d__%2d%2d%2d_%3d", fileFormatVer, st.wDay, st.wMonth, st.wYear - st.wYear / 1000 * 1000, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds) != 0) {
        std::string destFile = extentFileNameString(fileName, std::string(buf));
        copyFile(destFile, fileName);
    }
#elif defined(DEFCORE_CC_GNU)
    struct tm* pT;
    time_t tsec;

    time(&tsec);
    pT = gmtime(&tsec);

    if (snprintf(buf, 256, "_version_%d_%2d%2d%2d__%2d%2d%2d", static_cast<int>(fileFormatVer), pT->tm_mday, pT->tm_mon, pT->tm_year - pT->tm_year / 100 * 100, pT->tm_hour, pT->tm_min, pT->tm_sec) != 0) {
        std::string destFile = extentFileNameString(fileName, std::string(buf));
        copyFile(destFile, fileName);
    }
#endif
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool cDeviceCalibration::saveOldFormatFile(const std::string& fileName, unsigned int fileFormatVer)
{
    if (fileFormatVer < currentFileFormatVersion) {
        saveDatFile(fileName, fileFormatVer);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void cDeviceCalibration::copyFile(const std::string& pDestFileName, const std::string& pSourceFileName)
{
#ifdef DEFCORE_OS_LINUX
    sync();
#endif

    assert(!pDestFileName.empty());
    assert(!pSourceFileName.empty());

    std::ifstream source(pSourceFileName.c_str(), std::ios::binary);
    std::ofstream dest(pDestFileName.c_str(), std::ios::binary);

    std::istreambuf_iterator<char> begin_source(source);
    std::istreambuf_iterator<char> end_source;
    std::ostreambuf_iterator<char> begin_dest(dest);
    std::copy(begin_source, end_source, begin_dest);

    source.close();
    dest.close();

#ifdef DEFCORE_OS_LINUX
    sync();
#endif
}
