#ifndef WHEELLESSSLIDER_H
#define WHEELLESSSLIDER_H

#include <QSlider>

class WheellessSlider : public QSlider
{
public:
    WheellessSlider(QWidget *parent = Q_NULLPTR);
    WheellessSlider(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR);
    virtual ~WheellessSlider();
    
protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // WHEELLESSSLIDER_H
