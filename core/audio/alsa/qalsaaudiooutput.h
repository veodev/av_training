#ifndef QAUDIOOUTPUTALSA_H
#define QAUDIOOUTPUTALSA_H

#include <alsa/asoundlib.h>

#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qiodevice.h>

#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudiodeviceinfo.h>
#include <QtMultimedia/qaudiosystem.h>

QT_BEGIN_NAMESPACE

class QAlsaAudioOutput : public QAbstractAudioOutput
{
    friend class AlsaOutputPrivate;
    Q_OBJECT
public:
    QAlsaAudioOutput(const QByteArray &device);
    ~QAlsaAudioOutput();

    qint64 write( const char *data, qint64 len );

    void start(QIODevice* device);
    QIODevice* start();
    void stop();
    void reset();
    void suspend();
    void resume();
    int bytesFree() const;
    int periodSize() const;
    void setBufferSize(int value);
    int bufferSize() const;
    void setNotifyInterval(int milliSeconds);
    int notifyInterval() const;
    qint64 processedUSecs() const;
    qint64 elapsedUSecs() const;
    QAudio::Error error() const;
    QAudio::State state() const;
    void setFormat(const QAudioFormat& fmt);
    QAudioFormat format() const;
    void setVolume(qreal);
    qreal volume() const;


    QIODevice* audioSource;
    QAudioFormat settings;
    QAudio::Error errorState;
    QAudio::State deviceState;

private slots:
    void userFeed();
    bool deviceReady();

signals:
    void processMore();

private:
    bool opened;
    bool pullMode;
    bool resuming;
    int buffer_size;
    int period_size;
    int intervalTime;
    qint64 totalTimeValue;
    unsigned int buffer_time;
    unsigned int period_time;
    snd_pcm_uframes_t buffer_frames;
    snd_pcm_uframes_t period_frames;
    int xrun_recovery(int err);

    int setFormat();
    bool open();
    void close();

    QTimer* timer;
    QByteArray m_device;
    int bytesAvailable;
    QTime timeStamp;
    QTime clockStamp;
    qint64 elapsedTimeOffset;
    char* audioBuffer;
    snd_pcm_t* handle;
    snd_pcm_access_t access;
    snd_pcm_format_t pcmformat;
    snd_pcm_hw_params_t *hwparams;
    qreal m_volume;
};

class AlsaOutputPrivate : public QIODevice
{
    friend class QAlsaAudioOutput;
    Q_OBJECT
public:
    AlsaOutputPrivate(QAlsaAudioOutput* audio);
    ~AlsaOutputPrivate();

    qint64 readData( char* data, qint64 len);
    qint64 writeData(const char* data, qint64 len);

private:
    QAlsaAudioOutput *audioDevice;
};

QT_END_NAMESPACE


#endif
