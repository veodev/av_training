#include "ftpuploader.h"
#include "debug.h"

FTPUploader::FTPUploader(QObject* parent)
    : QObject(parent)
    , _nam(new QNetworkAccessManager())
    , _server("")
    , _path("")
    , _login("")
    , _password("")
    , _lastBytesDone(0)
    , _lastSpeed(0)
{
    ASSERT(connect(_nam, &QNetworkAccessManager::finished, this, &FTPUploader::onNamFinished));
}

FTPUploader::~FTPUploader()
{
    delete _nam;
}

void FTPUploader::setParameters(const QString& server, const QString& path, const QString& login, const QString& password)
{
    _server = server;
    _path = path;
    _login = login;
    _password = password;
}

bool FTPUploader::uploadFile(const QString& localFile, const QString& remoteFile)
{
    if (_login.size() == 0) {
        return false;
    }
    if (_server.size() == 0) {
        return false;
    }

    qDebug() << "Starting upload! file:" << localFile << " to: " << remoteFile;
    QString urlstr = "ftp://" + _login + ":" + _password + "@" + _server + _path + "/" + remoteFile;
    _nam->clearAccessCache();
    qDebug() << "FTPUploader: uploading: " << urlstr;
    FTPUploadRequest* currentRequest = new FTPUploadRequest();
    ASSERT(connect(currentRequest, &FTPUploadRequest::progressChanged, this, &FTPUploader::onProgress));
    ASSERT(connect(currentRequest, &FTPUploadRequest::requestDone, this, &FTPUploader::onRequestDone));
    currentRequest->startRequest(urlstr, localFile, _nam);
    _activeRequests.push_back(currentRequest);
    _speedTimer.restart();
    return true;
}

void FTPUploader::abortCurrentReply()
{
    qDebug() << "FTPUploader: aborting...";
    for (FTPUploadRequest* req : _activeRequests) {
        req->abortUpload();
    }
}

void FTPUploader::onProgress()
{
    qint64 bytesDone = 0;
    qint64 bytesTotal = 0;
    for (FTPUploadRequest* req : _activeRequests) {
        bytesDone += req->bytesDone();
        bytesTotal += req->bytesTotal();
    }
    if (bytesTotal == 0) {
        bytesTotal = 1;
    }
    if (_speedTimer.elapsed() > 1000) {
        qint64 timeDelta = _speedTimer.restart();
        qint64 delta = bytesDone - _lastBytesDone;
        _lastSpeed = delta / (timeDelta / 1000);
        _lastBytesDone = bytesDone;
    }

    emit progress(bytesDone, bytesTotal, _lastSpeed);
}

void FTPUploader::onRequestDone(bool status)
{
    bool allReady = true;
    for (FTPUploadRequest* req : _activeRequests) {
        allReady = allReady && req->ready();
    }
    if (allReady) {
        for (FTPUploadRequest* req : _activeRequests) {
            disconnect(req, &FTPUploadRequest::progressChanged, this, &FTPUploader::onProgress);
            disconnect(req, &FTPUploadRequest::requestDone, this, &FTPUploader::onRequestDone);
            req->deleteLater();
        }
        _activeRequests.clear();
        emit finished();
        _speedTimer.invalidate();
        _lastBytesDone = 0;
    }

    if (!status) {
        emit uploadError();

        for (FTPUploadRequest* req : _activeRequests) {
            req->abortUpload();
        }
    }
}

void FTPUploader::onNamFinished()
{
    qDebug() << "nam finished!";
}

qint64 FTPUploadRequest::bytesDone() const
{
    return _bytesDone;
}

qint64 FTPUploadRequest::bytesTotal() const
{
    return _bytesTotal;
}

bool FTPUploadRequest::ready() const
{
    return _ready;
}

void FTPUploadRequest::uploadProgress(qint64 done, qint64 total)
{
    _bytesDone = done;
    _bytesTotal = total;
    emit progressChanged();
}

void FTPUploadRequest::replyFinished()
{
    _ready = true;
    if (_reply->error() == QNetworkReply::NoError) {
        emit requestDone(true);
    }
    else {
        emit requestDone(false);
    }
}

void FTPUploadRequest::onError(QNetworkReply::NetworkError code)
{
    qDebug() << "Upload error! " << code << " text:" << _reply->errorString();
    _reply->abort();
}


FTPUploadRequest::FTPUploadRequest()
    : _file(nullptr)
    , _reply(nullptr)
    , _nam(nullptr)
    , _bytesDone(0)
    , _bytesTotal(0)
    , _ready(false)
{
}

void FTPUploadRequest::startRequest(const QString& url, const QString& filePath, QNetworkAccessManager* nam)
{
    _ready = false;
    _nam = nam;
    _bytesDone = 0;
    QUrl uploadurl(url);
    QNetworkRequest upload(uploadurl);
    upload.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    _file = new QFile(filePath);
    _bytesTotal = _file->size();
    if (_file->open(QIODevice::ReadOnly)) {
        qDebug() << "Creating reply...";
        _reply = _nam->put(upload, _file);
        ASSERT(connect(_reply, &QNetworkReply::uploadProgress, this, &FTPUploadRequest::uploadProgress));
        ASSERT(connect(_reply, &QNetworkReply::finished, this, &FTPUploadRequest::replyFinished));

        if (_reply->error() != QNetworkReply::NoError) {
            _ready = true;
        }
    }
    else {
        qDebug() << "FTPUpload: unable to open file" << filePath;
        _reply = nullptr;
        _ready = true;
    }
}

FTPUploadRequest::~FTPUploadRequest()
{
    if (_reply != nullptr) {
        disconnect(_reply, &QNetworkReply::uploadProgress, this, &FTPUploadRequest::uploadProgress);
        disconnect(_reply, &QNetworkReply::finished, this, &FTPUploadRequest::replyFinished);
        _reply->close();
        _reply->deleteLater();
        _reply = nullptr;
    }
    _file->close();
    _file = nullptr;
    _nam = nullptr;
    qDebug() << "Request deleted!";
}

void FTPUploadRequest::abortUpload()
{
    if (_reply != nullptr) {
        _reply->abort();
    }
    else {
        _ready = true;
    }
}
