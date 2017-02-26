#include "abstractvolumemodel.h"

// This whole section is copied from ksmedia.h to avoid problems with it expecting stuff to be
// included.

// Speaker Positions:
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000

// Bit mask locations reserved for future use
#define SPEAKER_RESERVED                0x7FFC0000

// Used to specify that any possible permutation of speaker configurations
#define SPEAKER_ALL                     0x80000000

// DirectSound Speaker Config
#define KSAUDIO_SPEAKER_DIRECTOUT       0
#define KSAUDIO_SPEAKER_MONO            (SPEAKER_FRONT_CENTER)
#define KSAUDIO_SPEAKER_1POINT1         (SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_STEREO          (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)
#define KSAUDIO_SPEAKER_2POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_3POINT0         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER)
#define KSAUDIO_SPEAKER_3POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_QUAD            (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT)
#define KSAUDIO_SPEAKER_SURROUND        (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER)
#define KSAUDIO_SPEAKER_5POINT0         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | \
                                         SPEAKER_SIDE_LEFT  | SPEAKER_SIDE_RIGHT)
#define KSAUDIO_SPEAKER_5POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT)
#define KSAUDIO_SPEAKER_7POINT0         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | \
                                         SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)
#define KSAUDIO_SPEAKER_7POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | \
                                         SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER)
#define KSAUDIO_SPEAKER_5POINT1_SURROUND (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_SIDE_LEFT  | SPEAKER_SIDE_RIGHT)
#define KSAUDIO_SPEAKER_7POINT1_SURROUND (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | \
                                         SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)

// We now return you to your regular scheduled source code.

AbstractVolumeModel::AbstractVolumeModel(QObject *parent) : QObject(parent)
{
}

AbstractVolumeModel::~AbstractVolumeModel() { }

QString channelNames[] = {
    QString("Front Left"),
    QString("Front Right"),
    QString("Front Center"),
    QString("LFE"),
    QString("Back Left"),
    QString("Back Right"),
    QString("Left of Center"),
    QString("Right of Center"),
    QString("Back Center"),
    QString("Side Left"),
    QString("Side Right"),
    QString("Top Center"),
    QString("Top Front Left"),
    QString("Top Front Center"),
    QString("Top Front Right"),
    QString("Top Back Left"),
    QString("Top Back Center"),
    QString("Top Back Right"),
    QString("Ch. %0")
};

struct nameTabEntry {
    uint maskmask;
    uint maskmatch;
    int names[18];
};

nameTabEntry nameTable[] = {
    { 0xFFFFFFFF, 0,   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, // KSAUDIO_SPEAKER_DIRECTOUT
    { 0xFFFFFFFF, 0x4, { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, // KSAUDIO_SPEAKER_MONO
    
    
    { 0, 0, { 0 } } // End of table
};

QString AbstractVolumeModel::channelName(uint channel) {
    uint lm = this->channelLayoutMask();
    
    if(lm == KSAUDIO_SPEAKER_DIRECTOUT || channel > 17) {
        return QString("Ch. %0").arg(channel);
    }
    
    if(lm == KSAUDIO_SPEAKER_MONO) {
        return channel == 0 ? QString("Mono") : QString("Ch. %0").arg(channel);
    }
    
    int chindex = 0;
    for (uint i = 0; i <= channel; i++) {
        while ((lm & (1 << chindex)) == 0 && chindex <= 18) chindex++;
    }
    
    return channelNames[chindex].arg(channel);
}

bool AbstractVolumeModel::hasDecibels() {
    return false;
}

float AbstractVolumeModel::decibelsMin() {
    return 0.0f;
}

float AbstractVolumeModel::decibelsMax() {
    return 0.0f;
}

float AbstractVolumeModel::decibels() {
    return 0.0f;
}

void AbstractVolumeModel::setDecibels(float dbVolume) {
    (void)dbVolume;
}
