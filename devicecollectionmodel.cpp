#include "devicecollectionmodel.h"

#include <QMap>

#include "util.h"

COM_SMARTPTR(IMMDeviceCollection);

class DeviceCollectionModel::Internal {
public:
    IMMDeviceCollectionPtr devices;
    QMap<int, QSharedPointer<DeviceVolumeModel>> models;
};

DeviceCollectionModel::DeviceCollectionModel(IMMDeviceEnumeratorPtr enumerator, EDataFlow direction, DWORD stateMask)
{
    stuff = std::make_unique<Internal>();
    
    IMMDeviceCollectionPtr col;
    HRESULT hr = enumerator->EnumAudioEndpoints(direction, stateMask, &col);
    assertHR(hr, "Couldn't enumerate devices (%0)");
    
    stuff->devices = col;
    
    uint numDevices = rowCount();
    
    for(uint i = 0; i < numDevices; i++) {
        IMMDevicePtr dev;
        if(!AlertHresult(stuff->devices->Item(i, &dev), QString("Couldn't get device %0 (%1)").arg(i))) {
            return;
        }
        
        QSharedPointer<DeviceVolumeModel> dvm(new DeviceVolumeModel(dev));
        stuff->models.insert(i, dvm);
    }
}

int DeviceCollectionModel::rowCount(const QModelIndex &parent) const {
    (void)parent;
    uint numDevices;
    
    if(!AlertHresult(stuff->devices->GetCount(&numDevices), "Couldn't get device count(%0)")) {
        return 0;
    }
    return numDevices;
}

QVariant DeviceCollectionModel::data(const QModelIndex &index, int role) const {
    if(!stuff->models.contains(index.row())) {
        return QVariant();
    }
    
    auto item = stuff->models[index.row()];
    
    switch(role) {
    case Qt::DisplayRole:
        return QString("%0 (%1)").arg(item->name()).arg(item->description());
        
    case Qt::UserRole:
        return QVariant::fromValue(item);
        
    default:
        return QVariant();
    }
}
