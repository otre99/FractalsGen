#include "fractals.h"
#include "iostream"
#include <numeric>
#include <limits>
#include <functional>



void Family04::Init(const FractalParameters &p) {
    Fractal::Init(p);
    c_.real(p.c.real()); c_.imag(p.c.imag());
    q_.real(p.q.real()); q_.imag(p.q.imag());
    n_ = p.n;
    alpha_ = static_cast<dbltype>(n_-1)/n_;
    switch (n_-1) {
    case 1: funct_ = std::bind(Family02::FastPow1Inline, std::placeholders::_1, std::placeholders::_2); break;
    case 2: funct_ = std::bind(Family02::FastPow2Inline, std::placeholders::_1, std::placeholders::_2); break;
    case 3: funct_ = std::bind(Family02::FastPow3Inline, std::placeholders::_1, std::placeholders::_2); break;
    case 4: funct_ = std::bind(Family02::FastPow4Inline, std::placeholders::_1, std::placeholders::_2); break;
    case 5: funct_ = std::bind(Family02::FastPow5Inline, std::placeholders::_1, std::placeholders::_2); break;
    case 6: funct_ = std::bind(Family02::FastPow6Inline, std::placeholders::_1, std::placeholders::_2); break;
    default: funct_ = std::bind(Family02::FastPowNInline, std::placeholders::_1, std::placeholders::_2, n_-1);
    }
}

float Family04::CalcFinalNormMandelbrot(const cmplx &c) const
{
    int iter=1;
    dbltype rdem;
    dbltype x1, y1, x2, y2, tmp;
    dbltype x=0.0;
    dbltype y=0.0;
    dbltype dist  = std::numeric_limits<float>::max();
    for (; iter<max_iter_;++iter) {
        x1 = x; y1 = y;
        funct_(x,y); //z^(n-1)
        x2 = x1*x-y1*y; y2 = x1*y+y1*x; // z^n
        // dem
        x=n_*x+q_.real(); y=y*n_+q_.imag(); rdem = x*x+y*y;
        // num
        x2 += x1*q_.real()-y1*q_.imag()+c.real();
        y2 += x1*q_.imag()+y1*q_.real()+c.imag();
        tmp = x1 - (x2*x+y2*y)/rdem;
        y   = y1 - (y2*x-x2*y)/rdem; x = tmp;
        dist = std::min(dist, orbit_metric_funct_(x,y));
        rdem = std::max(std::abs(x1-x), std::abs(y1-y));
        if ( rdem < Fractal::kEps ) break;
    }
    return std::sqrt(dist);
}


float Family04::CalcFinalNormJulia(const cmplx &z) const {
    int iter=1;
    dbltype rdem;
    dbltype x1, y1, x2, y2, tmp;
    dbltype x=z.real();
    dbltype y=z.imag();
    dbltype dist  = std::numeric_limits<float>::max();
    for (; iter<max_iter_;++iter) {
        x1 = x; y1 = y;
        funct_(x,y); //z^(n-1)
        x2 = x1*x-y1*y; y2 = x1*y+y1*x; // z^n
        // dem
        x=n_*x+q_.real(); y=y*n_+q_.imag(); rdem = x*x+y*y;
        // num
        x2 += x1*q_.real()-y1*q_.imag()+c_.real();
        y2 += x1*q_.imag()+y1*q_.real()+c_.imag();
        tmp = x1 - (x2*x+y2*y)/rdem;
        y   = y1 - (y2*x-x2*y)/rdem; x = tmp;
        dist = std::min(dist, orbit_metric_funct_(x,y));
        rdem = std::max(std::abs(x1-x), std::abs(y1-y));
        if ( rdem < Fractal::kEps ) break;
    }
    return std::sqrt(dist);
}

//////////////////////////////////////////
float Family04::CalcEscapeJulia(const cmplx &z) const {
    int iter=1;
    dbltype rdem;
    dbltype x1, y1, x2, y2, tmp;
    dbltype x=z.real();
    dbltype y=z.imag();
    for (; iter<max_iter_;++iter) {
        x1 = x; y1 = y;
        funct_(x,y); //z^(n-1)
        x2 = x1*x-y1*y; y2 = x1*y+y1*x; // z^n
        // dem
        x=n_*x+q_.real(); y=y*n_+q_.imag(); rdem = x*x+y*y;
        // num
        x2 += x1*q_.real()-y1*q_.imag()+c_.real();
        y2 += x1*q_.imag()+y1*q_.real()+c_.imag();
        tmp = x1 - (x2*x+y2*y)/rdem;
        y   = y1 - (y2*x-x2*y)/rdem; x = tmp;
        rdem = std::max(std::abs(x1-x), std::abs(y1-y));
        if ( rdem < Fractal::kEps) break;
    }
    return iter;
}

float Family04::CalcEscapeMandelbrot(const cmplx &c) const {
//    return CalcEscapeJulia(c);
    int iter=1;
    dbltype rdem;
    dbltype x1, y1, x2, y2, tmp;
    dbltype x=0;
    dbltype y=0;
    for (; iter<max_iter_;++iter) {
        x1 = x; y1 = y;
        funct_(x,y); //z^(n-1)
        x2 = x1*x-y1*y; y2 = x1*y+y1*x; // z^n
        // dem
        x=n_*x+q_.real(); y=y*n_+q_.imag(); rdem = x*x+y*y;
        // num
        x2 += x1*q_.real()-y1*q_.imag()+c.real();
        y2 += x1*q_.imag()+y1*q_.real()+c.imag();
        tmp = x1 - (x2*x+y2*y)/rdem;
        y   = y1 - (y2*x-x2*y)/rdem; x = tmp;
        rdem = std::max(std::abs(x1-x), std::abs(y1-y));
        if ( rdem < Fractal::kEps) break;
    }
    return iter;
}
