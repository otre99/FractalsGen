#include "fractals.h"
#include "iostream"
#include <numeric>
#include <limits>
#include <functional>

void Family01::Init(const FractalParameters &p) {
    Fractal::Init(p);
    c_.real(p.c.real());
    c_.imag(p.c.imag());
}

float Family01::CalcFinalNormMandelbrot(const cmplx &c) const
{
    int iter=1;
    dbltype x, y, tmp1, tmp2;
    x = c_.real();
    y = c_.imag();
    dbltype dist  = std::numeric_limits<float>::max();
    for (; iter<max_iter_;++iter) {
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;
        dist = std::min(orbit_metric_funct_(tmp1, tmp2), dist);
        if (x+y>=th_norm_) break;
        x += -y+c.real();
        y  = 2*tmp1*tmp2 + c.imag();
    }
    return dist;
}


float Family01::CalcFinalNormJulia(const cmplx &z) const {
    int iter=0;
    dbltype x, y;
    dbltype tmp1, tmp2;
    x = z.real();
    y = z.imag();
    dbltype dist  = std::numeric_limits<dbltype>::max();
    for (; iter<max_iter_;++iter) {
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;
        dist = std::min(orbit_metric_funct_(tmp1, tmp2), dist);
        if (x+y>=th_norm_) break;

        x += -y+c_.real();
        y  = 2*tmp1*tmp2 + c_.imag();
    }
    return dist;
}

//////////////////////////////////////////
float Family01::CalcEscapeJulia(const cmplx &z) const {
    int iter=0;
    dbltype x, y, tmp1, tmp2;

    x = z.real();
    y = z.imag();
    for (;;){
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;
        if (x+y >= th_norm_ || iter >= max_iter_) break;
        x += -y+c_.real();
        y  = 2*tmp1*tmp2+c_.imag();
        ++iter;
    }
    return iter;
}

float Family01::CalcEscapeMandelbrot(const cmplx &c) const {
    int iter=1;
    dbltype x, y;
    dbltype tmp1, tmp2;
    x = c_.real();
    y = c_.imag();
    for (;;){
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;
        if (x+y >= th_norm_ || iter >= max_iter_) break;
        x += -y+c.real();
        y  = 2*tmp1*tmp2+c.imag();
        ++iter;
    }
    return iter;
}
