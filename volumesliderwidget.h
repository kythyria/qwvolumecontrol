#ifndef VOLUMESLIDERWIDGET_H
#define VOLUMESLIDERWIDGET_H

#include <QWidget>

class AbstractVolumeModel;

class VolumeSliderWidget : public QWidget
{
    class Internal;
    Internal *stuff;
    
public:
    explicit VolumeSliderWidget(QSharedPointer<AbstractVolumeModel> model, QWidget *parent = 0);
    virtual ~VolumeSliderWidget();
    
    QSharedPointer<AbstractVolumeModel> volumeModel();
    void setVolumeModel(QSharedPointer<AbstractVolumeModel> model);
    
public slots:
    void linkChannels(bool linked);
    void hideChannels(bool hidden);
    void setMasterVisible(bool visible);
};

#endif // VOLUMESLIDERWIDGET_H
