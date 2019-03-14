#ifndef PROT_UMU_CAN_CONSTANTS
#define PROT_UMU_CAN_CONSTANTS

const unsigned char UMU = 0x02; // БУИ
const unsigned char CMU = 0x04; // БУМ

//---------------------------------------------------------------------------
// Режимы работы A-развертки // Биты: 76543210
//---------------------------------------------------------------------------
const unsigned char asmAveragingOff    =  0x0;  // Бит 3, 2 – режим усреднения: 00 – выкл
const unsigned char asmAveraging2Frame =  0x4;  // Бит 3, 2 – режим усреднения: 01 – усреднение двух разверток, 10 – усреднение четырех разверток, 11 – усреднение восьми разверток
const unsigned char asmAveraging4Frame =  0x8;  // Бит 3, 2 – режим усреднения: 10 – усреднение четырех разверток, 11 – усреднение восьми разверток
const unsigned char asmAveraging8Frame =  0xC;  // Бит 3, 2 – режим усреднения: 11 – усреднение восьми разверток
const unsigned char asmAllData         =  0x0;  // Бит 4: 0 – передача по LAN всех данных А-развертки
const unsigned char asmNoBody          =  0x10; // Бит 4: 1 – передача только положения максимума сигнала (без данных развертки)
const unsigned char asmDepthGaugeOn    =  0x0;  // Бит 5: 0 – глубиномер включен
const unsigned char asmDepthGaugeOff   =  0x20; // Бит 5: 1 - выключен
const unsigned char asmOneChannel      =  0x0;  // Бит 6: 0 – развертка только по указанному каналу
const unsigned char asmCycleChannel    =  0x40; // Бит 6: 1 – развертка последовательно по всем каналам, начиная с указанного (в однократном режиме данный тип формирования развертки не имеет смысла)
const unsigned char asmCycle           =  0x0;  // Бит 7: 0 – циклический режим
const unsigned char asmOneTime         =  0x80; // Бит 7: 1 – однократный режим
//---------------------------------------------------------------------------
// Прошивка параметров тактов в БУМ
//---------------------------------------------------------------------------
const unsigned char uStrokeCount = 0x42; // + БУИ - Установить количество тактов
const unsigned char uParameter   = 0x40; // + БУИ - Записать значение параметра в таблицу тактов
const unsigned char uGainSeg     = 0x41; // + БУИ - Записать параметры отрезка кривой ВРУ
const unsigned char uGate        = 0x49; // + БУИ - Записать параметры строба
const unsigned char uPrismDelay  = 0x4A; // + БУИ - Записать задержку в призме
//---------------------------------------------------------------------------
// А-развертка
//---------------------------------------------------------------------------
const unsigned char uEnableAScan        = 0x45; // + БУИ - Разрешить передачу А-развертки
const unsigned char wmDisableAScan      = 0x46; // + БУИ - Запретить передачу А-развертки
const unsigned char dAScanDataHead      = 0x7F; // + БУМ - Заголовок А-развертки
const unsigned char dAScanMeasForCurr   = 0x82; // + БУМ - А-развертка, точное значение амплитуды и задержки в заданном такте
const unsigned char dAScanMeasForZero   = 0x84; // + БУМ - А-развертка, точное значение амплитуды и задержки для нулевого такта
const unsigned char dAScanMeasFirstPack = 0x80; // + БУМ - А-развертка, первая посылка содержащая 8 первых амплитуд
const unsigned char dAScanMeasOtherPack = 0x81; // + БУМ - А-развертка, последующие посылки по 8 амплитуд, всего посылок 232/8=29
//---------------------------------------------------------------------------
// B и M-развертки
//---------------------------------------------------------------------------
const unsigned char wmEnableBScan  = 0x47; // + БУИ - Разрешить передачу В-развертки
const unsigned char wmDisableBScan = 0x48; // + БУИ - Запретить передачу B-развертки (на БУМ 8 этот код также является подтверждением запрета)
const unsigned char dBScanData     = 0x70; // + БУМ - Данные В-развертки такта К нити Н
const unsigned char dMScanData     = 0x71; // + БУМ - Данные M-развертки такта К нити Н
//---------------------------------------------------------------------------
// Управление БУМ
//---------------------------------------------------------------------------
const unsigned char wmEnableAlarm   = 0x4B; // + БУИ - Разрешить передачу АСД
const unsigned char dAlarmData      = 0x83; // + БУМ - Данные АСД
const unsigned char dPathStep       = 0x3F; // + БУМ - Срабатывание датчика пути
const unsigned char dPathStepAddit  = 0x3D; // + БУМ - Срабатывание дополнительного датчика пути
const unsigned char uPathSimulator  = 0x3E; // + БУИ - Имитатор датчика пути
const unsigned char wmEnable        = 0x43; // + БУИ - Разрешить работу БУМ
const unsigned char wmDisable       = 0x44; // + БУИ - Запретить работу БУМ
const unsigned char wmReboot        = 0xF0; // + БУМ и БУИ - Перезагрузить
const unsigned char wmGetVoltage    = 0xC2; // + БУИ - Запрос на получение напряжения аккумулятора
const unsigned char dVoltage        = 0x11; // + БУМ - Напряжение аккумулятора НА БУМ
const unsigned char dWorkTime       = 0xC3; // + БУМ и БУИ - Запрос времени работы
const unsigned char wmLineSwitching = 0x4F; // БУИ - Переключение линий БУМ с сплошного контроля на ручной
											// совмещенный или раздельный
const unsigned char wmSetMScanFormat = 0x3B; // Установить формат сообщений М-развертки
//---------------------------------------------------------------------------
// Общие команды
//---------------------------------------------------------------------------
const unsigned char cDevice       = 0xC0; // + ОБЩ - Сообщение о наличии устройства в сети
const unsigned char wmDeviceID    = 0xDE; // + ОБЩ - Запросить информацию о версии и самоидентификации устройства
const unsigned char cFWver        = 0xDF; // + ОБЩ - Версия и дата прошивки блока
const unsigned char cSerialNumber = 0xDB; // + ОБЩ - Серийный номер устройства

#endif // PROT_UMU_CAN_CONSTANTS

