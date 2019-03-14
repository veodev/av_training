#ifndef TTEXTDATASOURCE_H
#define TTEXTDATASOURCE_H

#include <QVariant>
#include <QVector>
#include <QFile>
#include <QtXml>
/*
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

const QString DriverType = "QSQLITE";
const QString ConnectionName = "MYConn";

// имена полей в таблице отметок
const QString KmFieldName = "km";
const QString PkFieldName = "pk";
const QString MeterFieldName = "metre";
const QString RailFieldName = "rail"; // нить
const QString FlawCodeFieldName = "code_group";
const QString FlawTypeFieldName = "mark_type";
const QString FlawLengthFieldName = "mark_length";
const QString FlawDepthFieldName = "mark_depth";
//
const QString TracksectionsTableName = "TrackSections"; // имя таблицы перегонов в БД
const QString TracksectionFieldName = "track_section_name";  // имя поля "Название перегона" в таблице перегонов в БД
const QString TracksAmmountFieldName = "track_number_count"; // имя поля "Количество путей" в таблице перегонов в БД
const QString MarksTableName = "Marks"; // имя таблицы отметок в БД
const QString TracksectionsIndexFieldName = "track_section_id"; // имя поля "индекс имени перегона" в таблице отметок в БД
const QString TrackNumberFieldName = "track_number"; // имя поля "Номер пути" в таблице отметок в БД
//
const QString LT = " < ";
const QString GT = " > ";
const QString EQ = " = ";
const QString LTE = " <= ";
const QString GTE = " >= ";
const QString AND = " AND ";
const QString OR = " OR ";
*/

class EK_ASUI
{
private:
    int IncidentIndex = 1;
    //    QSqlDatabase DB;

    QString FWorkDir;

    // Дороги
    std::vector<int> RailRoads_NDOR;
    QStringList RailRoads_NAME;

    // Предприятия
    std::vector<int> PRED_NDOR;  // Идентификатор дороги
    QStringList PRED_LOCATION;   // Идентификатор ПЧ
    QStringList PRED_SNAME;      // Короткое название

    // Направления
    std::vector<int> Directions_NDOR;
    QStringList Directions_SCOD;
    QStringList Directions_NAME;
    QStringList Directions_PRED;

    // Главные пути
    std::vector<int> MainPaths_NDOR;
    QStringList MainPaths_NAPR;
    QStringList MainPaths_NPUT;
    QStringList MainPaths_IDPUT;
    QStringList MainPaths_PRED;

    // Станции
    std::vector<int> Station_NDOR;
    QStringList Station_SCOD;
    QStringList Station_NAME;
    QStringList Station_PODR;
    std::vector<int> Station_TYP;

    // Станционные пути
    std::vector<int> StationPaths_NDOR;
    QStringList StationPaths_NPUT;
    std::vector<int> StationPaths_TYP;
    QStringList StationPaths_PARK;
    QStringList StationPaths_SCOD;
    QStringList StationPaths_IDPUT;
    QStringList StationPaths_PRED;

    // Станции и ПЧ
    //    std::vector <int> PRED_vs_STAN_NDOR;
    //    QStringList PRED_vs_STAN_SCOD;
    //    QStringList PRED_vs_STAN_LOCATION;

    // Классификатор дефектов
    std::vector<int> DEF_RAid;
    std::vector<qlonglong> DEF_id;
    QStringList DEF_shortName;
    QStringList DEF_fullName;
    QString NSIversion;  // Версия нормативно справочная информации
    QString carID;       // ID средства НК

    // Загруженные данные
    QStringList load_RR;
    QStringList load_PRED;

    bool LoadRailRoads();
    bool LoadPRED();
    bool LoadDirections(const QString& RRname = "", const QString& PREDname = "");
    bool LoadMainPaths(const QString& RRname = "", const QString& PREDname = "");
    bool LoadStations(const QString& RRname = "", const QString& PREDname = "");
    bool LoadStationPaths(const QString& RRname = "", const QString& PREDname = "");

public:
    EK_ASUI(const QString& WorkDir, bool LoadAllData = true);
    ~EK_ASUI();
    bool LoadData(const QString& RRname, const QString& PREDname);  // Загрузка данных для конкретного ПЧ / Дороги, исп. если конструктор вызывался с LoadAllData = false
    void SetCarID(const QString& carID_);
    //    bool UpdateDB();

    QStringList GetRRs();  // Получение списка Дорог

    QStringList GetPREDs_by_RR(const QString& RRname);  // Получение списка ПЧ на Дороге

    QStringList GetDIRNames_by_RR_PRED(const QString& RRname, const QString& PREDname);                          // Получение Имен направлений по Дороге и ПЧ
    QStringList GetDIRCodes_by_RR_PRED(const QString& RRname, const QString& PREDname);                          // Получение Кодов направлений по Дороге и ПЧ
    QString GetDIRCode_by_RRs_PRED_DIR(const QString& RRname, const QString& PREDname, const QString& DIRname);  // Получение Кода Направления по Дороге, ПЧ и Названию направления

    QStringList GetMAINPATH_IDs_by_RR_PRED_DIR(const QString& RRname, const QString& PREDname, const QString& DIRname);                       // Получение Идентификаторов главных путей по Дороге, ПЧ и Направлению
    QStringList GetMAINPATH_NPUTs_by_RR_PRED_DIR(const QString& RRname, const QString& PREDname, const QString& DIRname);                     // Получение Номеров главных путей по Дороге, ПЧ и Направлению
    QString GetMAINPATH_ID_by_RR_PRED_DIR_PATH(const QString& RRname, const QString& PREDname, const QString& DIRname, const QString& PATH);  // Получение Идентификатора главного пути по Дороге, ПЧ, Направлению и Номеру пути
                                                                                                                                              // Выше проверенно

    QStringList GetSTATIONSs_by_RR_PRED(const QString& RRname, const QString& PREDname);  // Получение Станций по Дороге и ПЧ
    QString GetSTATIONcode_by_Name(const QString& name);                                  // Получение ID станции по Названию станции

    QStringList GetSTATIONSPATH_IDs_by_RR_PRED_STATION(const QString& RRname, const QString& PREDname, const QString& STATIONname);                       // Получение Идентификаторов станционного пути по Дороге, ПЧ и направлению
    QStringList GetSTATIONSPATH_NPUTs_by_RR_PRED_STATION(const QString& RRname, const QString& PREDname, const QString& STATIONname);                     // Получение номеров путей станционного пути по Дороге, ПЧ и Названию станции
    QStringList GetSTATIONSPATH_PARKs_by_RR_PRED_STATION(const QString& RRname, const QString& PREDname, const QString& STATIONname);                     // Получение парков станционного пути по Дороге, ПЧ и Названию станции
    QString GetSTATIONSPATH_ID_by_RR_PRED_STATION_PATH(const QString& RRname, const QString& PREDname, const QString& STATIONname, const QString& PATH);  // Получение идентификатора станционного пути по Дороге, ПЧ, направлению и номеру пути
    QString GetSTATIONSPATH_PARK_by_RR_PRED_STATION_PATH(const QString& RRname, const QString& PREDname, const QString& STATIONname, const QString& PATH);  // Получение парков станционного пути по Дороге, ПЧ и Названию станции

    QStringList GetDEFECTshortNames();                          // Получение коротких названий дефектов
    QStringList GetDEFECTdataBYshortName(const QString& name);  // Получение инфомации о дефекте по короткму названию

    bool CreateIncidentXML(const QString& fileName,         // Полное имя создоваемого файла
                           const QString& fwVersion,        // Версия ПО БУИ
                           const QString& decoderName,      // Оператор/расшифровщик
                           const QString& RRName,           // Название Ж/Д
                           const QString& PRED,             // Название ПЧ
                           const QString& DIRECTION,        // Название направления
                           const QString& NPUT,             // Номер пути
                           bool isStan,              // Инциндент на стации
                           const QString& StanName,         // Название станции
                           int DefLength,            // Длина дефекта
                           int DefDepth,             // Глубина дефекта
                           int DefWidth,             // Ширина дефекта
                           int speedLimit,           // Ограничение скорости
                           int posKM,                // Километр
                           int posPK,                // Пикет
                           int posM,                 // Метр
                           const QString& DefectShortname,  // Код дефекта
                           const QString& comment,          // Коментарий
                           const QString& posHint,          // Привязка
                           int Rail,                 // Нить пути: 0-Левая; 1-Правая
                           void* Image_Ptr,          // Изображение дефекта - указатель на изображение в формате PNG
                           int Image_Size,  // Изображение дефекта - размер изображения
                           const QString& Image_Name,       // Имя картинки
                           double GPS_latitude,      // GPS коодината - Широта
                           double GPS_longitude);    // GPS коодината - Долгота

    bool CreateMainPathsIncidentXML(QString fileName,         // Полное имя создоваемого файла
                                    QString fwVersion,        // Версия ПО БУИ [+]
                                    QString decoderName,      // Оператор/расшифровщик [+]
                                    QString RRName,           // Название Ж/Д  [+]
                                    QString PRED,             // Название ПЧ  [+]
                                    QString DIRECTION,        // Название направления [+ ГП]
                                    QString NPUT,             // Номер пути [+]
                                    int DefLength,            // Длина дефекта [+]
                                    int DefDepth,             // Глубина дефекта [+]
                                    int DefWidth,             // Ширина дефекта [+]
                                    int speedLimit,           // Ограничение скорости [+]
                                    int posKM,                // Километр [+]
                                    int posPK,                // Пикет [+]
                                    int posM,                 // Метр [+]
                                    QString DefectShortname,  // Код дефекта [+][+]
                                    QString comment,          // Коментарий [+]
                                    QString posHint,          // Привязка [+]
                                    int Rail,                 // Нить пути: 0-Левая; 1-Правая [+]
                                    void* Image_Ptr,          // Изображение дефекта - указатель на изображение в формате PNG
                                    int Image_Size,  // Изображение дефекта - размер изображения
                                    QString Image_Name,       // Имя картинки
                                    double GPS_latitude,      // GPS коодината - Широта
                                    double GPS_longitude);    // GPS коодината - Долгота

    bool CreateStationIncidentXML(QString fileName,         // Полное имя создоваемого файла
                                  QString fwVersion,        // Версия ПО БУИ [+]
                                  QString decoderName,      // Оператор/расшифровщик [+]
                                  QString RRName,           // Название Ж/Д  [+]
                                  QString PRED,             // Название ПЧ  [+]
                                  QString StanName,         // Название станции [+ СП]
                                  QString NPUT,             // Номер пути [+]
                                  int DefLength,            // Длина дефекта [+]
                                  int DefDepth,             // Глубина дефекта [+]
                                  int DefWidth,             // Ширина дефекта [+]
                                  int speedLimit,           // Ограничение скорости [+]
                                  int posKM,                // Километр [+]
                                  int posPK,                // Пикет [+]
                                  int posM,                 // Метр [+]
                                  QString DefectShortname,  // Код дефекта [+][+]
                                  QString comment,          // Коментарий [+]
                                  QString posHint,          // Привязка [+]
                                  int Rail,                 // Нить пути: 0-Левая; 1-Правая [+]
                                  void* Image_Ptr,          // Изображение дефекта - указатель на изображение в формате PNG
                                  int Image_Size,  // Изображение дефекта - размер изображения
                                  QString Image_Name,       // Имя картинки
                                  double GPS_latitude,      // GPS коодината - Широта
                                  double GPS_longitude);    // GPS коодината - Долгота
    int getIncidentIndex();
    void resetIncidentIndex();
};

#endif  // TTEXTDATASOURCE_H


/*





// Дороги
// RailRoads
int id; // Идентификатор дороги INTEGER
QString NAME; // Название дороги STRING

// Предприятия
// PREDs
int RRid; // Идентификатор дороги
QString id; // Идентификатор ПЧ
QString SNAME; // Короткое название

// Направления
// DIRs
int RRid; // Идентификатор дороги
QString PREDid; // Идентификатор ПЧ
QStringList id; // Идентификатор направления
QStringList NAME; // Название направления

// Главные пути
// MPATHs
int RRid; // Идентификатор дороги
QString PREDid; // Идентификатор ПЧ
QString DIRid; // Идентификатор направления
QString NAME; // Название пути
QString id; // Идентификатор

// Станции
// STATIONs
int RRid; // Идентификатор дороги
QString PREDid; // Идентификатор ПЧ
QString id; // Идентификатор станции
QString NAME; // Название станции
int TYPE; // Тип станции

// Станционные пути
// SPATHs
int RRid; // Идентификатор дороги
QString PREDid; // Идентификатор ПЧ
QString STATIONid; // Идентификатор станции
QString NAME; // Название пути
QString Id; // Идентификатор пути
QString PARKNAME; // Название парка
QString PARKid; // Идентификатор парка


// Классификатор дефектов
// DEF
int RAid;
qlonglong id;
QStringList shortName;
QStringList fullName;


// Общая информация
//
QString NSIversion; // Версия нормативно справочная информации
QString carID; // ID средства НК

*/
