#ifndef ABSTRACTVOLUMEMODEL_H
#define ABSTRACTVOLUMEMODEL_H

#include <QObject>

class AbstractVolumeModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractVolumeModel(QObject *parent = 0);
    virtual ~AbstractVolumeModel();
    
    virtual int channelCount() = 0;
    virtual float channelVolume(int channel) = 0;
    virtual void setChannelVolume(int channel, float volume) = 0;
    
    virtual bool hasDecibels() = 0;
    virtual float decibelsMin() = 0;
    virtual float decibelsMax() = 0;
    virtual void setDecibels(float dbVolume) = 0;
    
    virtual float masterVolume() = 0;
    virtual void setMasterVolume(float volume) = 0;
    
signals:
    void volumeChanged();
    
public slots:
};

#endif // ABSTRACTVOLUMEMODEL_H
