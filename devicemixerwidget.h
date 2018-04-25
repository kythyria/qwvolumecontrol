#ifndef DEVICEMIXERWIDGET_H
#define DEVICEMIXERWIDGET_H

#include <QWidget>
#include <QFrame>
#include <memory>

#include <mmdeviceapi.h>
#include "comstuff.h"
#include "abstractvolumemodel.h"

COM_SMARTPTR(IMMDevice);
class QAbstractItemModel;

class DeviceMixerWidget : public QFrame
{
    Q_OBJECT
    
    class Internals;
    
    Internals *stuff;
    
    DeviceMixerWidget(QWidget *parent = 0);
    
public:
    explicit DeviceMixerWidget(QSharedPointer<AbstractVolumeModel> device, QWidget *parent = 0);
    explicit DeviceMixerWidget(QAbstractItemModel *devices, QWidget *parent = 0);
    virtual ~DeviceMixerWidget();
    
    QSharedPointer<AbstractVolumeModel> model();
    void setModel(QSharedPointer<AbstractVolumeModel>);
    
    QAbstractItemModel *selectorModel();
    void setSelectorModel(QAbstractItemModel *items);
    void clearSelectorModel();
    
signals:
    void detailButtonClicked();
    void selectedIndexChanged(int index);
    
private slots:
    void refresh();
    void muteClicked(bool checked);
    void selectedIndexChanged_internal(int index);
};

#endif // DEVICEMIXERWIDGET_H
