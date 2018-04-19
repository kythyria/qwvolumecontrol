#ifndef SESSIONCREATIONSOURCE_H
#define SESSIONCREATIONSOURCE_H

#include <QObject>

#include <audiopolicy.h>

#include "comstuff.h"

COM_SMARTPTR(IAudioSessionManager2);
COM_SMARTPTR(IAudioSessionControl2);
COM_SMARTPTR(IAudioSessionNotification);

Q_DECLARE_METATYPE(IAudioSessionControl2Ptr)


class SessionCreationSource : public QObject
{
    Q_OBJECT
    
    class SessionNotificationCallback;
    
    friend class SessionNotificationCallback;
    SessionNotificationCallback *callback;
    IAudioSessionNotificationPtr callback_as_callback;
    
    IAudioSessionManager2Ptr manager;
    
public:
    explicit SessionCreationSource(IAudioSessionManager2Ptr sessionManager, QObject *parent = nullptr);
    virtual ~SessionCreationSource();
    
signals:
    void sessionExists(IAudioSessionControl2Ptr session);
    
public slots:
    void triggerEnumeration();
};

#endif // SESSIONCREATIONSOURCE_H
