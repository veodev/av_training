#ifndef FTPUPLOADER_H
#define FTPUPLOADER_H


#include <QtCore>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <iostream>

class FTPUploadRequest : public QObject
{
    Q_OBJECT
    QFile* _file;
    QNetworkReply* _reply;
    QNetworkAccessManager* _nam;
    qint64 _bytesDone;
    qint64 _bytesTotal;
    bool _ready;

private slots:
    void uploadProgress(qint64 done, qint64 total);
    void replyFinished();
    void onError(QNetworkReply::NetworkError code);

signals:
    void progressChanged();
    void requestDone(bool status);

public:
    FTPUploadRequest();
    ~FTPUploadRequest();
    void startRequest(const QString& url, const QString& filePath, QNetworkAccessManager* nam);
    void abortUpload();
    qint64 bytesDone() const;
    qint64 bytesTotal() const;
    bool ready() const;
};

class FTPUploader : public QObject
{
    Q_OBJECT
    QNetworkAccessManager* _nam;
    QString _server;
    QString _path;
    QString _login;
    QString _password;
    std::vector<FTPUploadRequest*> _activeRequests;
    qint64 _lastBytesDone;
    qint64 _lastSpeed;
    QElapsedTimer _speedTimer;

public:
    explicit FTPUploader(QObject* parent = 0);
    ~FTPUploader();
    void setParameters(const QString& server, const QString& path, const QString& login, const QString& password);
    bool uploadFile(const QString& localFile, const QString& remoteFile);
    void abortCurrentReply();

signals:
    void progress(qint64 done, qint64 total, qint64 speed);
    void finished();
    void uploadError();
private slots:
    void onProgress();
    void onRequestDone(bool status);
    void onNamFinished();
};

#endif  // FTPUPLOADER_H
