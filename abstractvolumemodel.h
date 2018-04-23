#ifndef ABSTRACTVOLUMEMODEL_H
#define ABSTRACTVOLUMEMODEL_H

#include <QObject>

class AbstractVolumeModel : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY muteChanged)
    Q_PROPERTY(float decibels READ decibels WRITE setDecibels)
    
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
    
    virtual float volume() = 0;
    virtual bool muted() = 0;
    
    virtual QString name() = 0;
    virtual QString description() = 0;
    
    virtual bool currentlyHasVolume() = 0;
    
signals:
    void volumeChanged(float volume);
    void nameChanged(QString newName);
    void muteChanged(bool newMute);
    
public slots:
    virtual void setVolume(float volume) = 0;
    virtual void setMuted(bool muted) = 0;
    virtual void setDecibels(float dbVolume);
    virtual void setName(QString newName);
};

#endif // ABSTRACTVOLUMEMODEL_H
