#include <QApplication>
#include <QBuffer>
#include <QThread>
#include "audiooutput.h"
#ifndef QT_NO_DEBUG
#define QT_NO_DEBUG
#endif
#include "debug.h"

const int DataSampleRateHz = 16000;

AudioOutput::AudioOutput(QObject* parent)
    : QObject(parent)
    , _audioOutput(nullptr)
    , _generator(nullptr)
    , _isStarted(false)
{
    qRegisterMetaType<AudioGenerator::Tone>("AudioGenerator::Tone");
    init();
}

AudioOutput::~AudioOutput()
{
    qDebug() << "Deleting AudioOutput...";
    if (_audioOutput != nullptr) {
        _audioOutput->stop();
        delete _audioOutput;
    }

    if (_generator != nullptr) {
        _generator->stop();
        delete _generator;
    }

    _sounds.clear();
}

void AudioOutput::play()
{
    Q_ASSERT(_generator != nullptr);
    _generator->apply();
}

void AudioOutput::stop()
{
    stopPlay();
}

void AudioOutput::resume()
{
    startPlay();
}

void AudioOutput::setVolume(double value)
{
    Q_ASSERT(_audioOutput != nullptr);
    _audioOutput->setVolume(qreal(value / 100.0f));
}

double AudioOutput::volume()
{
    Q_ASSERT(_audioOutput != nullptr);
    return (_audioOutput->volume() * 100.0f);
}

void AudioOutput::setTone(AudioGenerator::Tone tone)
{
    Q_ASSERT(_generator != nullptr);
    _generator->setTone(tone);
}

void AudioOutput::clearTone(AudioGenerator::Tone tone)
{
    Q_ASSERT(_generator != nullptr);
    _generator->clearTone(tone);
}

void AudioOutput::playSound(SystemSounds sound)
{
    for (const auto& data : _sounds) {
        if (data.id() == sound) {
            stop();
            if (_buffer.isOpen()) {
                _audioOutput->stop();
            }
            while (_buffer.isOpen()) {
                qApp->processEvents();
                QThread::msleep(10);
            }
            _buffer.setBuffer(data.array());
            _buffer.open(QIODevice::ReadOnly);
            _audioOutput->start(&_buffer);
            break;
        }
    }
}

void AudioOutput::playSound(const QString& soundPath)
{
    Q_ASSERT(_audioOutput != nullptr);
    if (_buffer.isOpen()) {
        _audioOutput->stop();
    }
    while (_buffer.isOpen()) {
        qApp->processEvents();
    }

    QFile* file = new QFile(soundPath);
    if (file->open(QFile::ReadOnly)) {
        _audioOutput->start(file);
    }
}

void AudioOutput::notified()
{
    QDEBUG << "bytesFree = " << _audioOutput->bytesFree() << ", "
           << "elapsedUSecs = " << _audioOutput->elapsedUSecs() << ", "
           << "processedUSecs = " << _audioOutput->processedUSecs() << ", "
           << "error = " << _audioOutput->error();
}

void AudioOutput::handleStateChanged(QAudio::State state)
{
    Q_ASSERT(_audioOutput != nullptr);
    QDEBUG << "state = " << state;
    switch (state) {
    case QAudio::ActiveState:
        break;
    case QAudio::StoppedState:
        // Stopped for other reasons
        if (_audioOutput && _audioOutput->error() != QAudio::NoError) {
            // Error handling
            _audioOutput->stop();
        }
        else {
            QWARNING << "Audio ouput error:" << _audioOutput->error();
        }
        _buffer.close();
        _isStarted = false;
        break;
    case QAudio::IdleState:
        // Finished playing (no more data)
        _buffer.close();
        _isStarted = false;
        startPlay();
        break;
    default:
        break;
    }
}

void AudioOutput::init()
{
    Q_ASSERT(_audioOutput == nullptr);

    qDebug() << "Initializing audio output...";
    loadSounds();

    QAudioFormat format;
    format.setSampleRate(DataSampleRateHz);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        QWARNING << "Default format not supported - trying to use nearest";
        format = info.nearestFormat(format);
    }

    _audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), format, this);

    Q_ASSERT(_audioOutput);
    ASSERT(connect(_audioOutput, &QAudioOutput::notify, this, &AudioOutput::notified));
    ASSERT(connect(_audioOutput, &QAudioOutput::stateChanged, this, &AudioOutput::handleStateChanged));

    _generator = new AudioGenerator(format, this);
    _generator->start();

    startPlay();
    qDebug() << "Audio output ready!";
}

void AudioOutput::startPlay()
{
    Q_ASSERT(_audioOutput != nullptr);

    if (!_isStarted) {
        _audioOutput->start(_generator);
        qDebug() << "Audio error:" << _audioOutput->error() << "State:" << _audioOutput->state();
        _isStarted = true;
    }
}

void AudioOutput::stopPlay()
{
    Q_ASSERT(_audioOutput != nullptr);
    if (_isStarted) {
        _audioOutput->stop();
        _isStarted = false;
    }
}

void AudioOutput::loadSounds()
{
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    _sounds.emplace_back(ACNotify, "noac.wav");
#endif
}

bool AudioOutput::isStarted() const
{
    return _isStarted;
}
