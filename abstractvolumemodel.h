#ifndef ABSTRACTVOLUMEMODEL_H
#define ABSTRACTVOLUMEMODEL_H

#include <QObject>

class AbstractVolumeModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractVolumeModel(QObject *parent = 0);
    virtual ~AbstractVolumeModel();
    
    virtual uint channelCount() = 0;
    virtual uint channelLayoutMask() = 0;
    virtual QString channelName(uint channel);
    virtual float channelVolume(uint channel) = 0;
    virtual void setChannelVolume(uint channel, float volume) = 0;
    
    virtual bool hasDecibels();
    virtual float decibelsMin();
    virtual float decibelsMax();
    virtual float decibels();
    virtual void setDecibels(float dbVolume);
    
    virtual float volume() = 0;
    virtual void setVolume(float volume) = 0;
    
    virtual bool muted() = 0;
    virtual void setMuted(bool muted) = 0;
    
signals:
    void changed();
    
public slots:
};

#endif // ABSTRACTVOLUMEMODEL_H
