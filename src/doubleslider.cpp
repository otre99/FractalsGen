#include "doubleslider.h"

DoubleSlider::DoubleSlider(QWidget *parent) : QSlider(parent), minVal_(0), maxVal_(1)
{
    connect(this, &DoubleSlider::sliderMoved, this, &DoubleSlider::notifyValueMoved);
}

void DoubleSlider::SetRange(const double &vmin, const double &vmax)
{
    minVal_ = vmin;
    maxVal_ = vmax;
}
