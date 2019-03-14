#ifndef AV17PROJECTIONS_H
#define AV17PROJECTIONS_H

#include <vector>

#include "DataFileUnit.h"

#pragma pack(push, 1)
typedef std::vector<bool> VBool;
typedef unsigned char uchar;
typedef struct
{
    int x;
    int y;
} MPoint;
typedef MPoint TScanZone[6];  // Зона контроля в милиметрах
                              //    1   3        |y=  0
                              // 0         5     |y= 75
                              //    2   4        |y=150

//  typedef std::vector<uchar*> TLevelImage;//uchar TLevelImage[];//   = array of array of Byte;  // Образ этажа [y][x]
typedef std::vector<std::vector<uchar>> TLevelImage;  // uchar TLevelImage[];//   = array of array of Byte;  // Образ этажа [y][x]
typedef enum { grayscale, binary } TMakeVersion;      // версии make'а (оттенки серого, черно-белый)
#pragma pack(pop)

class TLevelRecons
{
private:
    A17Data* FData;
    int FScanerWidth_mm;    // Расстояние в сканере между датчиками 0 град. и 45 град. [мм]
    TLevelImage FImage;     // Образ этажа [y][x]
    int FImageOffsetCount;  //
                            //	int FImageAlongCount;     //    смотри в самом верху
    int FHeadWidth_mm;      //   /
    bool v45, v0, v135;     //      : Boolean;   // включенные каналы(45,0 и 135 градусов)
    int FMaxAmpl;           // Максимальная амплитуда (15,255?)

protected:
    TLevelImage GetImage2() const;
    // TScanZone
    void GetScanZone(int HeightPos_mm, int HeadWidth_mm);  // Выдаёт границы зоны сканирования

public:
    TLevelRecons();   // Create();
    ~TLevelRecons();  // void destructor Destroy;

    void SetData(A17Data* Data, int ScanerWidth_mm);  // Устанавливает начальные данные
    void SetWorkChannels(bool v1, bool v2, bool v3);  // Устанавливает используемые каналы
    void SetMaxAmplitude(int NewVal);                 // Устанавливает максимальную амплитуду

    void Make(int HeightPos_mm, int HeadWidth_mm, int Percent, TMakeVersion Version);  // Устанавливает размеры Image, заполняет его

    //    property A17Data: A17DataFile read FData; ///////зачем?
    //    property Image[OffsetPos_mm, AlongPos_mm: Integer]: Byte read GetImage; default;
    //    property Image2 : TLevelImage read GetImage2;
    //    property ScanZone: TScanZone read FScanZone;
    //    property ImageOffsetCount: Integer read FImageOffsetCount;
    //    property ImageAlongCount: Integer read FImageAlongCount;

    uchar GetImage(unsigned int OffsetPos_mm, unsigned int AlongPos_mm);
    TScanZone FScanZone;  // Зона сканирования
    int ImageAlongCount;  //    смотри в самом верху

    int OffsetPos_mm_to_Idx(int mm) const;
    int OffsetPos_Idx_to_mm(int Idx) const;
    int AlongPos_mm_to_Idx(int mm) const;
    int AlongPos_Idx_to_mm(int Idx) const;
};

// ------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct
{
    int x_mm;       // вдоль рельса
    int y_mm;       // поперек рельса
    int length_mm;  // поперек рельса
} TKolbasa;         // отрезок на горизонтальной плоскости

typedef std::vector<std::vector<TKolbasa>> TKolbasa_Array;    // array of array of TKolbasa; // первое измерение - положение по высоте рельса(ноль на поверхности катания),второе-вдоль рельса
                                                              //  bool TPixels[]; //array of array of array of boolean; //по факту 3хмерный массив [z][y][x]->[h][x][l](true-дефект, false отсутсвие)
typedef std::vector<std::vector<std::vector<bool>>> TPixels;  // array of array of array of boolean; //по факту 3хмерный массив [z][y][x]->[h][x][l](true-дефект, false отсутсвие)
//  typedef bool*** TPixels;
#pragma pack(pop)

const int RailWidth = 75;

class T3DRecons
{
private:
    A17Data* FData;
    TLevelRecons* FLevelRec;
    TLevelImage LImage;  // 255-белое(без дефекта), 0-черное (с дефектом)
protected:
    uchar GetImage(unsigned int height, unsigned int OffsetSize);

public:
    TKolbasa_Array FKolbasa, FKolbasaCut;
    TPixels FPixels3D;
    //	int ZLen,XLen,YLen;
    int ZoneBegin, ZoneEnd;


    T3DRecons();
    ~T3DRecons();

    void SetData(A17Data* Data);                                                                   // Устанавливает начальные данные
    void Make(int Percent);                                                                        //создаёт колбасу
    void Pixels();                                                                                 //создает трехмерный растровый массив(образ) Boolean
    void Layers(TMyAxis& axis, int LBegin, int LEnd, int HBegin, int HEnd, int XBegin, int XEnd);  //создаёт слой

    void MakeCut(int LBegin, int LEnd, int HBegin, int HEnd, int XBegin, int XEnd);  //Отрезает колбасу по заданным границам
    TKolbasa Cutting(TKolbasa& Kolbasa, int YBeg, int YEnd);                         //отрезает колбасу по её длине
    int OffsetPos_Idx_to_mm(int Idx);
    int OffsetPos_mm_to_Idx(int mm);
    int AlongPos_mm_to_Idx(int mm) const;
    int AlongPos_Idx_to_mm(int Idx) const;

    //    property Image[OffsetPos_mm, AlongPos_mm: Integer]: Byte read GetImage; default;
};
//------------------------------------------------------------------------------


#endif
