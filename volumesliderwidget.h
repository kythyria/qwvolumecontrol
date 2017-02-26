#ifndef VOLUMESLIDERWIDGET_H
#define VOLUMESLIDERWIDGET_H

#include <QWidget>

class AbstractVolumeModel;

class VolumeSliderWidget : QWidget
{
    class Internal;
    Internal *stuff;
    
public:
    explicit VolumeSliderWidget(QWidget *parent = 0);
    virtual ~VolumeSliderWidget();
    
    AbstractVolumeModel *volumeModel();
    void setVolumeModel(AbstractVolumeModel *model);
    
    // This is backwards from what channelsVisible() would be so that it matches the pushedness
    // of a QPushButton
    bool channelsHidden();
    
    bool channelsLinked();
    bool masterVisible();
    
public slots:
    void linkChannels(bool linked);
    void hideChannels(bool hidden);
    void setMasterVisible(bool visible);
};

#endif // VOLUMESLIDERWIDGET_H
