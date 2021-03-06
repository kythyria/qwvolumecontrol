#include "sessioncreationsource.h"

#include <QThread>
#include <QDebug>
#include <objbase.h>
#include "util.h"

REGISTER_METATYPE(IAudioSessionControl2Ptr)

COM_SMARTPTR(IAudioSessionEnumerator);
COM_SMARTPTR(IAudioSessionControl);

class SessionCreationSource::SessionNotificationCallback : IAudioSessionNotification {
    ULONG refcount;
    SessionCreationSource *manager;
    
public:
    SessionNotificationCallback(SessionCreationSource *src);
    virtual ~SessionNotificationCallback();
    
    // IUnknown
    virtual ULONG AddRef();
    virtual ULONG Release();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    
    // IAudioSessionNotification
    
    virtual HRESULT OnSessionCreated(IAudioSessionControl *NewSession);
};

SessionCreationSource::SessionNotificationCallback::SessionNotificationCallback(SessionCreationSource *src) :
    refcount(1),
    manager(src)
{ }

SessionCreationSource::SessionNotificationCallback::~SessionNotificationCallback() { }

COM_IMPL_REFCOUNT(SessionCreationSource::SessionNotificationCallback)
COM_IMPL_QUERYINTERFACE(SessionCreationSource::SessionNotificationCallback,
    COM_IMPL_QICASE(IAudioSessionNotification))

HRESULT SessionCreationSource::SessionNotificationCallback::OnSessionCreated(IAudioSessionControl *NewSession) {
    IAudioSessionControl2 *s2;
    HRESULT hr = NewSession->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&s2);
    if(FAILED(hr)) {
        qDebug() << "Couldn't cast session (" << hr << ")";
    }
    
    emit manager->notificationTrampoline(s2);
    return S_OK;
}

class SessionNotificationFactoryThread : QThread {
protected:
    void run() {
        auto hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        
        if(FAILED(hr)) {
            QString failmsg = QString("CoInitializeEx failed in session factory thread (%0)").arg(hr);
            wchar_t *msgdata = new wchar_t[failmsg.length()];
            failmsg.toWCharArray(msgdata);
            MessageBox(NULL, msgdata, L"Volume Control", MB_OK);
            delete[] msgdata;
            return;
        }
        
        this->exec();
        
        CoUninitialize();
    }
    
private:
    static SessionNotificationFactoryThread *factorythread;
public:
    static SessionNotificationFactoryThread *runningThread() { 
        if(factorythread == nullptr) {
            factorythread = new SessionNotificationFactoryThread();
            factorythread->start();
        }
        
        return factorythread;
    }
};

SessionNotificationFactoryThread *SessionNotificationFactoryThread::factorythread = nullptr;

SessionCreationSource::SessionCreationSource(IAudioSessionManager2Ptr sessionManager, QObject *parent) :
    QObject(parent),
    manager(sessionManager)
{
    SessionNotificationFactoryThread::runningThread();
    
    connect(this, &SessionCreationSource::notificationTrampoline, this, &SessionCreationSource::notificationTrampolineLanding, Qt::QueuedConnection);
    
    this->callback = new SessionNotificationCallback(this);
    
    HRESULT hr;
    hr = this->callback->QueryInterface(__uuidof(IAudioSessionNotification), (void**)&callback_as_callback);
    assertHR(hr, "Somehow failed to cast the session notification callback (%0)");
    
    this->manager->RegisterSessionNotification(this->callback_as_callback);
}

SessionCreationSource::~SessionCreationSource() {
    this->manager->UnregisterSessionNotification(this->callback_as_callback);
    this->callback->Release();
}

void SessionCreationSource::notificationTrampolineLanding(IAudioSessionControl2 *session) {
    IAudioSessionControl2Ptr s(session);
    emit sessionExists(s);
}

void SessionCreationSource::triggerEnumeration() {
    IAudioSessionEnumeratorPtr enumer;
    int sessionCount;
    
    HRESULT hr;
    
    hr = this->manager->GetSessionEnumerator(&enumer);
    assertHR(hr, "Failed to get session enumerator (%0)");
    
    hr = enumer->GetCount(&sessionCount);
    assertHR(hr, "Failed to get session count (%0)");
    
    for(int i = 0; i < sessionCount; i++) {
        IAudioSessionControlPtr session;
        hr = enumer->GetSession(i, &session);
        if(FAILED(hr)) {
            qDebug() << "Couldn't get session " << i;
            continue;
        }
        IAudioSessionControl2Ptr s2;
        hr = session.QueryInterface(__uuidof(IAudioSessionControl2), &s2);
        if(FAILED(hr)) {
            qDebug() << "Couldn't cast session " << i;
            continue;
        }
        
        emit sessionExists(s2.GetInterfacePtr());
    }
}
