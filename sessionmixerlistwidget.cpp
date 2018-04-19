#include "sessionmixerlistwidget.h"
#include "sessioncreationsource.h"

#include <QDebug>

class SessionMixerListWidget::Internal : public QObject {
    
public:
    SessionCreationSource *sessionSource;
    
    Internal(QObject *parent = nullptr);
    virtual ~Internal();
    
};

SessionMixerListWidget::SessionMixerListWidget(IAudioSessionManager2Ptr smgr)
{
    stuff = new SessionMixerListWidget::Internal(this);
    stuff->sessionSource = new SessionCreationSource(smgr);
    connect(stuff->sessionSource, &SessionCreationSource::sessionExists, this, &SessionMixerListWidget::sessionExists);
    stuff->sessionSource->triggerEnumeration();
}

SessionMixerListWidget::~SessionMixerListWidget() { }

SessionMixerListWidget::Internal::Internal(QObject *parent) : QObject(parent) { }
SessionMixerListWidget::Internal::~Internal() { }


void SessionMixerListWidget::sessionExists(IAudioSessionControl2Ptr session) {
    LPWSTR sessioniid;
    HRESULT hr = session->GetSessionInstanceIdentifier(&sessioniid);
    if(FAILED(hr)) {
        qDebug() << "Failed to get session instance identifier (" << hr << ")";
    }
    else {
        qDebug() << "Saw session: " << sessioniid;
        CoTaskMemFree(sessioniid);
    }
}
