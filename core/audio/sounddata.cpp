#include "sounddata.h"
#include "appdatapath.h"

#include <QFile>
#include "debug.h"

SystemSounds SoundData::id() const
{
    return _id;
}

QByteArray* SoundData::array() const
{
    return _array;
}

SoundData::SoundData(SystemSounds soundId, const QString& path)
    : _array(nullptr)
{
    _id = soundId;
    QFile mediaFile(soundsPath() + path);
    if (mediaFile.exists()) {
        mediaFile.open(QIODevice::ReadOnly);
        wav_hdr hdr;

        mediaFile.read(hdr.RIFF, 4);
        Q_ASSERT(hdr.RIFF[0] == 'R' && hdr.RIFF[1] == 'I' && hdr.RIFF[2] == 'F' && hdr.RIFF[3] == 'F');
        mediaFile.read(reinterpret_cast<char*>(&hdr.ChunkSize), 4);
        mediaFile.read(hdr.WAVE, 4);
        Q_ASSERT(hdr.WAVE[0] == 'W' && hdr.WAVE[1] == 'A' && hdr.WAVE[2] == 'V' && hdr.WAVE[3] == 'E');
        mediaFile.read(hdr.fmt, 4);
        mediaFile.read(reinterpret_cast<char*>(&hdr.Subchunk1Size), 4);
        mediaFile.read(reinterpret_cast<char*>(&hdr.AudioFormat), 2);
        mediaFile.read(reinterpret_cast<char*>(&hdr.NumOfChan), 2);
        mediaFile.read(reinterpret_cast<char*>(&hdr.SamplesPerSec), 4);
        mediaFile.read(reinterpret_cast<char*>(&hdr.bytesPerSec), 4);
        mediaFile.read(reinterpret_cast<char*>(&hdr.blockAlign), 2);
        mediaFile.read(reinterpret_cast<char*>(&hdr.bitsPerSample), 2);
        mediaFile.read(hdr.Subchunk2ID, 4);
        mediaFile.read(reinterpret_cast<char*>(&hdr.Subchunk2Size), 4);

        QDEBUG << "Loading wav:" << path;
        QDEBUG << "Bits per sample:" << hdr.bitsPerSample;
        QDEBUG << "Bytes per second:" << hdr.bytesPerSec;
        QDEBUG << "Num of channels:" << hdr.NumOfChan;
        QDEBUG << "Samples per second:" << hdr.SamplesPerSec;
        _array = new QByteArray(mediaFile.readAll());
        mediaFile.close();
    }
    else {
        qDebug() << "File:" << soundsPath() + path << "not found";
        Q_ASSERT(false);
    }
}

SoundData::~SoundData()
{
    if (_array != nullptr) {
        delete _array;
    }
}
