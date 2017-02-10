#ifndef DEVICEMIXERWIDGET_H
#define DEVICEMIXERWIDGET_H

#include <QWidget>
#include <memory>

#include <mmdeviceapi.h>
#include <comdef.h>
#include <comip.h>

_COM_SMARTPTR_TYPEDEF(IMMDevice, __uuidof(IMMDevice));

class DeviceMixerWidget : public QWidget
{
    Q_OBJECT
    
    class Internals;
    
    Internals *stuff;
    
public:
    explicit DeviceMixerWidget(IMMDevicePtr device, QWidget *parent = 0);
    virtual ~DeviceMixerWidget();
    
signals:
    
public slots:
};

#endif // DEVICEMIXERWIDGET_H
