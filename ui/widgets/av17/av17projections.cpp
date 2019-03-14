/*unit ReconstructionUnit2;

interface

uses
  DataFileUnit, Types, SysUtils, Dialogs, Math, Windows;

type

// Система координат и идентификаторы:
//  AlongPos - Положение по длине рельса, ноль по центру зоны сканирования
//  HeightPos - Положение по высоте рельса, ноль на поверхнисти катания
//  OffsetPos - Смещение от оси рельса, ноль по центру поверхности катания
//  HeadWidth - Ширина головки рельса
*/

// implementation

#include <cstdlib>
#include <cmath>
#include "debug.h"
#include "qdatetime.h"

#include "av17projections.h"
#include "DataFileUnit.h"

uchar TLevelRecons::GetImage(unsigned int OffsetPos_mm, unsigned int AlongPos_mm)
{
    return FImage[OffsetPos_mm][AlongPos_mm];
}

TLevelImage TLevelRecons::GetImage2() const
{
    return FImage;
}

TLevelRecons::TLevelRecons()
    : FData(NULL)
    , FScanerWidth_mm(0)
    , FImageOffsetCount(0)
    , FHeadWidth_mm(0)
    , v45(true)
    , v0(true)
    , v135(true)
    , FMaxAmpl(255)
    , ImageAlongCount(0)
{
    for (int i = 0; i < sizeof(FScanZone) / sizeof(*FScanZone); ++i) {
        FScanZone[i].x = 0;
        FScanZone[i].y = 0;
    }
}

TLevelRecons::~TLevelRecons()
{
    int y, fin;
    for (y = 0, fin = FImage.size(); y < fin; y++) FImage[y].clear();
    FImage.clear();
}

void TLevelRecons::SetData(A17Data* Data, int ScanerWidth_mm)
{
    FData = Data;
    FScanerWidth_mm = ScanerWidth_mm;  // Установка размера сканера
}

int TLevelRecons::OffsetPos_mm_to_Idx(int mm) const
{
    return (mm + FHeadWidth_mm / 2);
}

int TLevelRecons::OffsetPos_Idx_to_mm(int Idx) const
{
    return (Idx - FHeadWidth_mm / 2);
}

int TLevelRecons::AlongPos_mm_to_Idx(int mm) const
{
    return (mm + (FData->Head._halfScanWidth_mm));
}

int TLevelRecons::AlongPos_Idx_to_mm(int Idx) const
{
    return (Idx - (FData->Head._halfScanWidth_mm));
}

void TLevelRecons::Make(int HeightPos_mm, int HeadWidth_mm, int Percent, TMakeVersion Version)
{
    int idx1 = 0, idx2 = 0, OffsetPos_mm = 0;
    int v1 = 0, v2 = 0, v3 = 0;
    uchar Ampl = 0, limit1 = 0, limit2 = 0, limit3 = 0;

    int AlongPos_Idx = 0;
    int AlongPos_mm = 0;
    int HeightPos_Idx = 0;

    FHeadWidth_mm = HeadWidth_mm;
    HeightPos_Idx = (FData->Y_mm_to_Idx(HeightPos_mm));

    size_t fsize = FImage.size();
    for (size_t i = 0; i < fsize; ++i) {
        FImage[i].clear();
    }
    FImage.clear();

    size_t newsize = static_cast<size_t>(2 * (FData->Head._halfScanWidth_mm));
    FImage.reserve(75);
    for (OffsetPos_mm = 0; OffsetPos_mm < HeadWidth_mm; OffsetPos_mm++) {  // устанавливаем размер массива по ширине
        FImage.push_back(std::vector<uchar>());
        FImage.back().resize(newsize);
    }

    FImageOffsetCount = HeadWidth_mm;
    ImageAlongCount = 2 * (FData->Head._halfScanWidth_mm);

    if (Version == binary) {  // для 2ой версии высчитывание максимума
        limit1 = 0;
        limit2 = 0;
        limit3 = 0;
        for (int i = 0; i < (FData->Head._scanWidth); ++i) {
            const TScanDataItem_2& item = FData->GetScanItem(i, HeightPos_Idx);
            limit1 = std::max(limit1, item.MaxAmpl[BP11_BP23 - BP11_BP23]);  //!!!
            limit2 = std::max(limit2, item.MaxAmpl[BP12_BP22 - BP11_BP23]);
            limit3 = std::max(limit3, item.MaxAmpl[BP13_BP21 - BP11_BP23]);
        }
    }

    ////////////////////
    int hwHalf = HeadWidth_mm / 2;
    for (OffsetPos_mm = -hwHalf; OffsetPos_mm <= hwHalf; OffsetPos_mm++)  // Цикл - Смещение от оси рельса
    {
        // рассчитываем смещение на каждом y
        int mmLen = FData->X_mmLen_to_IdxLen(OffsetPos_mm);
        size_t offsetPosIdx = OffsetPos_mm_to_Idx(OffsetPos_mm);
        for (AlongPos_mm = -(FData->Head._halfScanWidth_mm); AlongPos_mm < (FData->Head._halfScanWidth_mm); AlongPos_mm++)  // Цикл - по длине рельса
        {
            AlongPos_Idx = (FData->X_mm_to_Idx(AlongPos_mm));

            idx1 = AlongPos_Idx - mmLen;  //расчет индекса для 1ого канала
            const TScanDataItem_2& item1 = FData->GetScanItem(idx1, HeightPos_Idx);
            if ((idx1 >= 0) && (idx1 < (FData->Head._scanWidth))) {
                v1 = (item1.MaxAmpl[BP11_BP23 - BP11_BP23]);
            }
            else {
                v1 = 0;
            }
            const TScanDataItem_2& item2 = FData->GetScanItem(AlongPos_Idx, HeightPos_Idx);
            //для 2ого канала индекс равен AlongPos_Idx
            v2 = (item2.MaxAmpl[BP12_BP22 - BP11_BP23]);
            idx2 = AlongPos_Idx + mmLen;  //расчет индекса для 3ого канала
            const TScanDataItem_2& item3 = FData->GetScanItem(idx2, HeightPos_Idx);
            if ((idx2 >= 0) && (idx2 < (FData->Head._scanWidth))) {
                v3 = (item3.MaxAmpl[BP13_BP21 - BP11_BP23]);
            }
            else {
                v3 = 0;
            }

            //не отрисовка рабочих каналов, если надо

            //      v1:=0; // 45
            //      v2:=0; // Прямой
            //      v3:=0; // - 45


            /// для бинарной версии преобразование амплитуд
            if (Version == binary) {
                if (v1 >= limit1 * Percent / 100)
                    v1 = FMaxAmpl;
                else
                    v1 = 0;
                if (v2 >= limit2 * Percent / 100)
                    v2 = FMaxAmpl;
                else
                    v2 = 0;
                if (v3 >= limit3 * Percent / 100)
                    v3 = FMaxAmpl;
                else
                    v3 = 0;
            };
            ////////////////

            // зона контроля
            if ((!item1.Exist) || (!item2.Exist) || (!item3.Exist) || (idx1 < 0) || (idx2 < 0) || (idx1 > (FData->Head._scanWidth)) || (idx2 > FData->Head._scanWidth)) {
                Ampl = 0;
            }
            else {
                Ampl = std::max(std::max(v1, v2), v3);
            }
            ////////////////

            FImage[offsetPosIdx][AlongPos_mm_to_Idx(AlongPos_mm)] = Ampl;  //запись значений амплитуды в итоговый массив
        }
    }

    //  FScanZone=
    GetScanZone(HeightPos_Idx, HeadWidth_mm);  // fill FScanZone
}

void TLevelRecons::SetWorkChannels(bool v1, bool v2, bool v3)
{
    v45 = v1;   // 45 градусов
    v0 = v2;    // 0  градусов
    v135 = v3;  // 135 градусов
}

void TLevelRecons::SetMaxAmplitude(int NewVal)
{
    FMaxAmpl = NewVal;
}

// TScanZone
void TLevelRecons::GetScanZone(int HeightPos_mm, int HeadWidth_mm)
{
    int i;
    int len;

    len = sizeof(FScanZone) / sizeof(*FScanZone);

    for (i = 0; i < len; ++i) {  //начальное обнуление массива
        FScanZone[i].x = 0;
        FScanZone[i].y = 0;
    };

    ////////////////// схема точек зоны
    ///   1    3   ///
    /// 0        5 ///
    ///   2    4   ///
    //////////////////

    for (i = 0; i < (FData->Head._scanWidth); ++i) {  //поиск первой точки слева
        if (FData->GetScanItem(i, HeightPos_mm).Exist == true) {
            FScanZone[0].x = qRound(FData->X_Idx_to_mm(i));  // round
            FScanZone[0].y = 0;                              //{(HeadWidth_mm div 2)+(HeadWidth_mm mod 2)};
            break;
        };
    };

    for (i = (FData->Head._scanWidth); i > 0; --i) {  //! //downto 0 do  //поиск первой точки справа
        if (FData->GetScanItem(i, HeightPos_mm).Exist == true) {
            FScanZone[5].x = qRound(FData->X_Idx_to_mm(i));
            FScanZone[5].y = 0;  // {(HeadWidth_mm / 2)+(HeadWidth_mm % 2)};
            break;
        };
    };

    if (FScanZone[0].x < FScanZone[5].x) {  //если есть зона(т.е. начало раньше чем конец)
        int deltaX_mm = FScanerWidth_mm;    //дельта на краях равна ширине сканера(между 0 и 45)

        FScanZone[1].x = FScanZone[0].x + deltaX_mm;
        FScanZone[2].x = FScanZone[0].x + deltaX_mm;

        FScanZone[3].x = FScanZone[5].x - deltaX_mm;
        FScanZone[4].x = FScanZone[5].x - deltaX_mm;

        FScanZone[1].y = -HeadWidth_mm / 2;
        FScanZone[3].y = -HeadWidth_mm / 2;

        FScanZone[2].y = +HeadWidth_mm / 2;
        FScanZone[4].y = +HeadWidth_mm / 2;
    }

    //  return(FScanZone);
}

//
//
//
//
//
//
// ------------ T3DRecons ------------------------------------------------------
//
//
//
//


T3DRecons::T3DRecons()
    :  // constructor
    FData(0)
    , FLevelRec(new TLevelRecons)
    , ZoneBegin(0)
    , ZoneEnd(0)
{
}

T3DRecons::~T3DRecons()  // destructor
{
    unsigned int i, j;

    delete FLevelRec;
    //  delete FData;
    //уничтожение массивов
    for (i = 0; i < FKolbasa.size(); ++i) FKolbasa[i].clear();
    FKolbasa.clear();

    for (i = 0; i < FKolbasaCut.size(); ++i) FKolbasaCut[i].clear();
    FKolbasaCut.clear();

    for (i = 0; i < FPixels3D.size(); ++i) {
        for (j = 0; j < FPixels3D[i].size(); ++j) FPixels3D[i][j].clear();
        FPixels3D[i].clear();
    };
    FPixels3D.clear();

    for (i = 0; i < LImage.size(); ++i) LImage[i].clear();
    LImage.clear();
}

void T3DRecons::SetData(A17Data* Data)  // Устанавливает начальные данные
{
    // *FData = *Data;
    FData = Data;
    FLevelRec->SetData(Data, 37);
}

void T3DRecons::Make(int Percent)
{
    int x, y, y_min = 0, y_max = 0, length_min = 0;
    int HeightPos_mm;
    bool FirstTimeFlag;
    TKolbasa k;
    int ik;

    FirstTimeFlag = true;
    size_t ksize = FKolbasa.size();
    for (size_t i = 0; i < ksize; ++i) {
        FKolbasa[i].clear();
    }
    FKolbasa.clear();
    FKolbasa.reserve(FData->Head._scanHeight);

    for (ik = 0, HeightPos_mm = (FData->Head._YStart); ik /*HeightPos_mm*/ < (FData->Head._scanHeight); HeightPos_mm++, ik++) {
        FKolbasa.push_back(std::vector<TKolbasa>());

        FLevelRec->Make(HeightPos_mm, RailWidth, Percent, binary);  //вызываем Make для каждого этажа

        int x0 = FLevelRec->AlongPos_mm_to_Idx((FLevelRec->FScanZone[0].x));
        int x1 = FLevelRec->AlongPos_mm_to_Idx((FLevelRec->FScanZone[1].x));
        int x2 = FLevelRec->AlongPos_mm_to_Idx((FLevelRec->FScanZone[2].x));
        int x3 = FLevelRec->AlongPos_mm_to_Idx((FLevelRec->FScanZone[3].x));
        // x4=FLevelRec->AlongPos_mm_to_Idx((FLevelRec->FScanZone[4].x));
        int x5 = FLevelRec->AlongPos_mm_to_Idx((FLevelRec->FScanZone[5].x));

        int y0 = FLevelRec->OffsetPos_mm_to_Idx((FLevelRec->FScanZone[0].y));
        // y1=FLevelRec->OffsetPos_mm_to_Idx((FLevelRec->FScanZone[1].y));
        int y2 = FLevelRec->OffsetPos_mm_to_Idx((FLevelRec->FScanZone[2].y));
        int y3 = FLevelRec->OffsetPos_mm_to_Idx((FLevelRec->FScanZone[3].y));
        int y4 = FLevelRec->OffsetPos_mm_to_Idx((FLevelRec->FScanZone[4].y));
        int y5 = FLevelRec->OffsetPos_mm_to_Idx((FLevelRec->FScanZone[5].y));

        if (x0 < x5) {
            if (FirstTimeFlag == true) {
                ZoneBegin = x0;
                ZoneEnd = x5;
            }
            else {
                if (x0 < ZoneBegin) ZoneBegin = x0;
                if (x5 > ZoneEnd) ZoneEnd = x5;
            }
            FirstTimeFlag = false;
        }

        for (x = x0; x < x5; x++) {
            if (x <= (x1 - 1)) {                                                  // 1ая зона
                y_min = /*round*/ ((y0 - (x - x0) * (y2 - y0) / (x2 - x0)) + 2);  //верхняя граница
                y_max = y2 - y_min;                                               //нижняя граница
                length_min = 0;
            }

            if ((x >= x1) && (x <= x3 - 1)) {  // 2ая зона
                y_min = y3 + 2;                //верхняя граница
                y_max = y4 - 2;                //нижняя граница
                length_min = 0;
            }

            if ((x >= x3) && (x <= x5 - 1)) {                                   // 3ая зона
                y_min = /*round*/ (y5 - (x5 - x) * (y5 - y3) / (x5 - x3) + 2);  //верхняя граница
                y_max = y4 - y_min;                                             //нижняя граница
                length_min = 0;
            }

            for (y = y_min; y <= y_max; y++)           //здесь заполняем колбасу
                if ((FLevelRec->GetImage(y, x)) == 0)  //если область дефектная..
                    if (length_min == 0) {
                        k.x_mm = (FLevelRec->AlongPos_Idx_to_mm(x));
                        k.y_mm = (FLevelRec->OffsetPos_Idx_to_mm(y));
                        length_min++;
                        k.length_mm = length_min;
                        FKolbasa[ik].push_back(k);
                    }
                    else {
                        length_min = length_min + 1;
                        FKolbasa[ik].back().length_mm = length_min;
                    }
                else if ((y > y_min) && (FLevelRec->GetImage(y - 1, x) == 0))
                    length_min = 0;
        };
    };
    Pixels();
}

void T3DRecons::Pixels(void)
{
    int i, x, y, z;
    int y_begin, y_end;
    int ik;


    //установка размеров массива FPixels3D
    FPixels3D.reserve(FData->Head._scanHeight);
    for (z = 0; z < (FData->Head._scanHeight); z++) {
        FPixels3D.push_back(std::vector<std::vector<bool>>());
        FPixels3D.back().reserve(RailWidth);  // resize
        for (y = 0; y < RailWidth; y++) {
            FPixels3D[z].push_back(std::vector<bool>());
            FPixels3D[z][y].resize(FLevelRec->AlongPos_mm_to_Idx(qRound(FData->X_Idx_to_mm(FData->Head._scanWidth))));  // resize
        }
    };
    //передача FKolbasa в FPixels3D  (true-дефект, false отсутсвие)
    //  for (z= FData->Head.YStart, std::vector::iterator ik=FKolbasa.begin(); iter != FKolbasa.end(); z++, iter++) {
    for (ik = 0, z = (FData->Head._YStart); ik < static_cast<int>(FKolbasa.size()); z++, ik++) {
        int xSize = static_cast<int>(FKolbasa[ik].size());
        for (x = 0; x < xSize; x++) {
            y_begin = (FLevelRec->OffsetPos_mm_to_Idx(FKolbasa[ik][x].y_mm));
            y_end = y_begin + FKolbasa[ik][x].length_mm;
            for (y = y_begin; y <= y_end; y++) {
                i = (FLevelRec->AlongPos_mm_to_Idx(FKolbasa[ik][x].x_mm));
                FPixels3D[ik][y][i] = true;
            };
        };
    };
    /*	for (x= 0; x < FKolbasa[z].size(); x++) {
          y_begin=FLevelRec->OffsetPos_mm_to_Idx( FKolbasa[z][x].y_mm);
          y_end=y_begin+FKolbasa[z][x].length_mm;
          for (y=y_begin; y <= y_end; y++) {
            i=FLevelRec->AlongPos_mm_to_Idx(FKolbasa[z][x].x_mm);
            FPixels3D[z][y][i]=true;
          };
        };
    */
    //  };
}

void T3DRecons::Layers(TMyAxis& axis, int LBegin, int LEnd, int HBegin, int HEnd, int XBegin, int XEnd)
{
    int z, y, x;  //[z][y][x]     -> [h][x][l]

    //фильтр неправильных значений
    if (LBegin + (FLevelRec->ImageAlongCount / 2) < 0) LBegin = 0 - (FLevelRec->ImageAlongCount / 2);
    if ((LEnd + FLevelRec->ImageAlongCount / 2) - 1 >= static_cast<int>(FPixels3D[0][0].size())) {
        LEnd = static_cast<int>(FPixels3D[0][0].size()) - (FLevelRec->ImageAlongCount / 2);
    }

    if (HBegin < 0) HBegin = 0;
    if (HEnd - 1 >= static_cast<int>(FPixels3D.size())) HEnd = static_cast<int>(FPixels3D.size());

    if (XBegin + (37) < 0) XBegin = -(37);
    if (XEnd + (37 - 1) >= static_cast<int>(FPixels3D[0].size())) XEnd = static_cast<int>(FPixels3D[0].size()) - 37;


    if (!LImage.empty()) {
        for (unsigned int i = 0; i < LImage.size(); ++i) LImage[i].clear();  // delete LImage[i];
        LImage.clear();                                                      // resize(0);
    }

    //собственно алгоритм
    if (axis == aZ) {        //если ось Az (моя ox)
        LImage.reserve(38);  //{high(FPixels3D)+1}

        for (z = 0; z <= 37; z++) {  //по этажам  //high(FPixels3D)
            LImage[z].reserve(FPixels3D[0].size());
            //	  LImage[z]= new uchar[FPixels3D[0].size()];
            for (y = 0; y < static_cast<int>(FPixels3D[0].size()); y++)  //смещаяся от оси рельса по поверхности катания
                LImage[z][y] = 255;
        };

        for (z = HBegin; z < HEnd; z++) {                                                                              //по этажам
            for (y = XBegin + 37; y < XEnd + 37; y++)                                                                  //смещаяся от оси рельса по поверхности катания
                for (x = LBegin + (FLevelRec->ImageAlongCount / 2); x < (LEnd + FLevelRec->ImageAlongCount / 2); x++)  //вдоль оси рельса
                    if (FPixels3D[z][y][x] == true) {
                        LImage[z][y] = 0;
                        break;
                    }
        }
    }

    if (axis == aY) {  //если ось Ay (моя oz)
        LImage.reserve(FPixels3D[0].size());

        for (y = 0; y < static_cast<int>(FPixels3D[0].size()); y++) {  //по этажам
            LImage[y].reserve(FPixels3D[0][0].size());
            //	  LImage[y]= new uchar[FPixels3D[0][0].size()];
            for (x = 0; x < static_cast<int>(FPixels3D[0][0].size()); x++)  //смещаяся от оси рельса по поверхности катания
                LImage[y][x] = 255;                                         //по умолчанию без дефекта
        };

        for (y = XBegin + 37; y < XEnd + 37; y++)                                                                  //по этажам
            for (x = LBegin + (FLevelRec->ImageAlongCount / 2); x < (LEnd + FLevelRec->ImageAlongCount / 2); x++)  //смещаяся от оси рельса по поверхности катания
                for (z = HBegin; z < HEnd; z++)                                                                    //вдоль оси рельса
                    if (FPixels3D[z][y][x] == true) {
                        LImage[y][x] = 0;
                        break;
                    }
    };

    if (axis == aX) {        //если ось Ay (моя oz)
                             //    SetLength(LImage,38{high(FPixels3D)+1});
        LImage.reserve(38);  // high(FPixels3D)+1

        for (z = 0; z <= 37; z++) {  //{high(FPixels3D)}                //по этажам
            LImage[z].reserve(FPixels3D[0][0].size());
            //	  LImage[z]= new uchar[FPixels3D[0][0].size()];
            for (x = 0; x < static_cast<int>(FPixels3D[0][0].size()); x++)  //смещаяся от оси рельса по поверхности катания
                LImage[z][x] = 255;                                         //по умолчанию без дефекта
        }

        for (z = HBegin; z < HEnd; z++)                                                                            //по этажам
            for (x = LBegin + (FLevelRec->ImageAlongCount / 2); x < (LEnd + FLevelRec->ImageAlongCount / 2); x++)  //смещаяся от оси рельса по поверхности катания
                for (y = XBegin + 37; y < XEnd + 37; y++)                                                          //вдоль оси рельса
                    if (FPixels3D[z][y][x] == true) {
                        LImage[z][x] = 0;
                        break;
                    }
    }
}

void T3DRecons::MakeCut(int LBegin, int LEnd, int HBegin, int HEnd, int XBegin, int XEnd)
{
    int i, j, length_;
    TKolbasa Kolbasa;

    //фильтр неправильных значений
    if (LBegin + (FLevelRec->ImageAlongCount / 2) < 0) LBegin = 0 - (FLevelRec->ImageAlongCount / 2);
    if ((LEnd + FLevelRec->ImageAlongCount / 2) > static_cast<int>(FPixels3D[0][0].size())) LEnd = static_cast<int>(FPixels3D[0][0].size()) - (-1 + (FLevelRec->ImageAlongCount / 2));

    if (HBegin < 0) HBegin = 0;
    if (HEnd > static_cast<int>(FPixels3D.size())) HEnd = static_cast<int>(FPixels3D.size());

    if (XBegin + (37) < 0) XBegin = -(37);
    if (XEnd + (37) > static_cast<int>(FPixels3D[0].size())) XEnd = static_cast<int>(FPixels3D[0].size()) - 37;

    if (!FKolbasaCut.empty()) {
        for (i = 0; i < static_cast<int>(FKolbasaCut.size()); ++i) FKolbasaCut[i].clear();
        FKolbasaCut.clear();
    }

    //создаем отрезанную колбасу
    FKolbasaCut.reserve(HEnd);
    //фильтр по высоте рельса
    for (i = HBegin; i < HEnd; ++i) {
        for (j = 0; j < static_cast<int>(FKolbasa[i].size()); ++j) {
            //фильтр по длине рельса
            if ((FKolbasa[i][j].x_mm > LBegin) && (FKolbasa[i][j].x_mm < LEnd)) {
                Kolbasa = Cutting(FKolbasa[i][j], XBegin, XEnd);

                if (Kolbasa.length_mm > 0) {
                    length_ = static_cast<int>(FKolbasaCut[i].size());
                    FKolbasaCut[i].resize(length_ + 1);
                    FKolbasaCut[i][length_] = Kolbasa;
                }
            }
        }
    }
}

TKolbasa T3DRecons::Cutting(TKolbasa& Kolbasa, int YBeg, int YEnd)
{
    int KBeg, KEnd;
    TKolbasa ResKolbasa;

    //  [ YBeg
    //  ] YEnd
    //  < KBeg
    //  > KEnd
    KBeg = Kolbasa.y_mm;
    KEnd = Kolbasa.y_mm + Kolbasa.length_mm;

    ResKolbasa.x_mm = Kolbasa.x_mm;
    ResKolbasa.y_mm = 0;
    ResKolbasa.length_mm = 0;

    //    [ ] < >
    if (KBeg > YEnd) {
        return (ResKolbasa);
    }
    //    [ < ]
    else if ((YBeg < KBeg) && (KBeg < YEnd)) {
        ResKolbasa.y_mm = KBeg;

        //    [ < ] >
        if (YEnd < KEnd) ResKolbasa.length_mm = YEnd - KBeg;
        //    [ < > ]
        else if ((YBeg < KEnd) && (KEnd < YEnd))
            ResKolbasa.length_mm = KEnd - KBeg;
    }
    //  < [ ]
    else if (KBeg < YBeg) {
        //  < [ ] >
        if (YEnd < KEnd) {
            ResKolbasa.y_mm = YBeg;
            ResKolbasa.length_mm = YEnd - YBeg;
        }
        //  < [ > ]
        else if ((YBeg < KEnd) && (KEnd < YEnd)) {
            ResKolbasa.y_mm = YBeg;
            ResKolbasa.length_mm = KEnd - YBeg;
        }
        //  < > [ ]
        else if (KEnd < YEnd) {
            return (ResKolbasa);
        }
    };

    return (ResKolbasa);
};


uchar T3DRecons::GetImage(unsigned int height, unsigned int OffsetSize)
{
    return (LImage[height][OffsetSize]);
};

int T3DRecons::OffsetPos_Idx_to_mm(int Idx)
{
    return (Idx - RailWidth / 2);
};

int T3DRecons::OffsetPos_mm_to_Idx(int mm)
{
    return (mm + RailWidth / 2);
};

int T3DRecons::AlongPos_mm_to_Idx(int mm) const
{
    return (mm + (FData->Head._halfScanWidth_mm));
};

int T3DRecons::AlongPos_Idx_to_mm(int Idx) const
{
    return (Idx - (FData->Head._halfScanWidth_mm));
};
