#ifndef AUDIOGENERATOR_H
#define AUDIOGENERATOR_H

#include <QIODevice>
#include <QFlags>
#include <QMap>

class QByteArray;
class QAudioFormat;

class AudioGenerator : public QIODevice
{
    Q_OBJECT

public:
    AudioGenerator(const QAudioFormat& format, QObject* parent = 0);
    ~AudioGenerator();

    void start();
    void stop();

    enum Tone
    {
        left500HzBlink = 1 << 0,
        left500Hz = 1 << 1,
        left1000Hz = 1 << 2,
        left2000Hz = 1 << 3,
        left2000HzBlink = 1 << 4,
        right500HzBlink = 1 << 5,
        right500Hz = 1 << 6,
        right1000Hz = 1 << 7,
        right2000Hz = 1 << 8,
        right2000HzBlink = 1 << 9
    };
    Q_DECLARE_FLAGS(Tons, Tone)

    void setTone(Tone tones);
    void clearTone(Tone tones);
    void apply();
    AudioGenerator::Tons tones();

    qint64 readData(char* data, qint64 maxlen);
    qint64 writeData(const char* data, qint64 len);
    qint64 bytesAvailable() const;
    bool atEnd() const;

private:
    void generateData(const QAudioFormat& format, unsigned int samples);

private:
    std::vector<QByteArray*> _buffersVector;
    QMap<Tone, int> _useCounter;
    QByteArray* _buffer;
    int _bufferSize;
    Tons _tons;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AudioGenerator::Tons)

#endif  // AUDIOGENERATOR_H
