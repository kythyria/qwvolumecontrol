#ifndef SESSIONMIXERWIDGET_H
#define SESSIONMIXERWIDGET_H

#include <QWidget>
#include <memory>

#include <audiopolicy.h>
#include <comstuff.h>

COM_SMARTPTR(IAudioSessionControl2);

class SessionMixerWidget : public QWidget
{
    Q_OBJECT
    
    class Internal;
    std::unique_ptr<Internal> stuff;
    
public:
    explicit SessionMixerWidget(IAudioSessionControl2Ptr session, QWidget *parent = nullptr);
    
signals:
    
public slots:
};

#endif // SESSIONMIXERWIDGET_H
