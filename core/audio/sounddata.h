#ifndef SOUNDDATA_H
#define SOUNDDATA_H

#include "coredefinitions.h"
#include <QString>
#include <QByteArray>
#include <QBuffer>

#pragma pack(push, 1)

typedef struct WAV_HEADER
{
    char RIFF[4];            // RIFF Header      Magic header
    int32_t ChunkSize;       // RIFF Chunk Size
    char WAVE[4];            // WAVE Header
    char fmt[4];             // FMT header
    uint32_t Subchunk1Size;  // Size of the fmt chunk
    uint16_t AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t bytesPerSec;    // bytes per second
    uint16_t blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample;  // Number of bits per sample
    char Subchunk2ID[4];     // "data"  string
    uint32_t Subchunk2Size;  // Sampled data length

} wav_hdr;

#pragma pack(pop)


class SoundData
{
    QByteArray* _array;
    SystemSounds _id;

public:
    SoundData(SystemSounds soundId, const QString& path);
    ~SoundData();

    SystemSounds id() const;
    QByteArray* array() const;
};

#endif  // SOUNDDATA_H
