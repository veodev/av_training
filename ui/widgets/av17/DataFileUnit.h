#ifndef DATAFILEUNIT_H
#define DATAFILEUNIT_H

#include <string>
#include <QStringList>
#include <QString>
#include <QPoint>
#include <vector>

typedef unsigned char uchar;
typedef unsigned short ushort;

typedef uchar TAviconID[8];

typedef enum
{
    BP11_BP23 = 0,  // -45 град
    BP12_BP22 = 1,  //   0 град
    BP13_BP21 = 2,  // +45 град

    BP11_BP11,
    BP12_BP12,
    BP13_BP13,

    BP21_BP21,
    BP22_BP22,
    BP23_BP23
} TScanChannel;

const int SIZEENUMCHANNELS = 9;

typedef enum
{
    rLeft,
    rRight,
    rNotSet
} TRail;  //

#pragma pack(push, 1)
typedef struct
{  // packed
    TRail Rail;
    int Surface;
    int Angle;
    int Method;
    int Att;
    int Ku;
    int PrismDelay;
    int TVG;
    int Offset;
    int ItemCount;
} THandScanRecHead;
#pragma pack(pop)

typedef enum
{
    aX = 0,
    aY = 1,
    aZ = 2
} TMyAxis;  // Оси

#pragma pack(push, 1)
typedef struct
{                         // packed record      // необходим для хранения и воспроизведения отчета
    int LayerBegin;       // начало (T3DRecon.Layers)
    int LayerEnd;         // конец (T3DRecon.Layers)
    QPoint Rects[10][2];  // array [0..9, 0..1] of TPoint; // некоторые  замеры на картинке в милиметрах
    int RectCount;        //количество замеров
} TMeasure;

typedef TMeasure TMeasure_ar[3];
typedef wchar_t THeaderStr[65];
#pragma pack(pop)


#pragma pack(push, 1)
struct TA17DataHeader
{                              // Заголовок файла
    unsigned char _headerVer;  // Версия заголовка
    int _YStart;               // Начальное положение сканирования по вертикали (мм)
    int _XScanStep;            // Шаг ДП по горизонтали (мм * 100)
    int _YScanStep;            // Шаг ДП по вертикали (мм * 100)
    int _XAccuracy;            // Точность по горизонтали (мм * 100)
    int _YAccuracy;            // Точность по вертикали (мм * 100)
    int _halfScanWidth_mm;     // Половина ширины зоны сканирования (мм)
    int _scanHeight_mm;        // Высота зоны сканирования (мм)

    int _scanWidth;       // Размер буффера сканирования по длине
    int _scanHeight;      // Размер буффера сканирования по высоте
    unsigned _checkSumm;  // Контрольная сумма (unsigned 32-bit)

    THeaderStr _comment;
    int _railWorkSide;        // Ориентация сканера относительно рабочей грани рельса - номер сторны контролтрующей рабочую грань
    int _railSection;         // Звено рельса
    TRail _workRail;          // нить
    unsigned long _scanTime;  // DWORD//__int64 ScanTime;
    THeaderStr _controlConclusion;
};
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    bool Old;
    ushort Coord;  // Word;
    uchar Count;
    uchar Delay[8];
    uchar Ampl[8];
} THandEchoDataItem;
#pragma pack(pop)

typedef std::vector<THandEchoDataItem> THandScanData;

#pragma pack(push, 1)
typedef struct
{                                          // record
    uchar MaxAmpl[3];                      // array [BP11_BP23..BP13_BP21] of Byte;
    float MaxDelay[3];                     // array [BP11_BP23..BP13_BP21] of Single;
    bool MaxExists[3];                     // array [BP11_BP23..BP13_BP21] of Boolean;
    short Att[3];                          // array [BP11_BP23..BP13_BP21] of ShortInt
    uchar EchoCount[SIZEENUMCHANNELS];     // array [TScanChannel] of Byte;
    uchar EchoDelay[SIZEENUMCHANNELS][8];  // array [TScanChannel, 0..7] of Byte;
    uchar EchoAmpl[SIZEENUMCHANNELS][8];   // array [TScanChannel, 0..7] of Byte;
} TScanDataItem_1;

struct TScanDataItem_2
{                        // record
    uchar MaxAmpl[3];    // array [BP11_BP23..BP13_BP21] of Byte;
    signed char Att[3];  // array [BP11_BP23..BP13_BP21] of ShortInt; ShortInt=1byte
    bool Exist;
};
#pragma pack(pop)

typedef std::vector<std::vector<TScanDataItem_2>> TScanData;  // 1 - Y, Height; 2 - X, Width

class A17Data
{
private:
    bool NeedInit;

    QByteArray baLH;
    QByteArray baLW;
    QByteArray baWH;
    QByteArray baBScan;
    TMeasure_ar FMeasure;

public:
    TA17DataHeader Head;
    TScanData FScan;  // 1 - Y, Height 2 - X, Width
    int XDefOffset;   // distance from weel to center of defect

    inline TScanDataItem_2 GetScanItem(int X, int Y)
    {
        if (Y < static_cast<int>(FScan.size()) && (Y >= 0)) {
            const std::vector<TScanDataItem_2>& item = FScan[static_cast<unsigned int>(Y)];
            if ((X < static_cast<int>(item.size()) && (X >= 0))) {
                return item[static_cast<unsigned int>(X)];
            }
        }

        TScanDataItem_2 res = {{0}, {0}, false};
        return res;
    }
    A17Data();
    ~A17Data();

#ifdef AV17TEST
    bool LoadFromFile(QString FileName, bool OnlyHeader);
    bool LoadFromFile(QString FileName);
#endif

    void SetLayerBegin(TMyAxis Axis, int Value_mm);
    void SetLayerEnd(TMyAxis Axis, int Value_mm);

    int X_mm_to_Idx(float mm);
    float X_Idx_to_mm(int Idx);  // Single;
    int X_mm_to_Sys(float mm);
    float X_Sys_to_mm(int Sys);
    int X_Idx_to_Sys(int Idx);
    int X_Sys_to_Idx(int Sys);

    int X_mmLen_to_IdxLen(float mmLen);
    bool X_Idx_AttZone(int Idx);

    int Y_mm_to_Idx(float mm);
    float Y_Idx_to_mm(int Idx);
    int Y_mm_to_Sys(float mm);
    float Y_Sys_to_mm(int Sys);
    int Y_Idx_to_Sys(int Idx);
    int Y_Sys_to_Idx(int Sys);

    bool X_Idx_Test(int Idx);  // Проверка на допустимость индекса
    bool Y_Idx_Test(int Idx);  // Проверка на допустимость индекса

    void Clear();
    void InitScanData(int HalfScanWidth_mm, int ScanHeight_mm, int YStart, int XScanStep, int YScanStep, int XAccuracy, int YAccuracy);
    QByteArray& getBaLH();
    QByteArray& getBaLW();
    QByteArray& getBaWH();
    QByteArray& getBaBScan();
    bool getNeedInit() const;
    void setNeedInit(bool value);
};

const int ScanChannelToStroke[SIZEENUMCHANNELS] = {0, 2, 1, 3, 4, 5, 6, 7, 8};

class TAv17Config
{
public:
    int XPathStep;   // Шаг ДП по горизонтали (мм * 100)
    int YPathStep;   // Шаг ДП по вертикали (мм * 100)
    int StartY;      // Начальное положение сканирования по вертикали (мм)
    int ScanWidth;   // Ширина (по длине) зоны сконирования в (мм)
    int ScanHeight;  // Высота зоны сконирования в отсчетах ДП
    int CurrentY;
    int ScannerOrientation;  // Ориентация сканера относительно рабочей грани рельса
    int RailSection;         // Звено рельса
    int WorkRail;            // нить
    int threshold;

    TAv17Config();
    ~TAv17Config() {}
};

class HeadScannerData
{
    HeadScannerData();
    A17Data av17data;
    TAv17Config av17config;

public:
    static HeadScannerData& instance();
    ~HeadScannerData();
    A17Data& getData();
    TAv17Config& getConfig();
};


#endif
