#ifndef DEVICEMIXERLISTWIDGET_H
#define DEVICEMIXERLISTWIDGET_H

#include <QWidget>
#include <memory>
#include <mmdeviceapi.h>
#include <comdef.h>
#include <comip.h>

class DeviceMixerListWidget_internal;
_COM_SMARTPTR_TYPEDEF(IMMDeviceCollection, __uuidof(IMMDeviceCollection));

class DeviceMixerListWidget : public QWidget
{
    Q_OBJECT
    
    std::unique_ptr<DeviceMixerListWidget_internal> stuff;
    
public:
    explicit DeviceMixerListWidget(IMMDeviceCollectionPtr devices, QWidget *parent = 0);
    virtual ~DeviceMixerListWidget();
signals:
    
public slots:
    void UpdateListing();
};

#endif // DEVICEMIXERLISTWIDGET_H
