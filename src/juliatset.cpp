#include "juliatset.h"
#include "iostream"
#include <numeric>
#include <limits>
#include <functional>

void Family00::Init(const FractalParameters &p) {
    Fractal::Init(p);
    c_.real(p.c.real());
    c_.imag(p.c.imag());

    q_=p.q.real();
}

float Family00::CalcFinalNormMandelbrot(const cmplx &c) const
{
    int iter=1;
    dbltype x, y, dem;
    dbltype xx, yy, tmp1, tmp2, tmp0, q2;

    x = c.real();
    y = c.imag();
    q2 = q_*q_;

    std::function<dbltype(const dbltype &x, const dbltype &y)> metric;
    switch (orbit_mode_) {
    case 0: metric =  std::bind(&Family00::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2); break;
    case 1: metric =  std::bind(&Family00::OrbitDiscance1, this, std::placeholders::_1, std::placeholders::_2); break;
    case 2: metric =  std::bind(&Family00::OrbitDiscance2, this, std::placeholders::_1, std::placeholders::_2); break;
    case 3: metric =  std::bind(&Family00::OrbitDiscance3, this, std::placeholders::_1, std::placeholders::_2); break;
    case 4: metric =  std::bind(&Family00::OrbitDiscance4, this, std::placeholders::_1, std::placeholders::_2); break;
    case 5: metric =  std::bind(&Family00::OrbitDiscance5, this, std::placeholders::_1, std::placeholders::_2); break;
    default: metric = std::bind(&Family00::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2);
    }

    dbltype dist  = std::numeric_limits<float>::max();
    for (; iter<max_iter_;++iter) {
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;

        dist = std::min(metric(tmp1, tmp2), dist);
        if (x+y>=th_norm_) break;

        x -= y;
        y  = 2*tmp1*tmp2;
        xx = x*x;
        yy = y*y;
        tmp0 = (xx + yy)*q2;
        dem  = tmp0 - 2*(q_*x) + 1;
        tmp1 = -x*tmp0-2*q_*yy  + x;
        tmp2 = -y*tmp0+2*q_*x*y + y;
        x = tmp1/dem + c.real();
        y = tmp2/dem + c.imag();
    }
    return std::sqrt(dist);
}


float Family00::CalcFinalNormJulia(const cmplx &z) const {
    int iter=0;
    dbltype x, y, dem;
    dbltype xx, yy, tmp1, tmp2, tmp0, q2;

    x = z.real();
    y = z.imag();
    q2 = q_*q_;

    std::function<dbltype(const dbltype &x, const dbltype &y)> metric;
    switch (orbit_mode_) {
    case 0: metric =  std::bind(&Family00::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2); break;
    case 1: metric =  std::bind(&Family00::OrbitDiscance1, this, std::placeholders::_1, std::placeholders::_2); break;
    case 2: metric =  std::bind(&Family00::OrbitDiscance2, this, std::placeholders::_1, std::placeholders::_2); break;
    case 3: metric =  std::bind(&Family00::OrbitDiscance3, this, std::placeholders::_1, std::placeholders::_2); break;
    case 4: metric =  std::bind(&Family00::OrbitDiscance4, this, std::placeholders::_1, std::placeholders::_2); break;
    case 5: metric =  std::bind(&Family00::OrbitDiscance5, this, std::placeholders::_1, std::placeholders::_2); break;
    default: metric = std::bind(&Family00::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2);
    }

    dbltype dist  = std::numeric_limits<dbltype>::max();
    for (; iter<max_iter_;++iter) {
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;

        dist = std::min(metric(tmp1, tmp2), dist);
        if (x+y>=th_norm_) break;
        x -= y;
        y  = 2*tmp1*tmp2;
        xx = x*x;
        yy = y*y;
        tmp0 = (xx + yy)*q2;
        dem  = tmp0 - 2*(q_*x) + 1;
        tmp1 = -x*tmp0-2*q_*yy  + x;
        tmp2 = -y*tmp0+2*q_*x*y + y;
        x = tmp1/dem + c_.real();
        y = tmp2/dem + c_.imag();
    }
    return std::sqrt(dist);
}

//////////////////////////////////////////
float Family00::CalcEscapeJulia(const cmplx &z) const {
    int iter=0;
    dbltype x, y, dem;
    dbltype xx, yy, tmp1, tmp2, tmp0, q2;

    x = z.real();
    y = z.imag();
    q2 = q_*q_;
    for (;;){
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;
        if (x+y >= th_norm_ || iter >= max_iter_) break;
        x -= y;
        y  = 2*tmp1*tmp2;
        xx = x*x;
        yy = y*y;
        tmp0 = (xx + yy)*q2;
        dem  = tmp0 - 2*(q_*x) + 1;
        tmp1 = -x*tmp0-2*q_*yy  + x;
        tmp2 = -y*tmp0+2*q_*x*y + y;
        x = tmp1/dem + c_.real();
        y = tmp2/dem + c_.imag();
        ++iter;
    }
    return iter;
}

float Family00::CalcEscapeMandelbrot(const cmplx &c) const {
    int iter=0;
    dbltype x, y, dem;
    dbltype xx, yy, tmp1, tmp2, tmp0, q2;

    x = 0.0;
    y = 0.0;
    q2 = q_*q_;
    for (;;){
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;
        if (x+y >= th_norm_ || iter >= max_iter_) break;
        x -= y;
        y  = 2*tmp1*tmp2;
        xx = x*x;
        yy = y*y;
        tmp0 = (xx + yy)*q2;
        dem  = tmp0 - 2*(q_*x) + 1;
        tmp1 = -x*tmp0-2*q_*yy  + x;
        tmp2 = -y*tmp0+2*q_*x*y + y;
        x = tmp1/dem + c.real();
        y = tmp2/dem + c.imag();
        ++iter;
    }
    return iter;
}
//////////////////////////////////////////////////////
//float Family00::CalcEscapeJuliaSmoth(cmplx z) const
//{
//    int iter=0;
//    dbltype x, y, dem;
//    dbltype xx, yy, tmp1, tmp2, tmp0, q2;

//    x = z.real();
//    y = z.imag();
//    q2 = q_*q_;
//    for (;;){
//        tmp1 = x;
//        tmp2 = y;
//        x*=x; y*=y;
//        if (x+y >= th_norm_ || iter >= max_iter_) break;
//        x -= y;
//        y  = 2*tmp1*tmp2;
//        xx = x*x;
//        yy = y*y;
//        tmp0 = (xx + yy)*q2;
//        dem  = tmp0 - 2*(q_*x) + 1;
//        tmp1 = -x*tmp0-2*q_*yy  + x;
//        tmp2 = -y*tmp0+2*q_*x*y + y;
//        x = tmp1/dem + c_.real();
//        y = tmp2/dem + c_.imag();
//        ++iter;
//    }
//    return iter + 1.0 - log(log2(x+y));
//}

//float Family00::CalcEscapeMandelbrotSmoth(cmplx c) const
//{
//    int iter=0;
//    dbltype x, y, dem;
//    dbltype xx, yy, tmp1, tmp2, tmp0, q2;

//    x = 0.0;
//    y = 0.0;
//    q2 = q_*q_;
//    for (;;){
//        tmp1 = x;
//        tmp2 = y;
//        x*=x; y*=y;
//        if (x+y >= th_norm_ || iter >= max_iter_) break;
//        x -= y;
//        y  = 2*tmp1*tmp2;
//        xx = x*x;
//        yy = y*y;
//        tmp0 = (xx + yy)*q2;
//        dem  = tmp0 - 2*(q_*x) + 1;
//        tmp1 = -x*tmp0-2*q_*yy  + x;
//        tmp2 = -y*tmp0+2*q_*x*y + y;
//        x = tmp1/dem + c.real();
//        y = tmp2/dem + c.imag();
//        ++iter;
//    }
//    return iter + 1.0 - log(log2(x+y));
//}
