#include <QAudioDeviceInfo>
#include <qmath.h>
#include <qendian.h>
#include <array>
#include "audiogenerator.h"

#ifndef QT_NO_DEBUG
#define QT_NO_DEBUG
#endif
#include "debug.h"
//#define BENCHMARK_AUDIO_GENERATION
#ifdef BENCHMARK_AUDIO_GENERATION
#include <chrono>
#endif

#define BlinkPeriodMs 50
#define PIX2 6.28318530718f


AudioGenerator::AudioGenerator(const QAudioFormat& format, QObject* parent)
    : QIODevice(parent)
    , _buffer(nullptr)
    , _bufferSize(0)
    , _tons(0)
{
    qRegisterMetaType<AudioGenerator::Tone>("AudioGenerator::Tone");

    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;
    const int samples = (format.sampleRate() / 1000) * BlinkPeriodMs;
    _bufferSize = samples * sampleBytes;
    _buffersVector.resize(1024);
    for (unsigned short i = 0; i < 1024; ++i) {
        _buffersVector[i] = new QByteArray(_bufferSize, 0);
    }
    _buffer = _buffersVector.at(0);
    qDebug() << "Audio gen started...";
#ifdef BENCHMARK_AUDIO_GENERATION
    auto start = std::chrono::steady_clock::now();
#endif
    generateData(format, samples);
#ifdef BENCHMARK_AUDIO_GENERATION
    auto end = std::chrono::steady_clock::now();
    auto elapsed = end - start;
    qDebug() << "Audio generation time:" << elapsed.count();
#endif
    qDebug() << "Audio gen finished!";
    _useCounter[left500HzBlink] = 0;
    _useCounter[left500Hz] = 0;
    _useCounter[left1000Hz] = 0;
    _useCounter[left2000Hz] = 0;
    _useCounter[left2000HzBlink] = 0;
    _useCounter[right500HzBlink] = 0;
    _useCounter[right500Hz] = 0;
    _useCounter[right1000Hz] = 0;
    _useCounter[right2000Hz] = 0;
    _useCounter[right2000HzBlink] = 0;
}

AudioGenerator::~AudioGenerator()
{
    qDebug() << "Deleting AudioGenerator...";
    stop();
    _buffer = nullptr;
    for (auto& buffer : _buffersVector) {
        delete buffer;
    }
}

void AudioGenerator::start()
{
    open(QIODevice::ReadOnly);
}

void AudioGenerator::stop()
{
    close();
}

void AudioGenerator::setTone(AudioGenerator::Tone tone)
{
    if (_useCounter[tone] == 0) {
        _tons |= (tone);
    }
    _useCounter[tone]++;
}

void AudioGenerator::clearTone(AudioGenerator::Tone tone)
{
    if (_useCounter[tone] == 0) {
        return;
    }
    if (--_useCounter[tone] == 0) {
        _tons &= (~static_cast<unsigned int>(tone));
    }
}

void AudioGenerator::apply()
{
    _buffer = _buffersVector[_tons];
}

AudioGenerator::Tons AudioGenerator::tones()
{
    return _tons;
}

struct Sinusoid
{
    explicit Sinusoid(int formatSampleRate = 16000)
        : _sampleIndex(0)
        , _formatSampleRate(formatSampleRate)
        , temp(0)
    {
        formatConst = PIX2 / _formatSampleRate;
        temp = formatConst * static_cast<float>(_sampleIndex % _formatSampleRate);
    }

    inline float getSample(int sampleRate)
    {
        return std::sin(sampleRate * temp);
    }

    inline void nextIndex()
    {
        ++_sampleIndex;
        temp = formatConst * static_cast<float>(_sampleIndex % _formatSampleRate);
    }
    int _sampleIndex;
    int _formatSampleRate;
    float temp;
    float formatConst;
};

void AudioGenerator::generateData(const QAudioFormat& format, unsigned int samples)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;

    Q_ASSERT(_bufferSize % sampleBytes == 0);
    Q_UNUSED(sampleBytes)  // suppress warning in release builds
    Q_ASSERT(format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt && format.byteOrder() == QAudioFormat::LittleEndian);

    std::array<unsigned char*, 1024> ptr;
    for (unsigned int i = 0; i < 1024; ++i) {
        ptr[i] = reinterpret_cast<unsigned char*>(_buffersVector[i]->data());
    }
    Sinusoid sinusoid(format.sampleRate());
    for (unsigned int i = 0; i < samples; ++i) {
        const float x500Hz = sinusoid.getSample(500);
        const float x1000Hz = sinusoid.getSample(1000) * 0.7f;
        const float x2000Hz = sinusoid.getSample(2000) * 0.5f;
        sinusoid.nextIndex();
        for (unsigned int tone = 0; tone < 1024; ++tone) {
            float xLeft = 0;
            int divider = 0;
            if ((tone & AudioGenerator::left500HzBlink) != 0u) {
                if (i > (samples >> 1)) {
                    xLeft += x500Hz;
                }
                ++divider;
            }
            if ((tone & AudioGenerator::left500Hz) != 0u) {
                xLeft += x500Hz;
                ++divider;
            }
            if ((tone & AudioGenerator::left1000Hz) != 0u) {
                xLeft += x1000Hz;
                ++divider;
            }
            if ((tone & AudioGenerator::left2000Hz) != 0u) {
                xLeft += x2000Hz;
                ++divider;
            }
            if ((tone & AudioGenerator::left2000HzBlink) != 0u) {
                if (i > (samples >> 1)) {
                    xLeft += x2000Hz;
                }
                ++divider;
            }
            if (divider != 0) {
                xLeft /= divider;
            }

            float xRight = 0;
            divider = 0;
            if ((tone & AudioGenerator::right500HzBlink) != 0u) {
                if (i > (samples >> 1)) {
                    xRight += x500Hz;
                }
                ++divider;
            }
            if ((tone & AudioGenerator::right500Hz) != 0u) {
                xRight += x500Hz;
                ++divider;
            }
            if ((tone & AudioGenerator::right1000Hz) != 0u) {
                xRight += x1000Hz;
                ++divider;
            }
            if ((tone & AudioGenerator::right2000Hz) != 0u) {
                xRight += x2000Hz;
                ++divider;
            }
            if ((tone & AudioGenerator::right2000HzBlink) != 0u) {
                if (i > (samples >> 1)) {
                    xRight += x2000Hz;
                }
                ++divider;
            }
            if (divider != 0) {
                xRight /= divider;
            }

            qint16 value = 0;
            value = static_cast<qint16>(xLeft * 32767);
            qToLittleEndian<qint16>(value, ptr[tone]);
            ptr[tone] += channelBytes;
            value = static_cast<qint16>(xRight * 32767);
            qToLittleEndian<qint16>(value, ptr[tone]);
            ptr[tone] += channelBytes;
        }
    }
}

qint64 AudioGenerator::readData(char* data, qint64 len)
{
    qint64 avail = _buffer->size();
    qint64 total = avail > len ? len : avail;

    std::copy_n(_buffer->constData(), total, data);

    return total;
}

qint64 AudioGenerator::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 AudioGenerator::bytesAvailable() const
{
    qint64 available = _buffer->size();
    if (available == 0) {
        QWARNING << "bytes available is 0";
    }
    return available;
}

bool AudioGenerator::atEnd() const
{
    return false;
}
