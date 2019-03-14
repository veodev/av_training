#ifndef _DataBaseManager
#define _DataBaseManager

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QDebug>
#include <QTimer>

//#include "CriticalSectionW.h"
//#include "CriticalSection_Lin.h"
#include "CriticalSection.h"

#include "ftp/ftpclient.h"
// SQLite
namespace DBM {
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
const int DownLoadingTimeoutByDefault = 15;
//

typedef enum _MANAGERSTATE
{
    OFF = 0,
    DATA_LOADING = 1, // загрузка или открытие базы
    NODATA = 2, // нет файла, или база не открылась
    LOCAL_DATA = 3,
    REFRESHED_DATA = 4
} tMANAGERSTATE;

typedef struct _TracksectionExtName
{
    QString Name;     // имя перегона
    int /*int32_t*/ Number;   // номер пути или количество путей
} tTracksectionExtName;

typedef struct _TracksectionSpanProperties
{
  tTracksectionExtName TracksectionExtName;
  int Km;
  int Pk;
  int Meter;
  int Length;
  int MoveDir;
} tTracksectionSpanProperties;


typedef struct _Mark
{
    // Путейская координата: Км, Пк, метр
    int Km;
    int Pk;
    int Meter;
    int Rail;     // Нить, целое (0 – левая; 1 – Правая)
    QString Code; // Код дефекта
    int Type;     // Тип отметки, целое (0 – не заданно; 1 - дефект, 2- ОДР, 3 - подозрение на дефект)
    int Length;   // Длина [мм]
    int Depth;     // Глубина [мм]
    _Mark() : Km(-1),
        Pk(-1),
        Meter(-1),
        Rail(-1),
        Code(""),
        Type(-1),
        Length(-1),
        Depth(-1)
    { }
} tMark;


typedef struct _DBDATA
{
    QString UrlName;   // адрес сервера, где находится база, если пустая строка, то скачивание базы
// не производится, ищем локальный файл
    QString RemoutePath;  // путь к файлу базы на сервере
    QString User;
    QString Password;
    QString LocalPath;  // путь к локальному файлу базы
    QString Name;       // имя файла БД
//
} tDBDATA;


typedef struct _Colomn
{
    QString Name; // заголовок столбца
    QVector<QString>Value;
} tColomn;
typedef QVector<tColomn> tTable;


class DataBaseManager: public QObject
{
    Q_OBJECT
public:
    explicit DataBaseManager();
    ~DataBaseManager(void);

    tMANAGERSTATE getState();
// downloadingTimeout время в сек. ожидания загрузки файла БД
// значение downloadingTimeout будет применено, если НЕ находимся в состоянии DATA_LOADING
    void on(int downloadingTimeout);
    void on(){on(DBM::DownLoadingTimeoutByDefault);};
//
// задает пути файлам базы локальному и удаленному
// если UrlName - пустая строка, то данные по ftp не пытаемся загружать
    void setPath(tDBDATA *pDBData);
//
// получить отметок для участка пути с параметрами, заданными setTracksectionSpanProperties()
    int getMarksList(QVector<tMark> *pList);
// получить список перегонов
    int getTracksectionsList(QVector<tTracksectionExtName> *pList); // получить список перегонов
//
// устанавливает параметры участка перегона
    int setTracksectionSpanProperties(tTracksectionSpanProperties *pProp);

private:

    tMANAGERSTATE State;
    int DownLoadingTimeout;

    tFtpClient *pFC;
    QTimer DownLoadingTimer;
    cCriticalSection_Lin ManStateCS;
    QFile *pSavedFile;
    tFTPCLIENTCODE FTPError;

    tDBDATA DBData;
    QString DBLocalPath;
    QSqlDatabase HDB;
    unsigned short Port;
    QSqlQuery *pDBQuery;
    bool DBOpened;
//
//
    int TracksectionIndex;
    int TrackNumber;
    int StartKm;
    int StartPk;
    int StartMeter;
    int Length;
    int MoveDir;

    void close();
    void createSelectQuery(QString *pQuery, QVector<QString> *tableNamesList, QString *pWhereClause, QString *pOrderByClause, QVector<QString> *pNameListForDistinct);
// проверяем, есть ли локальный файл базы
    bool isLocalData();
//
    tColomn *getColomnPrtByName(tTable *pList,const QString & colomnName);
    bool isStateValid();
    QString getFullName(const QString *pPath, const QString *pName);
    QString getSQLValueString(int value);
    QString getSQLValueString(QString *pStrvalue);
    bool open(QString *pErrorStr);
    int queryExe(QString *pQuery,QString *pErrorStr);
    void queryErrorPrn(QString *pErrorStr);
    void sqlErrorToStr(QString *pErrorStr,QSqlError *pSqlError);
    int select(tTable *pTable, QVector<QString> *pTableNamesList,QString *pWhereClause, QString *pOrderByClause, int tableIndex,QString *pErrorStr);
    int setTracksectionExtName(tTracksectionExtName *pExtName); // задать имя перегона и номер пути для поиска в таблице отметок
    void setState(tMANAGERSTATE state);

    void deleteSavedFile();
    bool restoreSavedFile();
    void attemptToOpenDB();
    void whenDownLoadingFailed();
private slots:
    void FTPResult(tFTPCLIENTCODE result0, QString errorCodeStr);
    void FTPTimeout();
signals:
    void ftpReady(tFTPCLIENTCODE result0, QString errorCodeStr);
};
}
#endif
