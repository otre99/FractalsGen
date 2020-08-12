#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QObject>
#include <QSlider>

#include <QDebug>

class DoubleSlider : public QSlider
{
    Q_OBJECT
    double minVal_, maxVal_;
public:
    DoubleSlider(QWidget *parent = nullptr);
    void SetRange(const double &vmin, const double &vmax);
    void SetValueDouble(const double &val){
        const double mi = (maxVal_-minVal_)/(maximum()-minimum());
        const double ni = minVal_ - mi*minimum();
        setValue( (val-ni)/mi);
    }
    void SetMaximunDouble(const double &val){maxVal_ = val;}
    void SetManimunDouble(const double &val){minVal_ = val;}

signals:
    void doubleValueMoved(double value);

public slots:
    void notifyValueMoved() {
        const int A = minimum();
        const int B = maximum();
        const double mi = (maxVal_-minVal_)/(B-A);
        const double ni = minVal_ - mi*A;
        emit doubleValueMoved(mi*value() + ni);
    }
};

#endif // DOUBLESLIDER_H
