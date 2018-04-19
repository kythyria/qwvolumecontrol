#ifndef SESSIONMIXERLISTWIDGET_H
#define SESSIONMIXERLISTWIDGET_H

#include <QWidget>
#include <memory>
#include <audiopolicy.h>
#include "comstuff.h"

COM_SMARTPTR(IAudioSessionManager2);
COM_SMARTPTR(IAudioSessionControl2);

class SessionMixerListWidget : public QWidget
{
    Q_OBJECT
    
    class Internal;
    std::unique_ptr<Internal> stuff;
    
public:
    SessionMixerListWidget(IAudioSessionManager2Ptr smgr);
    virtual ~SessionMixerListWidget();
    
private slots:
    void sessionExists(IAudioSessionControl2 *session);
};

#endif // SESSIONMIXERLISTWIDGET_H
