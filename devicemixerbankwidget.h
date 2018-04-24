#ifndef DEVICEMIXERBANKWIDGET_H
#define DEVICEMIXERBANKWIDGET_H

#include <QWidget>
#include <memory>

#include <QAbstractItemModel>

class DeviceMixerBankWidget : public QWidget
{
    Q_OBJECT
    
    class Internal;
    std::unique_ptr<Internal> stuff;
    
public:
    explicit DeviceMixerBankWidget(QAbstractItemModel *devices, QWidget *parent = nullptr);
    
signals:
    
private slots:
    void selectedDeviceChanged_internal(int index);
};

#endif // DEVICEMIXERBANKWIDGET_H
