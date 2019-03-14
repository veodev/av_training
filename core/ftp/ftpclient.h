#ifndef _ftpclienth
#define _ftpclienth

#include <QObject>
#include <QFile>
//#include <QHash>
#include <QDebug>
#include <QTime>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>

//#include "CriticalSectionW.h"
#include "CriticalSection_Lin.h"


typedef enum _FTPCLIENTCODE {                      // значение errorCodeStr - строка символов
    FTPC_ERROR_NO = 0,                             // ""
    FTPC_ERROR_UNKNOWN = -1,                       // "результат неизвестен"  // ""
    FTPC_ERROR_DESTINATION_FILE_OPEN_FAILED = -2,  //имя файла
    FTPC_ERROR_FILE_DOWNLOADING_FAILED = -3,       //""
    FTPC_ERROR_TIMEOUT = -4,                       // ""
} tFTPCLIENTCODE;


class tFtpClient : public QObject
{
    Q_OBJECT

public:
    static const tFTPCLIENTCODE ErrorUnknown = FTPC_ERROR_UNKNOWN;
    explicit tFtpClient(const QString* _pUrlName, const QString* _pUser, const QString* _pPassword, const QString* _pSrcPath, const QString* _pDstPath, const QString* _pFileName);
    ~tFtpClient();
    // асинхронное получение файла - когда процесс завершится, будет сигнал execResult
    void get(int timeout);
    // синхронное получение файла - выход по завершению процесса, т.ч. по ошибке и тайм-ауту
    // метод нельзя вызывать в слоте
    tFTPCLIENTCODE getS(int timeout);
    void abortReplyWaiting();
    QString getPath(const QString* pPath, const QString* pName);

signals:
    void execResult(tFTPCLIENTCODE result, QString errorCodeStr);

private slots:
    void finishedA(QNetworkReply* rep);
    void finishedS(QNetworkReply* rep);

    void replyErrorA(QNetworkReply::NetworkError);
    void timeout();

private:
    const QString* pUrlName;
    const QString* pSrcPath;
    const QString* pFileName;
    QString DestFileName;

    QFile* file;
    QUrl* pUrl;
    QNetworkRequest request;
    QNetworkReply* pReply;
    QNetworkAccessManager* downloader;

    bool fDataArived;

    tFTPCLIENTCODE Error;
    QTimer Timer;
    cCriticalSection_Lin slotCS;

    void finishedProc(QNetworkReply*, bool fSyncronous);

    void whenDownloadingFailed();
};
#endif
