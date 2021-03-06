#ifndef VOLUMESLIDERWIDGETINTERNAL_H
#define VOLUMESLIDERWIDGETINTERNAL_H

#include <QObject>
#include <QList>

#include "volumesliderwidget.h"

class AbstractVolumeModel;
class QLabel;
class WheellessSlider;
class QSignalMapper;
class QGridLayout;

class VolumeSliderWidget::Internal : public QObject
{
    Q_OBJECT
    
    friend class VolumeSliderWidget;
    
    QSharedPointer<AbstractVolumeModel> model;
    
    class SliderRow {
        QGridLayout *grid;
        
        QString formatPercentage(float factor);
        
    public:
        QLabel *lblName;
        QLabel *lblValue;
        WheellessSlider *slider;
        
        SliderRow(QString name, QGridLayout *grid, int row);
        ~SliderRow();
        
        void setVisible(bool visible);
        float value();
        void setValue(float val);
    };
    
    QList<SliderRow*> rows;
    QSignalMapper *mapper;
    QGridLayout *grid;
    
    bool linked;
    
    void initChannels();
    void destroyChannels();
    
public:
    explicit Internal(QSharedPointer<AbstractVolumeModel> model, VolumeSliderWidget *parent);
    virtual ~Internal();
    
    void setVolumeModel(QSharedPointer<AbstractVolumeModel> model);
    
signals:
    
public slots:
    void linkChannels(bool linked);
    void hideChannels(bool hidden);
    void setMasterVisible(bool visible);
    
private slots:
    void sliderMoved(int channel);
    void modelUpdated();
};

#endif // VOLUMESLIDERWIDGETINTERNAL_H
