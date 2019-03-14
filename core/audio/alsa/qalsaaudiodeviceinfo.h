#ifndef QALSAAUDIODEVICEINFO_H
#define QALSAAUDIODEVICEINFO_H

#include <alsa/asoundlib.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>

#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudiodeviceinfo.h>
#include <QtMultimedia/qaudiosystem.h>

QT_BEGIN_NAMESPACE


const unsigned int MAX_SAMPLE_RATES = 5;
const unsigned int SAMPLE_RATES[] =
    { 8000, 11025, 22050, 44100, 48000 };

class QAlsaAudioDeviceInfo : public QAbstractAudioDeviceInfo
{
    Q_OBJECT
public:
    QAlsaAudioDeviceInfo(QByteArray dev,QAudio::Mode mode);
    ~QAlsaAudioDeviceInfo();

    bool testSettings(const QAudioFormat& format) const;
    void updateLists();
    QAudioFormat preferredFormat() const;
    bool isFormatSupported(const QAudioFormat& format) const;
    QString deviceName() const;
    QStringList supportedCodecs();
    QList<int> supportedSampleRates();
    QList<int> supportedChannelCounts();
    QList<int> supportedSampleSizes();
    QList<QAudioFormat::Endian> supportedByteOrders();
    QList<QAudioFormat::SampleType> supportedSampleTypes();
    static QByteArray defaultDevice(QAudio::Mode mode);
    static QList<QByteArray> availableDevices(QAudio::Mode);
    static QString deviceFromCardName(const QString &card);

private:
    bool open();
    void close();

    void checkSurround();
    bool surround40;
    bool surround51;
    bool surround71;

    QString device;
    QAudio::Mode mode;
    QAudioFormat nearest;
    QList<int> sampleRatez;
    QList<int> channelz;
    QList<int> sizez;
    QList<QAudioFormat::Endian> byteOrderz;
    QStringList codecz;
    QList<QAudioFormat::SampleType> typez;
    snd_pcm_t* handle;
    snd_pcm_hw_params_t *params;
};

QT_END_NAMESPACE


#endif // QALSAAUDIODEVICEINFO_H
