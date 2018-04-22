#include "abstractvolumemodel.h"
#include "speakerformats.h"

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
    QString("Top Back Right")
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
    
    if (lm == KSAUDIO_SPEAKER_STEREO) {
        switch(channel) {
        case 0:
            return QString("Left");
        case 1:
            return QString("Right");
        }
    }
    
    int chindex = -1;
    uint i;
    for (i = 0; i < 31; i++) {
        uint bit = (lm >> i) & 1;
        if(bit == 1) { chindex++; }
        if(chindex == channel) {
            break;
        }
    }
    
    return channelNames[i];
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

void AbstractVolumeModel::setName(QString newName) {
    (void)newName;
}
