#ifndef PROT_UMU_LAN_CONSTANTS
#define PROT_UMU_LAN_CONSTANTS

//---------------------------------------------------------------------------
// --- БУМ и БУИ ----
//---------------------------------------------------------------------------
const unsigned char CDU = 0x02;  //
const unsigned char UMU = 0x04;  //
//---------------------------------------------------------------------------
// --- Режимы работы A-развертки ---
//--------------------------------------------------------------------------- // Биты: 76543210
const unsigned char asmAveragingOff = 0x0;     // Бит 3, 2 – режим усреднения: 00 – выкл
const unsigned char asmAveraging2Frame = 0x4;  // Бит 3, 2 – режим усреднения: 01 – усреднение двух разверток, 10 – усреднение четырех разверток, 11 – усреднение восьми разверток
const unsigned char asmAveraging4Frame = 0x8;  // Бит 3, 2 – режим усреднения: 10 – усреднение четырех разверток, 11 – усреднение восьми разверток
const unsigned char asmAveraging8Frame = 0xC;  // Бит 3, 2 – режим усреднения: 11 – усреднение восьми разверток
const unsigned char asmAllData = 0x0;          // Бит 4: 0 – передача по LAN всех данных А-развертки
const unsigned char asmNoBody = 0x10;          // Бит 4: 1 – передача только положения максимума сигнала (без данных развертки)
const unsigned char asmDepthGaugeOn = 0x0;     // Бит 5: 0 – глубиномер включен
const unsigned char asmDepthGaugeOff = 0x20;   // Бит 5: 1 - выключен
const unsigned char asmOneChannel = 0x0;       // Бит 6: 0 – развертка только по указанному каналу
const unsigned char asmCycleChannel = 0x40;  // Бит 6: 1 – развертка последовательно по всем каналам, начиная с указанного (в однократном режиме данный тип формирования развертки не имеет смысла)
const unsigned char asmCycle = 0x0;     // Бит 7: 0 – циклический режим
const unsigned char asmOneTime = 0x80;  // Бит 7: 1 – однократный режим
//---------------------------------------------------------------------------
// --- up - сообщения верхнего блока - БУИ ---
//---------------------------------------------------------------------------
const unsigned char uStrokeCount = 0x42;  // БУИ - Установить количество тактов
const unsigned char uParameter = 0x40;    // БУИ - Записать значение параметра в таблицу тактов
// const unsigned char uParameter_ = 0x4D;     		// БУИ - Установка параметров получения точных значений амплитуды и задержки В-развертки
const unsigned char uGainSeg = 0x41;           // БУИ - Записать параметры отрезка кривой ВРУ
const unsigned char uGate = 0x49;              // БУИ - Записать параметры строба
const unsigned char uPrismDelay = 0x4A;        // БУИ - Записать задержку в призме
const unsigned char uEnableAScan = 0x45;       // БУИ - Разрешить передачу А-развертки
const unsigned char uPathSimulator = 0x3E;     // БУИ - Включение имитатора датчика пути
const unsigned char uPathSimulatorEx = 0x3F;   // БУИ - Включение имитатора датчика пути (расширенный вариант)
const unsigned char uPathSimulatorEx2 = 0x3C;  // БУИ - Включение имитатора датчика пути сканера
const unsigned char uEnable = 0x43;            // БУИ - Управление работой БУМ
const unsigned char uCalcACThreshold = 0x85;   // БУИ - Расчитать пороги акустического контакта
const unsigned char uBSFilterStDelay = 0x73;   // БУИ - Установка момента начала фильтрации сигналов B-развертки.
const unsigned char uSetACTresholds = 0x87;    // БУИ - Установка порогов проверки АК
const unsigned char dPathEncoderSetup = 0x1D;  // БУИ - Установка значения датчика пути и выбор основного ДП (инициирующего отсылку В-развертки)

//---------------------------------------------------------------------------
// --- down - сообщения нижнего блока - БУМ ---
//---------------------------------------------------------------------------
const unsigned char dAScanDataHead = 0x7F;            // БУМ - Данные А-развертки
const unsigned char dAScanMeas = 0x82;                // БУМ - А-развертка, точное значение амплитуды и задержки
const unsigned char dBScanData = 0x70;                // БУМ - Данные В-развертки
const unsigned char dMScanData = 0x71;                // БУМ - Данные М-развертки канала К нити Н
const unsigned char dBScan2Data = 0x72;               // БУМ - Данные В-развертки вариант 2
const unsigned char dAlarmData = 0x83;                // БУМ - Данные АСД
const unsigned char dPathStep = 0x3D /*F*/;           // БУМ - Срабатывание датчика пути
const unsigned char dPathStepEx = 0x3E;               // БУМ - Срабатывание датчика пути с дисплейной координатой
const unsigned char dA15ScanerPathStep = 0x39;        // БУМ - Координата сканера Ав-15
const unsigned char dConfirmPathEncoderSetup = 0x1D;  // БУМ - Подтверждение установки значения датчика пути
const unsigned char dMetalSensor = 0x60;              // БУМ - Срабатывание датчика маталла (болтового стыка и стрелочного перевода)
const unsigned char dVoltage = 0x11;                  // БУМ - Напряжение аккумулятора НА БУМ
const unsigned char dWorkTime = 0xC3;                 // БУМ - Время работы БУМ
const unsigned char dDeviceSpeed = 0x1C;              // БУМ - Скорость системы
// const unsigned char dEnvelopeMeas = 0x85;     	// БУМ - Значение огибающей сигнала
const unsigned char dTestAScan = 0x46;            // БУМ - подсчет А-разверток
const unsigned char dConfirmStopBScan = 0x48;     // БУМ - Подтверждение выполнения команды «Запретить передачу В-развертки с идентификатором 0x72»
const unsigned char dAcousticContactSumm = 0x80;  // БУМ - Сырые данные акустического контакта
const unsigned char dAcousticContact_ = 0x86;     // БУМ - Данные акустического контакта
const unsigned char dACTresholds = 0x88;          // БУМ - Данные установленных порогов проверки АК

//---------------------------------------------------------------------------
// --- common - общие сообщения ---
//---------------------------------------------------------------------------
const unsigned char cDevice_ = 0xC0;       // ОБЩ - Сообщение о наличии устройства в сети
const unsigned char cFWver = 0xDF;         // ОБЩ - Версия и дата прошивки блока
const unsigned char cSerialNumber = 0xDB;  // ОБЩ - Серийный номер устройства
//---------------------------------------------------------------------------
// --- work manage - управление работой ---
//---------------------------------------------------------------------------
const unsigned char wmDisableAScan = 0x46;   // БУИ - Запретить передачу А-развертки
const unsigned char wmEnableBScan = 0x47;    // БУИ - Разрешить передачу В-развертки
const unsigned char wmDisableBScan = 0x48;   // БУИ - Запретить передачу B-развертки*
const unsigned char wmEnableACSumm = 0x37;   // БУИ - Разрешить передачу сырых данных акустического контакта**
const unsigned char wmDisableACSumm = 0x38;  // БУИ - Запретить передачу сырых данных акустического контакта
const unsigned char wmEnableAC = 0x39;       // БУИ - Разрешить передачу данных акустического контакта**
// const unsigned char wmDisableAC   = 0x38;  	// БУИ - Запретить передачу данных акустического контакта
const unsigned char wmEnableAlarm = 0x4B;  // БУИ - Разрешить передачу АСД**

// * при этом будет выставлен запрет на передачу данных АСД и АК.
// ** предварительно необходимо разрешить передачу B-развертки.

const unsigned char wmDisableAlarm = 0x4C;  // БУИ - Запретить передачу АСД
const unsigned char wmEnable = 0x43;        // БУИ - Разрешить работу БУМ
const unsigned char wmDisable = 0x44;       // БУИ - Запретить работу БУМ
const unsigned char wmReboot = 0xF0;        // БУИ - Перезагрузить БУМ
const unsigned char wmGetWorkTime = 0xC3;   // БУИ - Запрос на получение времени работы
const unsigned char wmDeviceID = 0xDE;      // ОБЩ - Запросить информацию о версии и самоидентификации устройства
const unsigned char wmGetVoltage = 0xC2;    // БУИ - Запрос на получение напряжения аккумулятора
// const unsigned char uMotorManagement = 0x1A;  // БУИ - Управление шаговыми двигателями
const unsigned char wmLeftSideSwitching = 0x4F;  // БУИ - Переключение линий БУМ с сплошного контроля на сканер
const unsigned char wmGetACTresholds = 0x88;     // БУИ - Запрос на получение текущих установленных порогов проверки АК
const unsigned char wmEnableCalcDisCoord = 0x35; // БУИ - включить передачу данных о срабатывании основного датчика пути в формате п. 4.31***** вместо формата п. 4.13.
const unsigned char wmDisableCalcDisCoord = 0x36; // БУИ - отключить передачу данных о срабатывании основного датчика пути в формате п. 4.31

//---------------------------------------------------------------------------
// --- UMU_3204 ---
//---------------------------------------------------------------------------

// amplitude index
static const int StartAmpl = 0;
static const int MaxAmpl = 1;
static const int StartDelay_int = 2;
static const int MaxDelay_int = 3;
static const int EndDelay_int = 4;
static const int StartDelay_frac = 5;
static const int MaxDelay_frac = 6;
static const int EndDelay_frac = 7;
static const int AmplDBIdx_int = 8;

#endif  // PROT_UMU_LAN_CONSTANTS
