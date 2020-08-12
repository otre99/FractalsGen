#include "juliatset.h"
#include "iostream"
#include <numeric>
#include <limits>
#include <functional>
void JuliaSet::Init(const cmplx &c, const long double &q,  const double R, const int max_iter, const int n)  {

    radius_ = R;
    th_norm_ = R*R;
    c_ = c;
    q_ = q;
    max_iter_ = max_iter;
    n_  = n;
    orbit_mode_ = 0;
    orbit_pt_ = {0,0};
}


float JuliaSet::CalcFinalNormMandelbrot(cmplx c) const
{
    int iter=1;
    long double x, y, dem;
    long double xx, yy, tmp1, tmp2, tmp0, q2;

    x = c.real();
    y = c.imag();
    q2 = q_*q_;

    std::function<long double(const long double &x, const long double &y)> metric;
    switch (orbit_mode_) {
    case 0: metric =  std::bind(&JuliaSet::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2); break;
    case 1: metric =  std::bind(&JuliaSet::OrbitDiscance1, this, std::placeholders::_1, std::placeholders::_2); break;
    case 2: metric =  std::bind(&JuliaSet::OrbitDiscance2, this, std::placeholders::_1, std::placeholders::_2); break;
    case 3: metric =  std::bind(&JuliaSet::OrbitDiscance3, this, std::placeholders::_1, std::placeholders::_2); break;
    case 4: metric =  std::bind(&JuliaSet::OrbitDiscance4, this, std::placeholders::_1, std::placeholders::_2); break;
    case 5: metric =  std::bind(&JuliaSet::OrbitDiscance5, this, std::placeholders::_1, std::placeholders::_2); break;
    default: metric = std::bind(&JuliaSet::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2);
    }

    long double dist  = std::numeric_limits<float>::max();
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


float JuliaSet::CalcFinalNormJulia(cmplx z) const {
    int iter=0;
    long double x, y, dem;
    long double xx, yy, tmp1, tmp2, tmp0, q2;

    x = z.real();
    y = z.imag();
    q2 = q_*q_;

    std::function<long double(const long double &x, const long double &y)> metric;
    switch (orbit_mode_) {
    case 0: metric =  std::bind(&JuliaSet::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2); break;
    case 1: metric =  std::bind(&JuliaSet::OrbitDiscance1, this, std::placeholders::_1, std::placeholders::_2); break;
    case 2: metric =  std::bind(&JuliaSet::OrbitDiscance2, this, std::placeholders::_1, std::placeholders::_2); break;
    case 3: metric =  std::bind(&JuliaSet::OrbitDiscance3, this, std::placeholders::_1, std::placeholders::_2); break;
    case 4: metric =  std::bind(&JuliaSet::OrbitDiscance4, this, std::placeholders::_1, std::placeholders::_2); break;
    case 5: metric =  std::bind(&JuliaSet::OrbitDiscance5, this, std::placeholders::_1, std::placeholders::_2); break;
    default: metric = std::bind(&JuliaSet::OrbitDiscance0, this, std::placeholders::_1, std::placeholders::_2);
    }

    long double dist  = std::numeric_limits<float>::max();
    for (; iter<max_iter_;++iter) {
        tmp1 = x;
        tmp2 = y;
        x*=x; y*=y;

        dist = std::min(metric(tmp1, tmp2), dist);

        //        std::cout << "dist0 " << dist;
        if (x+y>=th_norm_) break;
        //        std::cout << " dist1 " << dist << std::endl;
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
float JuliaSet::CalcEscapeJulia(cmplx z) const {
    int iter=0;
    long double x, y, dem;
    long double xx, yy, tmp1, tmp2, tmp0, q2;

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

float JuliaSet::CalcEscapeMandelbrot(cmplx c) const {
    int iter=0;
    long double x, y, dem;
    long double xx, yy, tmp1, tmp2, tmp0, q2;

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
float JuliaSet::CalcEscapeJuliaSmoth(cmplx z) const
{
    int iter=0;
    long double x, y, dem;
    long double xx, yy, tmp1, tmp2, tmp0, q2;

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
    return iter + 1.0 - log(log2(x+y));
}

float JuliaSet::CalcEscapeMandelbrotSmoth(cmplx c) const
{
    int iter=0;
    long double x, y, dem;
    long double xx, yy, tmp1, tmp2, tmp0, q2;

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
    return iter + 1.0 - log(log2(x+y));
}
