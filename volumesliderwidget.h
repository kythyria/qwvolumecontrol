#ifndef VOLUMESLIDERWIDGET_H
#define VOLUMESLIDERWIDGET_H

#include <QWidget>

class AbstractVolumeModel;

class VolumeSliderWidget : public QWidget
{
    class Internal;
    Internal *stuff;
    
public:
    explicit VolumeSliderWidget(AbstractVolumeModel *model, QWidget *parent = 0);
    virtual ~VolumeSliderWidget();
    
    AbstractVolumeModel *volumeModel();
    void setVolumeModel(AbstractVolumeModel *model);
    
public slots:
    void linkChannels(bool linked);
    void hideChannels(bool hidden);
    void setMasterVisible(bool visible);
};

#endif // VOLUMESLIDERWIDGET_H
