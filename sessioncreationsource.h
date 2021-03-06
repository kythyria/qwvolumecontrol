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
    void sessionExists(IAudioSessionControl2 *session);
    
    // Implementation detail: to avoid problems with the lifetime of the session,
    // this pair are used to bring execution onto the main thread safely.
    // They're literally just connected together and used for the marshalling
    // properties of signals.
    void notificationTrampoline(IAudioSessionControl2 *session);
    
private slots:
    void notificationTrampolineLanding(IAudioSessionControl2* session);
    
public slots:
    void triggerEnumeration();

};

#endif // SESSIONCREATIONSOURCE_H
