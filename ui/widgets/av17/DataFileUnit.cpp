#include <fstream>
#include <cmath>
#include <QVector>
#include "DataFileUnit.h"
#include "settings.h"
#include <QDebug>

QByteArray& A17Data::getBaLH()
{
    return baLH;
}

QByteArray& A17Data::getBaLW()
{
    return baLW;
}

QByteArray& A17Data::getBaWH()
{
    return baWH;
}

QByteArray& A17Data::getBaBScan()
{
    return baBScan;
}

bool A17Data::getNeedInit() const
{
    return NeedInit;
}

void A17Data::setNeedInit(bool value)
{
    NeedInit = value;
}

A17Data::A17Data()  // Create;
{                   //  FMeasureCount:= 0;
    Head._scanTime = 0;
    NeedInit = true;
    baWH.clear();
    baLH.clear();
    baLW.clear();
    baBScan.clear();
    Clear();
    XDefOffset = 0;
}

A17Data::~A17Data()  // Destroy;
{
    for (unsigned int Y = 0; Y < FScan.size(); Y++) {
        FScan[Y].clear();  // Head.ScanHeight
    }
    FScan.clear();
}

typedef struct
{  // packed record
    unsigned short Coord;
    uchar Count;
} THSHead;

#ifdef AV17TEST
bool A17Data::LoadFromFile(QString FileName, bool OnlyHeader)
{
    int X, Y, Idx, A, B, C;
    TAviconID FileID;
    TAviconID DeviceID;
    //  ifstream FS;
    THSHead HSHead;
    int CoordIdx;
    TScanDataItem_1 tmp;
    TScanDataItem_2 tmp2;
    //  ifstream *FS
    TA17FileHeader FileHead;

    //  if (! FileExists(FileName) ) {
    //        return(false);
    //  };

    std::ifstream FS("D:\\Av31\\Av17\\230412_030_65535.a17", std::ifstream::binary);  // FileName.t_str());//, std::ifstream::binary);
    //  std::ifstream FS("D:\\Av31\\Av17\\120515_019_00003.a17",std::ifstream::binary);//FileName.t_str());//, std::ifstream::binary);
    // 160715_003_15002
    //  std::ifstream FS("D:\Av31\Av17\МАКЕТ_CPP1\Debug\files\230412_030_65535.a17",std::ifstream::binary);//FileName.t_str());//, std::ifstream::binary);
    //  std::ifstream FS("D:\\Av31\\Av17\\МАКЕТ_CPP1\\Debug\\files\\230412_030_65535.a17",std::ifstream::binary);
    //  std::ifstream FS(FileName.t_str());//, std::ifstream::binary);
    if (!FS.is_open()) {
        return (false);
    }

    try {
        FS.seekg(0);
        //  X=FS.tellg();
        FS.read((char*) &FileHead, sizeof(TA17FileHeader));  // TA17FileHeader)); Head
        //  X=FS.gcount();
        //  X=FS.tellg();
    } catch (...) {
        //        delete FS;
        return (false);
    };

    if (!OnlyHeader) {
        if (FileHead.HeaderVer >= 5) {
            try {
                FS.read((char*) FMeasure, sizeof(TMeasure_ar));
            } catch (...) {
                //                delete FS;
                return (false);
            };
        };

        // -------------------- Загрузка данных сканирования --------------------------------------
        for (Y = 0; Y < FScan.size(); Y++) FScan[Y].clear();
        FScan.clear();

        FS.seekg(FileHead.ScanRecOffset);
        FScan.reserve(FileHead.ScanHeight);
        for (Idx = 0; Idx < (FileHead.ScanHeight); Idx++) {  // FScan[Idx]= new vector<TScanDataItem_2>(Head.ScanWidth);
            //           FScan[Idx].reserve(Head.ScanWidth);
            //           FScan[Idx]=std::vector <TScanDataItem_2>(Head.ScanWidth);
            //           FScan.push_back( std::vector<TScanDataItem_2>(Head.ScanWidth));
            //           FScan.push_back(std::vector<TScanDataItem_2>(Head.ScanWidth));
            FScan.push_back(std::vector<TScanDataItem_2>(FileHead.ScanWidth));
            //            FScan.push_back(vector<TScanDataItem_2>(FileHead.ScanWidth));
            //           for (X = 0; X < Head.ScanWidth; X++) {
            //           FScan.back().resize(Head.ScanWidth);
            //           FScan[Idx].resize(Head.ScanWidth);
        }
        //           FScan.insert(FScan.end(), std::vector<TScanDataItem_2>(Head.ScanWidth));

        if (FileHead.DeviceVer == 1) {
            for (Y = 0; Y < FileHead.ScanHeight; Y++)
                for (X = 0; X < FileHead.ScanWidth; X++) {
                    FS.read((char*) &tmp, sizeof(TScanDataItem_1));
                    FScan[Y][X].MaxAmpl[BP11_BP23 - BP11_BP23] = tmp.MaxAmpl[BP11_BP23 - BP11_BP23];
                    FScan[Y][X].MaxAmpl[BP12_BP22 - BP11_BP23] = tmp.MaxAmpl[BP12_BP22 - BP11_BP23];
                    FScan[Y][X].MaxAmpl[BP13_BP21 - BP11_BP23] = tmp.MaxAmpl[BP13_BP21 - BP11_BP23];

                    FScan[Y][X].Att[BP11_BP23 - BP11_BP23] = tmp.Att[BP11_BP23 - BP11_BP23];
                    FScan[Y][X].Att[BP12_BP22 - BP11_BP23] = tmp.Att[BP12_BP22 - BP11_BP23];
                    FScan[Y][X].Att[BP13_BP21 - BP11_BP23] = tmp.Att[BP13_BP21 - BP11_BP23];

                    FScan[Y][X].Exist = tmp.MaxExists[BP11_BP23 - BP11_BP23] || tmp.MaxExists[BP12_BP22 - BP11_BP23] || tmp.MaxExists[BP13_BP21 - BP11_BP23];
                };
        }
        else {
            for (Y = 0; Y < (FileHead.ScanHeight); Y++)
                for (X = 0; X < (FileHead.ScanWidth); X++) {
                    //                  FS.read( (uchar*)&(FScan[Y][X].MaxAmpl[0]), sizeof(TScanDataItem_2));// TScanDataItem_2
                    FS.read((char*) &tmp2, sizeof(TScanDataItem_2));  // TScanDataItem_2
                    FScan[Y][X] = tmp2;
                }
        };

        if (FileHead.HeaderVer < 5) {
            for (Y = 0; Y < (FileHead.ScanHeight); Y++)
                for (X = 0; X < (FileHead.ScanWidth); X++) {
                    A = FScan[Y][X].MaxAmpl[BP11_BP23 - BP11_BP23] * 17;
                    B = FScan[Y][X].MaxAmpl[BP12_BP22 - BP11_BP23] * 17;
                    C = FScan[Y][X].MaxAmpl[BP13_BP21 - BP11_BP23] * 17;
                    FScan[Y][X].MaxAmpl[BP11_BP23 - BP11_BP23] = A;
                    FScan[Y][X].MaxAmpl[BP12_BP22 - BP11_BP23] = C;
                    FScan[Y][X].MaxAmpl[BP13_BP21 - BP11_BP23] = B;
                };
        };
    };
    FS.close();
    //  delete FS;
    return true;
};

bool A17Data::LoadFromFile(QString FileName)
{
    return LoadFromFile(FileName, false);
}
#endif


void A17Data::SetLayerBegin(TMyAxis Axis, int Value_mm)
{
    FMeasure[Axis].LayerBegin = Value_mm;
}

void A17Data::SetLayerEnd(TMyAxis Axis, int Value_mm)
{
    FMeasure[Axis].LayerEnd = Value_mm;
}

bool A17Data::X_Idx_AttZone(int Idx)
{
    float f;
    f = qAbs(X_Idx_to_mm(Idx));
    return ((f > Head._halfScanWidth_mm * 0.75f));
}

int A17Data::X_mmLen_to_IdxLen(float mmLen)
{
    return qRound(mmLen / (static_cast<float>(Head._XAccuracy) / 100.0f));
}

int A17Data::X_mm_to_Idx(float mm)
{
    return qRound((mm + static_cast<float>(Head._halfScanWidth_mm)) / (static_cast<float>(Head._XAccuracy) / 100.0f));
}

float A17Data::X_Idx_to_mm(int Idx)
{
    return (Idx * (static_cast<float>(Head._XAccuracy) / 100.0f) - Head._halfScanWidth_mm);
}

float A17Data::X_Sys_to_mm(int Sys)
{
    return (static_cast<float>(Sys * Head._XScanStep) / 100.0f);
}

int A17Data::X_Sys_to_Idx(int Sys)
{
    return (X_mm_to_Idx(X_Sys_to_mm(Sys)));
}

int A17Data::X_Idx_to_Sys(int Idx)
{
    return (X_mm_to_Sys(X_Idx_to_mm(Idx)));
}

int A17Data::X_mm_to_Sys(float mm)
{
    return (qRound(mm / (static_cast<float>(Head._XScanStep) / 100.0f)));
}

int A17Data::Y_mm_to_Idx(float mm)
{
    return qRound((mm - Head._YStart) / (static_cast<float>(Head._YAccuracy) / 100.0f));
}

float A17Data::Y_Idx_to_mm(int Idx)
{
    return (Head._YStart + Idx * (static_cast<float>(Head._YAccuracy) / 100.0f));
}

float A17Data::Y_Sys_to_mm(int Sys)
{
    return (static_cast<float>(Head._YStart) + static_cast<float>(Sys * Head._YScanStep) / 100.0f);
}

int A17Data::Y_Sys_to_Idx(int Sys)
{
    return (Y_mm_to_Idx(Y_Sys_to_mm(Sys)));
}

int A17Data::Y_mm_to_Sys(float mm)
{
    return (qRound((mm - Head._YStart) / (static_cast<float>(Head._YScanStep) / 100.0f)));
}

int A17Data::Y_Idx_to_Sys(int Idx)
{
    return (Y_mm_to_Sys(Y_Idx_to_mm(Idx)));
}

bool A17Data::X_Idx_Test(int Idx)
{
    return ((Idx >= 0) && (Idx < Head._scanWidth));
}

bool A17Data::Y_Idx_Test(int Idx)
{
    return ((Idx >= 0) && (Idx < Head._scanHeight));
}

void A17Data::Clear()
{
    for (auto& row : FScan) {
        for (auto& element : row) {
            element.Exist = false;
            element.MaxAmpl[BP11_BP23] = 255;
            element.MaxAmpl[BP12_BP22] = 255;
            element.MaxAmpl[BP13_BP21] = 255;
        }
    }
    FMeasure[aX].RectCount = 0;
    FMeasure[aY].RectCount = 0;
    FMeasure[aZ].RectCount = 0;
}


void A17Data::InitScanData(int HalfScanWidth_mm, int ScanHeight_mm, int YStart, int XScanStep, int YScanStep, int XAccuracy, int YAccuracy)
//должен вызываться при начале нового сеанса сканирования головки рельса
{
    Head._XScanStep = XScanStep;  // Шаг ДП по горизонтали (мм * 100)
    Head._YScanStep = YScanStep;  // Шаг ДП по вертикали (мм * 100)
    Head._XAccuracy = XAccuracy;
    Head._YAccuracy = YAccuracy;
    Head._YStart = YStart;
    Head._halfScanWidth_mm = HalfScanWidth_mm;
    Head._scanHeight_mm = ScanHeight_mm;

    Q_ASSERT(XAccuracy != 0 && YAccuracy != 0);
    Head._scanWidth = static_cast<long>(HalfScanWidth_mm) * 200 / XAccuracy;
    Head._scanHeight = static_cast<long>(ScanHeight_mm) * 100 / YAccuracy;

    for (auto& scan : FScan) {
        scan.clear();
    }
    FScan.clear();

    FScan.reserve(Head._scanHeight);
    for (int Y = 0; Y < Head._scanHeight; ++Y) {
        FScan.push_back(std::vector<TScanDataItem_2>(Head._scanWidth));
    }
    Clear();
}

//<<<<<<<<<<<<TAv17Config>>>>>>>>>>>>>>>>>>

TAv17Config::TAv17Config()
    : XPathStep(0)
    , YPathStep(0)
    , StartY(0)
    , ScanWidth(0)
    , ScanHeight(0)
    , CurrentY(0)
    , ScannerOrientation(0)
    , RailSection(0)
    , WorkRail(0)
    , threshold(0)
{
}

HeadScannerData::HeadScannerData()
{
    qDebug() << "HeadScannerData initialized!";
}

HeadScannerData& HeadScannerData::instance()
{
    static HeadScannerData _instance;
    return _instance;
}

HeadScannerData::~HeadScannerData()
{
    qDebug() << "Deleting HeadScannerData...";
}

A17Data& HeadScannerData::getData()
{
    return av17data;
}

TAv17Config& HeadScannerData::getConfig()
{
    return av17config;
}
