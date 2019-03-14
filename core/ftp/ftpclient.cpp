#include <QThread>
#include "ftpclient.h"


tFtpClient::tFtpClient(const QString* _pUrlName, const QString* _pUser, const QString* _pPassword, const QString* _pSrcPath, const QString* _pDstPath, const QString* _pFileName)
{
    Error = ErrorUnknown;
    pUrlName = _pUrlName;
    pSrcPath = _pSrcPath;
    pFileName = _pFileName;

    DestFileName = getPath(_pDstPath, _pFileName);
    //
    pUrl = new QUrl(*pUrlName);
    pUrl->setScheme("ftp");
    pUrl->setHost(*pUrlName);
    pUrl->setPath(*pSrcPath + QString("/") + *pFileName);
    pUrl->setPort(21);
    if ((_pUser != NULL) && (_pPassword != NULL) && (_pUser->isEmpty() == false) && (_pPassword->isEmpty() == false)) {
        pUrl->setUserName(*_pUser);
        pUrl->setPassword(*_pPassword);
    }
    else {
        pUrl->setUserName("anonymous");
    }
    request.setUrl(*pUrl);
    downloader = new QNetworkAccessManager((QObject*) this);
    fDataArived = false;
    qDebug() << "Download url: " << pUrl->toString();
}

tFtpClient::~tFtpClient()
{
    delete pUrl;
    delete downloader;
}
//
// асинхронное получение файла - когда процесс завершится, будет сигнал execResult
void tFtpClient::get(int timeout)
{
    // сигнал finished() возникнет, когда файл будет ПОЛНОСТЬЮ докачан
    // сигнал pReply->readyRead() возникает, когда файл только начинает скачиваться. Попытки
    // чего-то читать вызывают обрыв связи с сервером

    file = new QFile(DestFileName);
    if (file->open(QIODevice::WriteOnly)) {
        Error = ErrorUnknown;
        connect(downloader, &QNetworkAccessManager::finished, this, &tFtpClient::finishedA);
        connect(&Timer, &QTimer::timeout, this, &tFtpClient::timeout);
        Timer.start(timeout);
        pReply = downloader->get(request);
        connect(pReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &tFtpClient::replyErrorA);
    }
    else {
        Error = FTPC_ERROR_DESTINATION_FILE_OPEN_FAILED;
        emit execResult(Error, DestFileName);
    }
}
//
// синхронное получение файла - выход по завершению процесса, т.ч. по ошибке и тайм-ауту
// метод нельзя вызывать в слоте
tFTPCLIENTCODE tFtpClient::getS(int timeout)
{
    QTime tmr;

    // сигнал finished() возникнет, когда файл будет ПОЛНОСТЬЮ докачан
    // сигнал pReply->readyRead() возникает, когда файл только начинает скачиваться. Попытки
    // чего-то читать вызывают обрыв связи с сервером

    file = new QFile(DestFileName);
    if (file->open(QIODevice::WriteOnly)) {
        fDataArived = false;
        connect(downloader, &QNetworkAccessManager::finished, this, &tFtpClient::finishedS);
        pReply = downloader->get(request);
        tmr.start();
        while ((tmr.elapsed() < timeout) && (fDataArived == false)) {
            QThread::msleep(100);
        }
        if (fDataArived != true) {
            Error = FTPC_ERROR_TIMEOUT;
            file->remove();
        }
    }
    else {
        Error = FTPC_ERROR_DESTINATION_FILE_OPEN_FAILED;
    }
    delete file;
    return Error;
}
//
void tFtpClient::whenDownloadingFailed()
{
    abortReplyWaiting();
    file->close();
    file->remove();
}
//
void tFtpClient::replyErrorA(QNetworkReply::NetworkError netWorkError)
{
    QString errorCodeStr;
    QString codeNumStr;

    slotCS.Enter();
    if (Error == FTPC_ERROR_UNKNOWN) {
        Timer.stop();
        disconnect(&Timer, 0, 0, 0);
        Error = FTPC_ERROR_FILE_DOWNLOADING_FAILED;
        codeNumStr.setNum(netWorkError);
        errorCodeStr = QString("signal requestFailed - error = ") + codeNumStr;
        qDebug() << errorCodeStr;
        whenDownloadingFailed();
        emit execResult(Error, errorCodeStr);
    }
    slotCS.Release();
}

//
// если соединение с FTP-сервером не было установлено, то сигнала для этих
// слотов не будет
void tFtpClient::finishedA(QNetworkReply* rep)
{
    finishedProc(rep, false);
}
//
void tFtpClient::finishedS(QNetworkReply* rep)
{
    finishedProc(rep, true);
}
//
void tFtpClient::finishedProc(QNetworkReply* rep, bool fSyncronous)
{
    QByteArray buf;
    qint64 qBytes;

    slotCS.Enter();
    if (Error == FTPC_ERROR_UNKNOWN) {
        qDebug() << "devloader-finished: Reply finished. error = " << rep->errorString();
        // не обращаем внимания и вызываем слот, который не подоткнули к сигналу
        qBytes = pReply->bytesAvailable();
        qDebug() << "Recieved: " << qBytes << " bytes";
        if (qBytes != 0) {
            buf = pReply->readAll();
            file->write(buf);
            file->close();
            Error = FTPC_ERROR_NO;
            if (fSyncronous == false) {
                Timer.stop();
                disconnect(&Timer, 0, 0, 0);
                emit execResult(Error, QString(""));
            }
            else {
                delete file;
                fDataArived = true;
            }
        }
        else {
            Error = FTPC_ERROR_FILE_DOWNLOADING_FAILED;
            file->close();
            file->remove();
            if (fSyncronous == false) {
                Timer.stop();
                disconnect(&Timer, 0, 0, 0);
                emit execResult(Error, QString(""));
            }
            else {
                delete file;
                fDataArived = true;
            }
        }
        abortReplyWaiting();
    }
    slotCS.Release();
}
//
//
void tFtpClient::timeout()
{
    slotCS.Enter();
    if (Error == FTPC_ERROR_UNKNOWN) {
        Error = FTPC_ERROR_TIMEOUT;
        Timer.stop();
        disconnect(&Timer, 0, 0, 0);
        whenDownloadingFailed();
        emit execResult(Error, QString(""));
    }
    slotCS.Release();
}
//
QString tFtpClient::getPath(const QString* pPath, const QString* pName)
{
    QString resStr;

    if (pPath->isEmpty()) {
        resStr = *pName;
    }
    else {
        resStr = *pPath + QString("/") + *pName;
    }
    return resStr;
}
//
void tFtpClient::abortReplyWaiting()
{
    pReply->deleteLater();
    disconnect(downloader, 0, 0, 0);
}
