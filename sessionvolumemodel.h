#ifndef SESSIONVOLUMEMODEL_H
#define SESSIONVOLUMEMODEL_H

#include "abstractvolumemodel.h"

#include <audiopolicy.h>
#include "comstuff.h"

COM_SMARTPTR(IAudioSessionControl2);

class SessionVolumeModel : public AbstractVolumeModel
{
    Q_OBJECT
    
    class Internal;
    friend class Internal;
    Internal *stuff;
    
public:
    explicit SessionVolumeModel(IAudioSessionControl2Ptr session, QObject *parent = 0);
    virtual ~SessionVolumeModel();
    
    virtual uint channelCount();
    virtual uint channelLayoutMask();
    virtual float channelVolume(uint channel);
    virtual void setChannelVolume(uint channel, float volume);
    
    virtual float volume();
    virtual void setVolume(float volume);
    
    virtual bool muted();
    virtual void setMuted(bool muted);
    
    virtual bool currentlyHasVolume();
    
signals:
    void iconPathChanged(QString newIconPath);
    void sessionDisconnected();
    void stateChanged(AudioSessionState newState);
    
public slots:
    void setIconPath(QString newIconPath);
    virtual void setName(QString newName);
    
public:
    virtual QString name();
    QString iconPath();
    
    Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath NOTIFY iconPathChanged)
    
};

#endif // SESSIONVOLUMEMODEL_H
