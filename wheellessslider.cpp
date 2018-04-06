#include "wheellessslider.h"
#include <QWheelEvent>

WheellessSlider::WheellessSlider(QWidget *parent) : QSlider(parent)
{
    
}

WheellessSlider::WheellessSlider(Qt::Orientation orientation, QWidget *parent) :
    QSlider(orientation, parent)
{ }

WheellessSlider::~WheellessSlider() { }

void WheellessSlider::wheelEvent(QWheelEvent *event) {
    event->ignore();
}
