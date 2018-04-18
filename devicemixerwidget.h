#ifndef DEVICEMIXERWIDGET_H
#define DEVICEMIXERWIDGET_H

#include <QWidget>
#include <memory>

#include <mmdeviceapi.h>
#include "comstuff.h"

COM_SMARTPTR(IMMDevice);

class DeviceMixerWidget : public QWidget
{
    Q_OBJECT
    
    class Internals;
    
    Internals *stuff;
    
public:
    explicit DeviceMixerWidget(IMMDevicePtr device, QWidget *parent = 0);
    virtual ~DeviceMixerWidget();
    
signals:
    
private slots:
    void refresh();
};

#endif // DEVICEMIXERWIDGET_H
