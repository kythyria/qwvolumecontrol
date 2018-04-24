#ifndef DEVICEMIXERWIDGET_H
#define DEVICEMIXERWIDGET_H

#include <QWidget>
#include <memory>

#include <mmdeviceapi.h>
#include "comstuff.h"
#include "abstractvolumemodel.h"

COM_SMARTPTR(IMMDevice);

class DeviceMixerWidget : public QWidget
{
    Q_OBJECT
    
    class Internals;
    
    Internals *stuff;
    
public:
    explicit DeviceMixerWidget(QSharedPointer<AbstractVolumeModel> device, QWidget *parent = 0);
    virtual ~DeviceMixerWidget();
    
    QSharedPointer<AbstractVolumeModel> model();
    
signals:
    void detailButtonClicked();
    
private slots:
    void refresh();
    void muteClicked(bool checked);
};

#endif // DEVICEMIXERWIDGET_H
