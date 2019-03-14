#ifndef DATA_CONTAINER_CONSTANTS
#define DATA_CONTAINER_CONSTANTS

typedef unsigned char tAviconFileID[8];

const tAviconFileID AviconFileID    = {0x52, 0x53, 0x50, 0x42, 0x52, 0x41, 0x46, 0x44}; // Идентификатор файла [RSPBRAFD]
const tAviconFileID Avicon31Scheme1 = {0xDE, 0xFE, 0x41, 0x33, 0x31, 0x53, 0x31, 0x01}; // Авикон-31 схема 1
const tAviconFileID AviconHandScan  = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Данные ручного канала

// ------------------------------- ENUMS ---------------------------------------

enum tMediaType //
{
    mtUnkown = 0,
    mtAudio  = 1,
    mtPhoto  = 2,
    mtVideo  = 3
};

enum eCoordSys // Система отсчета путейской координаты
{
    csMetricRF         = 0,
    csImperial         = 1,
    csMetric0_1km      = 2,
    csMetric1km        = 3,
    csMetric1kmTurkish = 4
};

enum eCorrespondenceSides // Соответствие стороны тележки нитям пути
{
    csDirect  = 0, // прямое - левая сторона = левой нити
    csReverse = 1
};

// ------------------------< Идентификаторы событий файла >---------------------------------------
const unsigned char EID_HandScan         = 0x82; // Ручник
const unsigned char EID_Ku               = 0x90; // Изменение условной чувствительности
const unsigned char EID_Att              = 0x91; // Изменение аттенюатора
const unsigned char EID_TVG              = 0x92; // Изменение ВРЧ
const unsigned char EID_StStr            = 0x93; // Изменение положения начала строба
const unsigned char EID_EndStr           = 0x94; // Изменение положения конца строба
const unsigned char EID_HeadPh           = 0x95; // Список включенных наушников
const unsigned char EID_Mode             = 0x96; // Изменение режима
const unsigned char EID_SetRailType      = 0x9B; // Настройка на тип рельса
const unsigned char EID_PrismDelay       = 0x9C; // Изменение 2Тп (word)
const unsigned char EID_Stolb            = 0xA0; // Отметка координаты
const unsigned char EID_Switch           = 0xA1; // Номер стрелочного перевода
const unsigned char EID_DefLabel         = 0xA2; // Отметка дефекта
const unsigned char EID_TextLabel        = 0xA3; // Текстовая отметка
const unsigned char EID_StBoltStyk       = 0xA4; // Нажатие кнопки БС
const unsigned char EID_EndBoltStyk      = 0xA5; // Отпускание кнопки БС
const unsigned char EID_Time             = 0xA6; // Отметка времени
const unsigned char EID_StolbChainage    = 0xA7; // Отметка координаты Chainage
const unsigned char EID_Sensor1          = 0xB0; // Сигнал датчика БС и Стрелки
const unsigned char EID_AirBrush         = 0xB1; // Краскоотметчик
const unsigned char EID_AirBrushJob      = 0xB2; // Сообщение о выполнении задания на краско-отметку
const unsigned char EID_SatelliteCoord   = 0xC0; // Географическая координата
const unsigned char EID_SCReceiverStatus = 0xC1; // Состояние приемника GPS
const unsigned char EID_Media            = 0xC2; // Медиа данные
const unsigned char EID_SysCrd_SS        = 0xF1; // Системная координата короткая
const unsigned char EID_SysCrd_SF        = 0xF4; // Полная системная координата с короткой ссылкой
const unsigned char EID_SysCrd_FS        = 0xF9; // Системная координата короткая
const unsigned char EID_SysCrd_FF        = 0xFC; // Полная системная координата с полной ссылкой
const unsigned char EID_SysCrd_NS        = 0xF2; // Системная координата новая, короткая
const unsigned char EID_SysCrd_NF        = 0xF3; // Системная координата новая, полная
const unsigned char EID_EndFile          = 0xFF; // Конец файла
//  unsigned char EID_CheckSum = 0xF0; // Контрольная сумма

// ------------------------< Идентификаторы дополнительных событий >---------------------------------------
const unsigned char EID_FwdDir = 0x01; // Телега катится вперед
const unsigned char EID_LinkPt = 0x02; // Точка для связи координат и позиции в файле
const unsigned char EID_BwdDir = 0x03; // Телега катится назад
const unsigned char EID_EndBM  = 0x04; //

// ------------------------< Заголовок файла >---------------------------------------
// Флаги определяющие используемые записи в заголовке файла
const unsigned char uiUnitsCount      = 0x00; // Количество блоков прибора
const unsigned char uiRailRoadName    = 0x01; // Название железной дороги
const unsigned char uiOrganization    = 0x02; // Название организации осуществляющей контроль
const unsigned char uiDirectionCode   = 0x03; // Код направления
const unsigned char uiPathSectionName = 0x04; // Название участка дороги
const unsigned char uiRailPathNumber  = 0x05; // Номер ж/д пути
const unsigned char uiRailSection     = 0x06; // Звено рельса
const unsigned char uiDateTime        = 0x07; // Дата время контроля
const unsigned char uiOperatorName    = 0x08; // Имя оператора
const unsigned char uiCheckSumm       = 0x09; // Контрольная сумма
const unsigned char uiStartMetric     = 0x0A; // Начальная координата - Метрическая
const unsigned char uiMoveDir         = 0x0B; // Направление движения
const unsigned char uiPathCoordSystem = 0x0C; // Система путейской координат
const unsigned char uiWorkRailTypeA   = 0x0D; // Рабочая нить (для однониточных приборов)
const unsigned char uiStartChainage   = 0x0E; // Начальная координата в XXX.YYY
const unsigned char uiWorkRailTypeB   = 0x0F; // Рабочая нить вариант №2
const unsigned char uiTrackDirection  = 0x10; // Код направления
const unsigned char uiTrackID         = 0x11; // TrackID
const unsigned char uiCorrSides       = 0x12; // Соответствие стороны тележки нитям пути

// ------------------------< Количество каналов оценки >---------------------------------------
const unsigned char MinEvalChannel = 0x00;
const unsigned char MaxEvalChannel = 0x64;

// --------------------------------------------------------------------------------------------

#endif // DATA_CONTAINER_CONSTANTS

