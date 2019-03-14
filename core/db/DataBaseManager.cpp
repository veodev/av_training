#include "DataBaseManager.h"

#include <QFileInfo>
#include <assert.h>

namespace DBM {

DataBaseManager::DataBaseManager():
    State(OFF),
    DownLoadingTimeout(0),
    pFC(NULL),
    pSavedFile(NULL),
    FTPError(tFtpClient::ErrorUnknown),
    Port(0),
    pDBQuery(NULL),
    DBOpened(false),
    TracksectionIndex(0),
    TrackNumber(0),
    StartKm(0),
    StartPk(0),
    StartMeter(0),
    Length(0),
    MoveDir(0)
{ }
//---------------------------------------------------------------------

DataBaseManager::~DataBaseManager()
{
    close();
    if (pSavedFile) delete pSavedFile;
    if (pFC) delete pFC;
}


//---------------------------------------------------------------------
// задает пути файлам базы локальному и удаленному
// если UrlName - пустая строка, то данные по ftp не пытаемся загружать
void DataBaseManager::setPath(tDBDATA *pDBData)
{
    DBData.UrlName = pDBData->UrlName;
    DBData.RemoutePath = pDBData->RemoutePath;
    DBData.LocalPath = pDBData->LocalPath;
    DBData.Name   = pDBData->Name;
    DBData.User   = pDBData->User;
    DBData.Password   = pDBData->Password;


    DBLocalPath = getFullName(&DBData.LocalPath,&DBData.Name);
}
//---------------------------------------------------------------------
void DataBaseManager::on(int downloadingTimeout)
{
QFile *savedFile;

    if (getState() == DATA_LOADING) return;
    FTPError = tFtpClient::ErrorUnknown;
    DownLoadingTimeout =  downloadingTimeout;
    setState(DATA_LOADING);
    close();

    if (DBData.UrlName.isEmpty() == false)
    {// перед тем как загружать по ftp сохраним то, что есть
        savedFile = new QFile(DBLocalPath);
        if (savedFile->open(QIODevice::ReadOnly))
        {
            savedFile->rename(DBLocalPath + QString("~"));
            pSavedFile = savedFile;
        }

         if (pFC) delete pFC;
         pFC = new tFtpClient(&DBData.UrlName,&DBData.User,&DBData.Password, &DBData.RemoutePath,&DBData.LocalPath,&DBData.Name);

         //qDebug()<<DBData.LocalPath;
         connect(&DownLoadingTimer, SIGNAL(timeout()), this, SLOT(FTPTimeout()));
         connect(pFC,SIGNAL(execResult(tFTPCLIENTCODE,QString)),this,SLOT(FTPResult(tFTPCLIENTCODE, QString)));
         DownLoadingTimer.start(DownLoadingTimeout*1000);
         pFC->get(downloadingTimeout*900);
    }
        else
        {
            attemptToOpenDB();
        }
}
//---------------------------------------------------------------------
// получить отметок для участка пути с параметрами, заданными setTracksectionSpanProperties()
int DataBaseManager::getMarksList(QVector<tMark> *pList)
{
int res = -2;
tTable sampleTable;
QVector<QString> tablesName;
QString listForWhere;
QString listForOrderBy;
QString errorStr;
int meterEndPos = StartKm * 1000 + StartPk * 100 + StartMeter;
int endKm;
int endPk;
int endMeter;

    if (isStateValid() == false)  return res;
    (*pList).clear();
    sampleTable.resize(8);
    sampleTable[0].Name = KmFieldName;
    sampleTable[1].Name = PkFieldName;
    sampleTable[2].Name = MeterFieldName;
    sampleTable[3].Name = RailFieldName;
    sampleTable[4].Name = FlawCodeFieldName;
    sampleTable[5].Name = FlawDepthFieldName;
    sampleTable[6].Name = FlawLengthFieldName;
    sampleTable[7].Name = FlawTypeFieldName;

    if (MoveDir < 0)
    {
        if (meterEndPos > Length)
        {
            meterEndPos -= Length;
        }
            else
            {
                meterEndPos = 0;
            }
    }
        else
        {
            meterEndPos  += Length;
        }

    endKm = meterEndPos / 1000;
    endPk = (meterEndPos % 1000) / 100;
    endMeter = (meterEndPos % 1000) % 100;
    tablesName.resize(1);
    tablesName[0] = MarksTableName;


     listForWhere = TracksectionsIndexFieldName + EQ + getSQLValueString(TracksectionIndex);
     listForWhere += AND + TrackNumberFieldName + EQ + getSQLValueString(TrackNumber);

    if (MoveDir < 0)
    {
       int v;
       v = StartKm;
       StartKm = endKm;
       endKm = v;
//
       v = StartPk;
       StartPk = endPk;
       endPk = v;
//
       v = StartMeter;
       StartMeter = endMeter;
       endMeter = v;
    }

    listForWhere += AND + QString("(");
    listForWhere += KmFieldName + EQ + getSQLValueString(StartKm);
    listForWhere += AND + PkFieldName + EQ + getSQLValueString(StartPk);
    listForWhere += AND + MeterFieldName + GTE + getSQLValueString(StartMeter);
    listForWhere += OR;
    listForWhere += KmFieldName + GT + getSQLValueString(StartKm);
    listForWhere += OR;
    listForWhere += KmFieldName + EQ + getSQLValueString(StartKm);
    listForWhere += AND + PkFieldName + GT + getSQLValueString(StartPk);
    listForWhere += QString(") ");
//
    listForWhere += AND + QString("(");
    listForWhere += KmFieldName + EQ + getSQLValueString(endKm);
    listForWhere += AND + PkFieldName + EQ + getSQLValueString(endPk);
    listForWhere += AND + MeterFieldName + LTE + getSQLValueString(endMeter);
    listForWhere += OR;
    listForWhere += KmFieldName + LT + getSQLValueString(endKm);
    listForWhere += OR;
    listForWhere += KmFieldName + EQ + getSQLValueString(endKm);
    listForWhere += AND + PkFieldName + LT + getSQLValueString(endPk);
    listForWhere += QString(") ");

    if (MoveDir < 0)
    {
        listForOrderBy = KmFieldName + QString(" desc, ") + PkFieldName + QString(" desc, ") + MeterFieldName + QString(" desc, ") + RailFieldName + QString(" asc");
    }
        else
        {
            listForOrderBy = KmFieldName + QString(" asc, ") + PkFieldName + QString(" asc, ") + MeterFieldName + QString(" asc, ") + RailFieldName + QString(" asc");
        }

    res = select(&sampleTable,&tablesName,&listForWhere,&listForOrderBy,0,&errorStr);
    if (res == 0)
    {
        for (int ii=0; (ii< sampleTable[0].Value.size()) && (sampleTable.size() != 0); ii++)
        {
            tMark temp;
            temp.Km = sampleTable[0].Value[ii].toInt();
            temp.Pk = sampleTable[1].Value[ii].toInt();
            temp.Meter = sampleTable[2].Value[ii].toInt();
            temp.Rail = sampleTable[3].Value[ii].toInt();
            temp.Code = sampleTable[4].Value[ii];
            temp.Depth = sampleTable[5].Value[ii].toInt();
            temp.Length = sampleTable[6].Value[ii].toInt();
            temp.Type = sampleTable[7].Value[ii].toInt();

            pList->push_back(temp);
//            (*pList).resize((*pList).size() + 1);
//            (*pList)[ii].Km = sampleTable[0].Value[ii].toInt();
//            (*pList)[ii].Pk = sampleTable[1].Value[ii].toInt();
//            (*pList)[ii].Meter = sampleTable[2].Value[ii].toInt();
//            (*pList)[ii].Rail =  sampleTable[3].Value[ii].toInt();
//            (*pList)[ii].Code = sampleTable[4].Value[ii];
//            (*pList)[ii].Depth = sampleTable[5].Value[ii].toInt();
//            (*pList)[ii].Length = sampleTable[6].Value[ii].toInt();
//            (*pList)[ii].Type = sampleTable[7].Value[ii].toInt();
        }
    }
        else
        {
            while(0) ;
            qDebug() << "getMarksList: error = " << errorStr;
        }
    sampleTable.clear();
    return res;
}
//---------------------------------------------------------------------
int DataBaseManager::getTracksectionsList(QVector<tTracksectionExtName> *pList)
{
int res = -2;
tTable dataTable;
QString listForOrderBy;
QVector<QString> tablesName;
QString errorStr;

    if (isStateValid() == false)  return res;

    pList->clear();
    dataTable.resize(2);
    dataTable[0].Name = TracksectionFieldName;
    dataTable[0].Value.clear();
    dataTable[1].Name = TracksAmmountFieldName;
    dataTable[1].Value.clear();
    tablesName.resize(1);
    tablesName[0] = TracksectionsTableName;
    listForOrderBy = TracksectionsTableName;
    res = select(&dataTable,&tablesName,NULL,NULL,0,&errorStr);

    if (res == 0)
    {
        for (int ii=0; ii<dataTable[0].Value.size(); ii++)
        {
            tTracksectionExtName temp;
            temp.Name = dataTable[0].Value[ii];
            temp.Number = dataTable[1].Value[ii].toInt();
            pList->push_back(temp);
            //pList->resize(pList->size()+1);
            //(*pList)[pList->size()-1].Name =   dataTable[0].Value[ii];
            //(*pList)[pList->size()-1].Number = dataTable[1].Value[ii].toInt();
        }
    }
        else
        {
           while(0) ;
           qDebug() << "getTracksectionList: error = " << errorStr;
        }
    dataTable[0].Value.clear();
    dataTable[1].Value.clear();
    dataTable.clear();
    return res;
}
//---------------------------------------------------------------------
void DataBaseManager::sqlErrorToStr(QString *pErrorStr,QSqlError *pSqlError)
{

    pErrorStr->clear();
    if (pSqlError->text().isEmpty() == false)
    {
        *pErrorStr += "DataBaseErrorText: ";
        *pErrorStr += pSqlError->text();
        *pErrorStr += " ";
    }
}
//---------------------------------------------------------------------
void DataBaseManager::queryErrorPrn(QString *pErrorStr)
{
QSqlError err = pDBQuery->lastError();
     qDebug() << err;
     sqlErrorToStr(pErrorStr,&err);
}
//---------------------------------------------------------------------
void DataBaseManager::setState(tMANAGERSTATE state)
{
    State = state;
}
//---------------------------------------------------------------------
// проверяем, есть ли локальный файл базы
bool DataBaseManager::isLocalData()
{
QFileInfo checkFile(DBLocalPath);
    return checkFile.isFile();
}

//---------------------------------------------------------------------
QString DataBaseManager::getSQLValueString(int value)
{
QString res;
    res.setNum(value);
    return res;
}
//---------------------------------------------------------------------
QString DataBaseManager::getSQLValueString(QString *pStrValue)
{
QString res;
    res = QString("'") + *pStrValue + QString("'");
    return res;
}
//---------------------------------------------------------------------
void DataBaseManager::createSelectQuery(QString *pQuery,QVector<QString> *tableNamesList,QString *pWhereClause, QString *pOrderByClause, QVector<QString> *pNameListForDistinct)
{

   pQuery->clear();
   if ((tableNamesList == NULL) || (tableNamesList->size() == 0)) return;

//
   *pQuery += QString("SELECT ");
   if ((pNameListForDistinct != NULL) && (pNameListForDistinct->size() > 0))
   {
       for (int ii=0; ii<pNameListForDistinct->size(); ii++)
       {
           if (ii != 0)
           {
              *pQuery += QString(",");
           }
           *pQuery += (*pNameListForDistinct)[ii];
       }
   }
       else
       {
           *pQuery += QString(" *");
       }
//
   *pQuery += QString(" FROM ");
   for (int ii=0; ii<tableNamesList->size(); ii++)
   {
       if (ii != 0)
       {
          *pQuery += QString(", ");
       }
       *pQuery += (*tableNamesList)[ii];
   }
   if ((pWhereClause != NULL) && (pWhereClause->isEmpty() == false))
   {
       *pQuery += QString(" WHERE ") + *pWhereClause;
   }
   if ((pOrderByClause != NULL) && (pOrderByClause->isEmpty() == false))
   {
       *pQuery += QString(" ORDER BY ") + *pOrderByClause;
   }
}
//---------------------------------------------------------------------
int DataBaseManager::queryExe(QString *pQuery, QString *pErrorStr)
{
    bool res = pDBQuery->exec(*pQuery);
    if (res)
    {
        return 0;
    }
    queryErrorPrn(pErrorStr);
    return -1;
}
//---------------------------------------------------------------------
// если tableIndex !=0, (tableIndex-1) - индекс элемента из *pTableNamesList, значение имени которого будем подставлять в
// имена полей в nameListForDistinct
int DataBaseManager::select(tTable *pTable, QVector<QString> *pTableNamesList,QString *pWhereClause, QString *pOrderByClause,int tableIndex,QString *pErrorStr)
{
QString query;
QVector<QString> nameListForDistinct;
int res;

    for (int ii=0; ii< (*pTable).size(); ii++)
    {
        (*pTable)[ii].Value.clear();
        nameListForDistinct.resize(nameListForDistinct.size() + 1);
        if (tableIndex != 0)
        {
            nameListForDistinct[nameListForDistinct.size()-1] = (*pTableNamesList)[tableIndex-1] + QString(".") + (*pTable)[ii].Name;
        }
            else
            {
                nameListForDistinct[nameListForDistinct.size()-1] = (*pTable)[ii].Name;
            }
    }
//
    createSelectQuery(&query,pTableNamesList,pWhereClause,pOrderByClause,&nameListForDistinct);
    res = queryExe(&query,pErrorStr);
    if (res == 0)
    {
       while (pDBQuery->next())
       {
           for (int ii=0; ii< (*pTable).size(); ii++)
           {
               (*pTable)[ii].Value.resize((*pTable)[ii].Value.size()+1);
               (*pTable)[ii].Value[(*pTable)[ii].Value.size()-1]  = pDBQuery->value((*pTable)[ii].Name).toString();
           }
       }
   }
   nameListForDistinct.clear();
   return res;
}
//---------------------------------------------------------------------
QString DataBaseManager::getFullName(const QString *pPath, const QString *pName)
{
QString resStr;

    if (pPath->isEmpty())
    {
        resStr = *pName;
    }
        else
        {
            resStr = *pPath + QString("/") + *pName;
        }
    return resStr;
}
//---------------------------------------------------------------------
bool DataBaseManager::isStateValid()
{
tMANAGERSTATE s;
    s = getState();
    if ((s == LOCAL_DATA) || (s == REFRESHED_DATA)) return true;
    qDebug() << "Data unavailable!";
    return false;
}
// задать имя перегона и номера пути для поиска в таблице отметок
// предполагаем, что перегон единственный в таблице
int DataBaseManager::setTracksectionExtName(tTracksectionExtName *pExtName)
{
tTable dataTable;
QVector<QString> tablesName;
QString listForWhere;
int res;
QString errorStr;


    if (pExtName->Number == 0)
    {
        qDebug() << "setTracksectionExtName: error - pExtName->Number == 0";
        return -2;
    }
    dataTable.resize(1);
    dataTable[0].Name = QString("id");
    tablesName.resize(1);
    tablesName[0] = TracksectionsTableName;

    listForWhere = TracksectionFieldName + EQ + getSQLValueString(&pExtName->Name);
    res = select(&dataTable,&tablesName,&listForWhere,NULL,0,&errorStr);
    if (res == 0)
    {
        TracksectionIndex = dataTable[0].Value[0].toInt();
        TrackNumber = pExtName->Number;
    }
        else
        {
            qDebug() << "setTracksectionExtName: error - res == 0" << pExtName->Name;
        }

    tablesName[0].clear();
    dataTable[0].Value.clear();
    dataTable.clear();
    return 0;
}
//---------------------------------------------------------------------
// устанавливает параметры участка пути
int DataBaseManager::setTracksectionSpanProperties(tTracksectionSpanProperties *pProp)
{
int res;
    res = setTracksectionExtName(&pProp->TracksectionExtName);
    if (res == 0)
    {
        StartKm = pProp->Km;
        StartPk = pProp->Pk;
        StartMeter = pProp->Meter;
        Length = pProp->Length;
        MoveDir = pProp->MoveDir;
    }
    return res;
}
//---------------------------------------------------------------------
tColomn *DataBaseManager::getColomnPrtByName(tTable *pList, const QString &colomnName)
{
tColomn *res = NULL;

    if (pList->size() != 0)
    {
        for(int ii=0; ii < pList->size(); ii++)
        {
            if ( (*pList)[ii].Name == colomnName)
            {
                res = &(*pList)[ii];
                break;
            }
        }
    }
    return res;
}
//---------------------------------------------------------------------
tMANAGERSTATE DataBaseManager::getState()
{
    return State;
}
//---------------------------------------------------------------------
void DataBaseManager::FTPTimeout()
{
    ManStateCS.Enter();
    if (FTPError == FTPC_ERROR_UNKNOWN)
    {
        qDebug() << "tFTPClient not responding!";
        FTPError = FTPC_ERROR_TIMEOUT;
        whenDownLoadingFailed();
        pFC->abortReplyWaiting();
        DownLoadingTimer.stop();
        disconnect(&DownLoadingTimer,0,0,0);
    }
    ManStateCS.Release();
}
//---------------------------------------------------------------------
void DataBaseManager::FTPResult(tFTPCLIENTCODE result, QString errorCodeStr)
{
    ManStateCS.Enter();
    if (FTPError == FTPC_ERROR_UNKNOWN)
    {
        switch(result)
        {
            case FTPC_ERROR_NO:
            {
                qDebug() << QString("File loaded!");
                FTPError = result;
                deleteSavedFile();
                attemptToOpenDB();
                break;
            }
//
            case FTPC_ERROR_DESTINATION_FILE_OPEN_FAILED:
            {
                qDebug() << QString("Unable to open file: ") + errorCodeStr;
                FTPError = result;
                whenDownLoadingFailed();
                break;
            }
//
            case FTPC_ERROR_FILE_DOWNLOADING_FAILED:
            {
                qDebug() << QString("Error while downloading file: ") + errorCodeStr;
                FTPError = result;
                whenDownLoadingFailed();
                break;
            }
            case FTPC_ERROR_TIMEOUT:
            {
                qDebug() << QString("FTP Server timeout!: ") + errorCodeStr;
                FTPError = result;
                whenDownLoadingFailed();
            break;
            }
            default:
            {
               while(0);
               assert(0);
//             слот получил недопустимое значение параметра
               break;
            }
        }
        DownLoadingTimer.stop();
        disconnect(&DownLoadingTimer,0,0,0);
    }
    ManStateCS.Release();
    emit ftpReady(result,errorCodeStr);
}
//---------------------------------------------------------------------
bool DataBaseManager::restoreSavedFile()
{
    if (pSavedFile == NULL) return false;
    pSavedFile->rename(DBLocalPath);
    delete pSavedFile;
    pSavedFile = NULL;
    return true;
}
//---------------------------------------------------------------------
void DataBaseManager::deleteSavedFile()
{
    if (pSavedFile == NULL) return;
    pSavedFile->remove();
    delete pSavedFile;
    pSavedFile = NULL;
}
//---------------------------------------------------------------------
void DataBaseManager::whenDownLoadingFailed()
{
    restoreSavedFile();
    attemptToOpenDB();
}
//---------------------------------------------------------------------
void DataBaseManager::attemptToOpenDB()
{
QString errorStr;

    if (open(&errorStr) == true)
    {
        if (FTPError == FTPC_ERROR_NO)
        {
            setState(REFRESHED_DATA);
        }
            else
            {
                setState(LOCAL_DATA);
            }
    }
        else
        {
            setState(NODATA);
        }
}
//---------------------------------------------------------------------
// открывает локальный файл базы
bool DataBaseManager::open(QString *pErrorStr)
{
    if (DBOpened)
    {
        qDebug() << QString("Database already opened!");
        return true;
    }

    if (isLocalData())
    {
        HDB =  QSqlDatabase::addDatabase(DriverType,ConnectionName);
        HDB.setDatabaseName(DBLocalPath);
        DBOpened = HDB.open();
        if (DBOpened == false)
        {
            QSqlError err =  HDB.lastError();
            sqlErrorToStr(pErrorStr,&err);
            HDB.removeDatabase(ConnectionName);
            qDebug() << err;
        }
            else
            {
                pDBQuery = new QSqlQuery(HDB);
            }
    }
    return DBOpened;
}
//---------------------------------------------------------------------
void DataBaseManager::close()
{
   if (DBOpened)
   {
       HDB.close();
       HDB.removeDatabase(ConnectionName);
       DBOpened = false;
       if (pDBQuery)
       {
           delete pDBQuery;
           pDBQuery = NULL;
       }
   }
}
/*
QString pathToDB = QDir::currentPath()+QString("/Accounts.sqlite");
myDB.setDatabaseName(pathToDB);


*/
}
