#ifndef DEVICECOLLECTIONMODEL_H
#define DEVICECOLLECTIONMODEL_H

#include <mmdeviceapi.h>
#include <memory>
#include "comstuff.h"
#include "devicevolumemodel.h"

#include <QAbstractListModel>

COM_SMARTPTR(IMMDeviceEnumerator);
COM_SMARTPTR(IMMDevice);
Q_DECLARE_METATYPE(IMMDevicePtr)
Q_DECLARE_METATYPE(QSharedPointer<DeviceVolumeModel>)

class DeviceCollectionModel : public QAbstractListModel
{
    class Internal;
    std::unique_ptr<Internal> stuff;
    
public:
    // TODO: Allow changing the state mask/direction afterwards
    // TODO: Respond to changes in available devices
    DeviceCollectionModel(IMMDeviceEnumeratorPtr enumerator, EDataFlow direction, DWORD stateMask);
    
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
};

#endif // DEVICECOLLECTIONMODEL_H
