#ifndef JULIATSET_H
#define JULIATSET_H
#include <complex>

using  cmplx = std::complex<long double>;

class JuliaSet {
    cmplx c_, orbit_pt_;
    long double q_;
    long double th_norm_;
    long double radius_;
    long double orbit_tangle_, orbit_dem_;
    int max_iter_;
    int n_;
    int orbit_mode_;


    long double OrbitDiscance0(const long double &x, const long double &y) const {
        return (orbit_pt_.real()-x)*(orbit_pt_.real()-x)+(orbit_pt_.imag()-y)*(orbit_pt_.imag()-y);
    }
    long double OrbitDiscance1(const long double &x, const long double &y) const {
        return std::abs(orbit_pt_.real()-x);
    }
    long double OrbitDiscance2(const long double &x, const long double &y) const {
        return std::abs(orbit_pt_.imag()-y);
    }
    long double OrbitDiscance3(const long double &x, const long double &y) const {
        return std::min(std::min( std::abs(-radius_-x), std::abs(radius_-x) ), std::min( std::abs(-radius_-y), std::abs(radius_-y) ));
    }
    long double OrbitDiscance4(const long double &x, const long double &y) const {
        return std::abs(x*x+y*y-th_norm_);
    }
    long double OrbitDiscance5(const long double &x, const long double &y) const {
        return std::abs(y-orbit_tangle_*x)/orbit_dem_;
    }

public:
    void SetOrbitPt(const cmplx &o, int orbit_mode){
        orbit_pt_   = o;
        orbit_mode_ = orbit_mode;

        orbit_tangle_ = std::tan(std::arg(orbit_pt_));
        orbit_dem_ = std::sqrt(1+orbit_tangle_*orbit_tangle_);
    }

    void Init(const cmplx &c, const long double &q,  const double R, const int max_iter, const int n);
    float CalcEscapeJulia(cmplx z) const;
    float CalcEscapeMandelbrot(cmplx c) const;
    float CalcEscapeJuliaSmoth(cmplx z) const;
    float CalcEscapeMandelbrotSmoth(cmplx c) const;
    float CalcFinalNormJulia(cmplx z) const;
    float CalcFinalNormMandelbrot(cmplx c) const;

};


#endif // JULIATSET_H
